#pragma once
#include<iostream>
#include<WinSock2.h>

using namespace std;
#pragma comment(lib,"Ws2_32.lib")

class INetMediator;//先声明有这个类，但是不用去编译这个类

class INet
{

public:
	INet(){}
	virtual ~INet(){}
	virtual bool InitNet() = 0;
	virtual void UnInitNet() = 0;
	// UDP由ip地址决定发给谁，ip是ulong类型,TCP由soclet决定发给谁，socket是SOCKET类型的，实际上SOCKET就是uint
	//ulong和uint就可以用ulong类型传参数
	virtual bool SendData(char* buf, int nLen, long lSend) = 0; 

protected:
	virtual void RecvData() = 0;
	INetMediator* m_pMediator;

	
};