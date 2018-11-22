//	アンマネージ型で実装されるパイプ処理クラス
#include "NonClrPch.h"
#include "CollabAppDoc.h"
#include "PipeCommunication.h"

#include <atlfile.h>
#include <comdef.h>

const LPCTSTR	PIPE_NAME = _T( "DotNetLab\\201811" );
const DWORD		BUFF_SIZE	= 4000;

inline CString MakePipeName()
{
	CString pipeName( _T( "\\\\.\\pipe\\" ) );
	return pipeName + PIPE_NAME;
}
CPipeCommunication::CPipeCommunication()
	: m_hwndNotify( nullptr )
{
}
CPipeCommunication::~CPipeCommunication()
{
	if( m_hwndNotify != nullptr )
	{
		m_syncEvent.reset();	//	スレッドの終了監視
		CAtlFile pipe;
		auto hRes = pipe.Create( MakePipeName(), GENERIC_WRITE, 0, OPEN_EXISTING );
		if( SUCCEEDED( hRes ) )
		{
			m_syncEvent.wait();	//	スレッドの終了を待つ(安全に終了するための秘訣)
		}
	}
}
bool CPipeCommunication::InitializeCommunication()
{
	//	すでに初期化を済ませている場合がある
	if( m_hwndNotify != nullptr )
	{
		return true;
	}
	m_hwndNotify = *AfxGetMainWnd();
	m_syncEvent.reset();
	//	ここでスレッドを起こす(わかりにくいけど、別で動くのよ？)
	concurrency::CurrentScheduler::ScheduleTask( []( void* param )
	{
		CPipeCommunication* pThis = static_cast<CPipeCommunication*>(param);
		pThis->PipeKeepTask();
	}, this );
	m_syncEvent.wait();	//	初期化が無事終わるまで待機
	return m_hwndNotify != nullptr;
}

void CPipeCommunication::ExecuteOutProcessWork()
{
	if( m_hwndNotify != nullptr )
	{
		auto pwndMain = AfxGetMainWnd();
		//	タスクを立ち上げてバックグラウンドでスレッド通信する。
		TCHAR	execPath[MAX_PATH];
		GetModuleFileName( nullptr, execPath, MAX_PATH );
		lstrcpy( PathFindFileName( execPath ), _T( "PipeClientApp.exe" ) );
		ShellExecute( *pwndMain, _T( "open" ), execPath, PIPE_NAME, nullptr, SW_SHOWNORMAL );
	}
}
void CPipeCommunication::SendPipeMessage( const CString& msg, bool sendNotify )
{
	//	メッセージ文字列があれば、キューに追加する
	if( !msg.IsEmpty() )
	{
		m_queueMsg.push( msg );
	}
	//	通知要求があれば通知する(ここは非同期)
	if( sendNotify && !m_queueMsg.empty() )
	{
		_ASSERTE( m_hwndNotify != nullptr );
		::PostMessage( m_hwndNotify, WMA_UPDATE_FROM_THREAD, 0, CCollabAppDoc::UpdateHint::GetPipeMsg );
	}
}

void CPipeCommunication::PipeKeepTask()
{
	_ASSERTE( m_hwndNotify != nullptr );
	m_syncEvent.set();	//	スレッドが無事に稼働し始めたので、処理を継続できるようにする

	while( m_hwndNotify != nullptr && m_syncEvent.wait( 0 ) != concurrency::COOPERATIVE_WAIT_TIMEOUT )
	{
		//	パイプハンドルを作成する
		CAtlFile pipe( CreateNamedPipe(
			MakePipeName(),					// パイプの名前
			PIPE_ACCESS_INBOUND,		// 一方的な受信のみ
			PIPE_TYPE_MESSAGE |			// 書き込みモード(メッセージモードはワード単位)
			PIPE_READMODE_MESSAGE |		// 読み取りモード
			PIPE_WAIT,					// ブロッキングモード(PIPE_NOWAITは今は使わないので必ずブロッキングモードにする)
			PIPE_UNLIMITED_INSTANCES,	// 最大インスタンス数は無制限
			BUFF_SIZE,					// 読み取り用バッファサイズ
			BUFF_SIZE,					// 書き込み用バッファサイズ
			0,							// クライアントがWaitNamedPipe で NMPWAIT_USE_DEFAULT_WAIT を指定した時の待ち時間
			nullptr ) );					// セキュリティ記述子(SECURITY_ATTRIBUTES*)
		//	パイプハンドルが作成できなくなったら終了
		if( pipe == INVALID_HANDLE_VALUE )
		{
			auto lastError = GetLastError();
			_com_error err( HRESULT_FROM_WIN32( lastError ) );
			SendPipeMessage( err.ErrorMessage(), false );
			break;
		}
		auto pipeConnected = ConnectNamedPipe( pipe, nullptr );
		//	タイムアウトした場合は終了待機モードになっている場合
		if( m_syncEvent.wait( 0 ) == concurrency::COOPERATIVE_WAIT_TIMEOUT )
		{
			break;
		}
		else if( !pipeConnected )
		{
			auto errorCode = GetLastError();
			//	CreateNamedPipeした時点でパイプにつながっていた場合(クライアント側が作るほうが先の場合)
			if( errorCode == ERROR_PIPE_CONNECTED )
			{
				pipeConnected = TRUE;
			}
			else
			{
				_com_error err( HRESULT_FROM_WIN32( errorCode ) );
				SendPipeMessage( err.ErrorMessage(), true );
			}
		}
		if( pipeConnected )
		{
			while( m_syncEvent.wait( 0 ) != concurrency::COOPERATIVE_WAIT_TIMEOUT )
			{
				wchar_t	buffer[BUFF_SIZE];
				DWORD sendLength;
				DWORD	readed;
				auto result = pipe.Read( &sendLength, sizeof( sendLength ), readed );
				if( !CheckResult( result, readed ) )
				{
					break;
				}
				//	ここは本当は尺取り虫をしないと駄目だけどサンプルなので手を抜く
				result = pipe.Read( buffer, sendLength, readed );
				if( !CheckResult( result, readed ) )
				{
					break;
				}
				buffer[sendLength/sizeof( wchar_t )] = L'\0';
				SendPipeMessage( buffer, true );
			}
			DisconnectNamedPipe( pipe );
		}
	}
	if( m_hwndNotify != nullptr && IsExistPipeMessage() )
	{
		//	スレッドを終了する前に処理してもらう必要がある
		::SendMessage( m_hwndNotify, WMA_UPDATE_FROM_THREAD, 0, CCollabAppDoc::UpdateHint::GetPipeMsg );
	}
	//	スレッドを終了するので、もろもろクリーンナップ
	m_hwndNotify = nullptr;
	m_syncEvent.set();
}
bool CPipeCommunication::CheckResult( HRESULT hRes, DWORD readLength )
{
	//	さらにデータがある場合は成功とみなしてよい
	if( HRESULT_CODE( hRes ) == ERROR_MORE_DATA )
	{
		hRes = S_OK;	//	まだデータがある場合は成功とみなす
	}
	if( FAILED( hRes ) || readLength == 0 )
	{
		if( FAILED( hRes ) && HRESULT_CODE( hRes ) != ERROR_BROKEN_PIPE )
		{
			_com_error err( hRes );
			SendPipeMessage( err.ErrorMessage(), true );
		}
		return false;
	}
	//	読み取りの途中でも終了通知が来てればそこで終わる
	return (m_syncEvent.wait( 0 ) != concurrency::COOPERATIVE_WAIT_TIMEOUT);
}
