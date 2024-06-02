#include "TcpClientMediator.h"
#include"TcpClient.h"
TcpClientMediator::TcpClientMediator()
{
	m_pNet = new TcpClientNet(this);
}

TcpClientMediator::~TcpClientMediator()
{
	m_pNet->UnInitNet();
}

bool TcpClientMediator::OpenNet()
{
	return m_pNet->InitNet();
}

void TcpClientMediator::CloseNet()
{
	m_pNet->UnInitNet();
}

bool TcpClientMediator::SendData(char* buf, int nLen, long lSend)
{
	return m_pNet->SendData(buf, nLen, lSend);
}

void TcpClientMediator::DealData(char* buf, int nLen, long lFrom)
{
	cout << "TcpClientMediator::DealData " << buf << endl;
}
