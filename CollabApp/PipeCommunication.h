#pragma once


#include <concurrent_queue.h>
#include <atomic>

#define WMA_UPDATE_FROM_THREAD	(WM_APP+1)

class CPipeCommunication
{
public:
	CPipeCommunication();
	~CPipeCommunication();

public:
	bool	InitializeCommunication();	//	連動用パイプ通知管理オブジェクトを構築

	void	ExecuteOutProcessWork();	//	連動処理

	bool	IsExistPipeMessage() const
	{
		return m_queueMsg.empty() == false;
	}
	bool GetPipeMessage( CString& msg )
	{
		return m_queueMsg.try_pop( msg );
	}
	void	SendPipeMessage( const CString& msg, bool sendNotify = true );
private:
	void	PipeKeepTask();	//	パイプ処理をキープするタスク
	bool	CheckResult( HRESULT hRes, DWORD readLength );
private:
	std::atomic<HWND>	m_hwndNotify;	//	UIスレッド側に同期させるためのウィンドウハンドル
	concurrency::event	m_syncEvent;	//	いろいろ同期用イベント(InitializeCommunicationでは、初期化終了。それ以外はスレッド終了通知)
	concurrency::concurrent_queue<CString> m_queueMsg;
};

