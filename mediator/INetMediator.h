#pragma once

class INet;
class INetMediator
{

public:
	INetMediator(){}
	virtual~INetMediator() {}
	virtual bool OpenNet() = 0;
	virtual void CloseNet() = 0;
	// UDP由ip地址决定发给谁，ip是ulong类型,TCP由soclet决定发给谁，socket是SOCKET类型的，实际上SOCKET就是uint
	// ulong和uint就可以用ulong类型传参数

	virtual bool SendData(char* buf, int nLen, long lSend) = 0;
	virtual void DealData(char* buf, int nLen, long lFrom) = 0;

protected:
	INet* m_pNet;
};