#pragma once
#include"INet.h"
#include<map>
#include<list>
class TcpServerNet :public INet
{

public:
	TcpServerNet(INetMediator* pMediator);
	~TcpServerNet();
	bool InitNet();
	void UnInitNet();
	bool SendData(char* buf, int nLen, long lSend);
protected:
	static unsigned __stdcall RecvThread(void* lpVoid);
	static unsigned __stdcall AcceptThread(void* lpVoid);
	SOCKET m_sock;
	void RecvData();
	bool m_bStop;
	map<unsigned int, SOCKET>m_mapThreadIdToSocket;
	list<HANDLE> m_listHandle;

};