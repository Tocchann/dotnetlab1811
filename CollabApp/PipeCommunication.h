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
	bool	InitializeCommunication();	//	�A���p�p�C�v�ʒm�Ǘ��I�u�W�F�N�g���\�z

	void	ExecuteOutProcessWork();	//	�A������

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
	void	PipeKeepTask();	//	�p�C�v�������L�[�v����^�X�N
	bool	CheckResult( HRESULT hRes, DWORD readLength );
private:
	std::atomic<HWND>	m_hwndNotify;	//	UI�X���b�h���ɓ��������邽�߂̃E�B���h�E�n���h��
	concurrency::event	m_syncEvent;	//	���낢�듯���p�C�x���g(InitializeCommunication�ł́A�������I���B����ȊO�̓X���b�h�I���ʒm)
	concurrency::concurrent_queue<CString> m_queueMsg;
};

