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
// UDP��ip��ַ��������˭��ip��ulong����, TCP��socket��������˭��socket��SOCKET���͵ģ�ʵ����SOCKET����uint
//ulong��uint�Ϳ�����ulong���ʹ�����
bool UdpMediator::SendData(char* buf, int nLen, long lSend)
{
	return m_pNet->SendData(buf,nLen,lSend);
}

void UdpMediator::DealData(char* buf, int nLen, long lFrom)
{
	//����kernel��
	// ���Դ��룬��ӡ���յ�������

	cout << "buf:" << buf << endl;
}
