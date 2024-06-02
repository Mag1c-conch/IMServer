#pragma once
#include"INet.h"

class UdpNet :public INet
{

public:
	UdpNet(INetMediator* pMediator);
	~UdpNet();
	bool InitNet();
	void UnInitNet();
	bool SendData(char* buf, int nLen, long lSend);
protected:
	static unsigned __stdcall RecvThread(void* lpVoid);
	SOCKET m_sock;
	void RecvData();
	bool m_bStop;
	HANDLE m_handle;

};