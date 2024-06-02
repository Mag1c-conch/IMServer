#pragma once
#include<iostream>
#include<WinSock2.h>

using namespace std;
#pragma comment(lib,"Ws2_32.lib")

class INetMediator;//������������࣬���ǲ���ȥ���������

class INet
{

public:
	INet(){}
	virtual ~INet(){}
	virtual bool InitNet() = 0;
	virtual void UnInitNet() = 0;
	// UDP��ip��ַ��������˭��ip��ulong����,TCP��soclet��������˭��socket��SOCKET���͵ģ�ʵ����SOCKET����uint
	//ulong��uint�Ϳ�����ulong���ʹ�����
	virtual bool SendData(char* buf, int nLen, long lSend) = 0; 

protected:
	virtual void RecvData() = 0;
	INetMediator* m_pMediator;

	
};