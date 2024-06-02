#pragma once
#include"INetMediator.h"

class UdpMediator :public INetMediator
{
public:
	UdpMediator();
	~UdpMediator();
	bool OpenNet();
	void CloseNet();
	// UDP由ip地址决定发给谁，ip是ulong类型,TCP由soclet决定发给谁，socket是SOCKET类型的，实际上SOCKET就是uint
	//ulong和uint就可以用ulong类型传参数

	bool SendData(char* buf, int nLen, long lSend);
	void DealData(char* buf, int nLen, long lFrom);
};