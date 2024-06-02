#pragma once
#include"INetMediator.h"

class TcpServerMediator :public INetMediator
{
public:
	TcpServerMediator();
	~TcpServerMediator();
	bool OpenNet();
	void CloseNet();

	bool SendData(char* buf, int nLen, long lSend);
	void DealData(char* buf, int nLen, long lFrom);
};