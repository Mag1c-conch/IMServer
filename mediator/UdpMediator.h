#pragma once
#include"INetMediator.h"

class UdpMediator :public INetMediator
{
public:
	UdpMediator();
	~UdpMediator();
	bool OpenNet();
	void CloseNet();
	// UDP��ip��ַ��������˭��ip��ulong����,TCP��soclet��������˭��socket��SOCKET���͵ģ�ʵ����SOCKET����uint
	//ulong��uint�Ϳ�����ulong���ʹ�����

	bool SendData(char* buf, int nLen, long lSend);
	void DealData(char* buf, int nLen, long lFrom);
};