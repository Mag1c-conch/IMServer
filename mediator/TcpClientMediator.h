#pragma once
#include"INetMediator.h"

class TcpClientMediator :public INetMediator
{
public:
	TcpClientMediator();
	~TcpClientMediator();
	bool OpenNet();
	void CloseNet();

	bool SendData(char* buf, int nLen, long lSend);
	void DealData(char* buf, int nLen, long lFrom);
};