#pragma once

class INet;
class INetMediator
{

public:
	INetMediator(){}
	virtual~INetMediator() {}
	virtual bool OpenNet() = 0;
	virtual void CloseNet() = 0;
	// UDP��ip��ַ��������˭��ip��ulong����,TCP��soclet��������˭��socket��SOCKET���͵ģ�ʵ����SOCKET����uint
	// ulong��uint�Ϳ�����ulong���ʹ�����

	virtual bool SendData(char* buf, int nLen, long lSend) = 0;
	virtual void DealData(char* buf, int nLen, long lFrom) = 0;

protected:
	INet* m_pNet;
};