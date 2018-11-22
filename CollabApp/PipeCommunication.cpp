//	�A���}�l�[�W�^�Ŏ��������p�C�v�����N���X
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
		m_syncEvent.reset();	//	�X���b�h�̏I���Ď�
		CAtlFile pipe;
		auto hRes = pipe.Create( MakePipeName(), GENERIC_WRITE, 0, OPEN_EXISTING );
		if( SUCCEEDED( hRes ) )
		{
			m_syncEvent.wait();	//	�X���b�h�̏I����҂�(���S�ɏI�����邽�߂̔錍)
		}
	}
}
bool CPipeCommunication::InitializeCommunication()
{
	//	���łɏ��������ς܂��Ă���ꍇ������
	if( m_hwndNotify != nullptr )
	{
		return true;
	}
	m_hwndNotify = *AfxGetMainWnd();
	m_syncEvent.reset();
	//	�����ŃX���b�h���N����(�킩��ɂ������ǁA�ʂœ����̂�H)
	concurrency::CurrentScheduler::ScheduleTask( []( void* param )
	{
		CPipeCommunication* pThis = static_cast<CPipeCommunication*>(param);
		pThis->PipeKeepTask();
	}, this );
	m_syncEvent.wait();	//	�������������I���܂őҋ@
	return m_hwndNotify != nullptr;
}

void CPipeCommunication::ExecuteOutProcessWork()
{
	if( m_hwndNotify != nullptr )
	{
		auto pwndMain = AfxGetMainWnd();
		//	�^�X�N�𗧂��グ�ăo�b�N�O���E���h�ŃX���b�h�ʐM����B
		TCHAR	execPath[MAX_PATH];
		GetModuleFileName( nullptr, execPath, MAX_PATH );
		lstrcpy( PathFindFileName( execPath ), _T( "PipeClientApp.exe" ) );
		ShellExecute( *pwndMain, _T( "open" ), execPath, PIPE_NAME, nullptr, SW_SHOWNORMAL );
	}
}
void CPipeCommunication::SendPipeMessage( const CString& msg, bool sendNotify )
{
	//	���b�Z�[�W�����񂪂���΁A�L���[�ɒǉ�����
	if( !msg.IsEmpty() )
	{
		m_queueMsg.push( msg );
	}
	//	�ʒm�v��������Βʒm����(�����͔񓯊�)
	if( sendNotify && !m_queueMsg.empty() )
	{
		_ASSERTE( m_hwndNotify != nullptr );
		::PostMessage( m_hwndNotify, WMA_UPDATE_FROM_THREAD, 0, CCollabAppDoc::UpdateHint::GetPipeMsg );
	}
}

void CPipeCommunication::PipeKeepTask()
{
	_ASSERTE( m_hwndNotify != nullptr );
	m_syncEvent.set();	//	�X���b�h�������ɉғ����n�߂��̂ŁA�������p���ł���悤�ɂ���

	while( m_hwndNotify != nullptr && m_syncEvent.wait( 0 ) != concurrency::COOPERATIVE_WAIT_TIMEOUT )
	{
		//	�p�C�v�n���h�����쐬����
		CAtlFile pipe( CreateNamedPipe(
			MakePipeName(),					// �p�C�v�̖��O
			PIPE_ACCESS_INBOUND,		// ����I�Ȏ�M�̂�
			PIPE_TYPE_MESSAGE |			// �������݃��[�h(���b�Z�[�W���[�h�̓��[�h�P��)
			PIPE_READMODE_MESSAGE |		// �ǂݎ�胂�[�h
			PIPE_WAIT,					// �u���b�L���O���[�h(PIPE_NOWAIT�͍��͎g��Ȃ��̂ŕK���u���b�L���O���[�h�ɂ���)
			PIPE_UNLIMITED_INSTANCES,	// �ő�C���X�^���X���͖�����
			BUFF_SIZE,					// �ǂݎ��p�o�b�t�@�T�C�Y
			BUFF_SIZE,					// �������ݗp�o�b�t�@�T�C�Y
			0,							// �N���C�A���g��WaitNamedPipe �� NMPWAIT_USE_DEFAULT_WAIT ���w�肵�����̑҂�����
			nullptr ) );					// �Z�L�����e�B�L�q�q(SECURITY_ATTRIBUTES*)
		//	�p�C�v�n���h�����쐬�ł��Ȃ��Ȃ�����I��
		if( pipe == INVALID_HANDLE_VALUE )
		{
			auto lastError = GetLastError();
			_com_error err( HRESULT_FROM_WIN32( lastError ) );
			SendPipeMessage( err.ErrorMessage(), false );
			break;
		}
		auto pipeConnected = ConnectNamedPipe( pipe, nullptr );
		//	�^�C���A�E�g�����ꍇ�͏I���ҋ@���[�h�ɂȂ��Ă���ꍇ
		if( m_syncEvent.wait( 0 ) == concurrency::COOPERATIVE_WAIT_TIMEOUT )
		{
			break;
		}
		else if( !pipeConnected )
		{
			auto errorCode = GetLastError();
			//	CreateNamedPipe�������_�Ńp�C�v�ɂȂ����Ă����ꍇ(�N���C�A���g�������ق�����̏ꍇ)
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
				//	�����͖{���͎ڎ�蒎�����Ȃ��Ƒʖڂ����ǃT���v���Ȃ̂Ŏ�𔲂�
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
		//	�X���b�h���I������O�ɏ������Ă��炤�K�v������
		::SendMessage( m_hwndNotify, WMA_UPDATE_FROM_THREAD, 0, CCollabAppDoc::UpdateHint::GetPipeMsg );
	}
	//	�X���b�h���I������̂ŁA�������N���[���i�b�v
	m_hwndNotify = nullptr;
	m_syncEvent.set();
}
bool CPipeCommunication::CheckResult( HRESULT hRes, DWORD readLength )
{
	//	����Ƀf�[�^������ꍇ�͐����Ƃ݂Ȃ��Ă悢
	if( HRESULT_CODE( hRes ) == ERROR_MORE_DATA )
	{
		hRes = S_OK;	//	�܂��f�[�^������ꍇ�͐����Ƃ݂Ȃ�
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
	//	�ǂݎ��̓r���ł��I���ʒm�����Ă�΂����ŏI���
	return (m_syncEvent.wait( 0 ) != concurrency::COOPERATIVE_WAIT_TIMEOUT);
}
