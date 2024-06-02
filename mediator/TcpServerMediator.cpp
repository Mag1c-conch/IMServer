#include "TcpServerMediator.h"
#include"TcpServer.h"
#include"../CKernel.h"
TcpServerMediator::TcpServerMediator()
{
	m_pNet = new TcpServerNet(this);
}

TcpServerMediator::~TcpServerMediator()
{
	m_pNet->UnInitNet();
}

bool TcpServerMediator::OpenNet()
{
	return m_pNet->InitNet();
}

void TcpServerMediator::CloseNet()
{
	m_pNet->UnInitNet ();
}

bool TcpServerMediator::SendData(char* buf, int nLen, long lSend)
{
	return m_pNet->SendData(buf, nLen, lSend);
}

void TcpServerMediator::DealData(char* buf, int nLen, long lFrom)
{
	//cout << "TcpServerMediator::DealData " << buf << endl;
	//// 给客户端发给消息
	//SendData("TcpServerMediator hello", sizeof("TcpServerMediator hello"), lFrom);
	CKernel::pKernel->readyData(buf, nLen, lFrom);
}
