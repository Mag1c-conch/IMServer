#include "UdpMediator.h"
#include"../net/UdpNet.h"

UdpMediator::UdpMediator()
{
	m_pNet = new UdpNet(this);
}

UdpMediator::~UdpMediator()
{
	if (m_pNet)
	{
		delete m_pNet;
		m_pNet = nullptr;
	}

}

bool UdpMediator::OpenNet()
{
	return m_pNet->InitNet();
}

void UdpMediator::CloseNet()
{
	m_pNet->UnInitNet();
}
// UDP由ip地址决定发给谁，ip是ulong类型, TCP由socket决定发给谁，socket是SOCKET类型的，实际上SOCKET就是uint
//ulong和uint就可以用ulong类型传参数
bool UdpMediator::SendData(char* buf, int nLen, long lSend)
{
	return m_pNet->SendData(buf,nLen,lSend);
}

void UdpMediator::DealData(char* buf, int nLen, long lFrom)
{
	//传给kernel类
	// 测试代码，打印接收到的数据

	cout << "buf:" << buf << endl;
}
