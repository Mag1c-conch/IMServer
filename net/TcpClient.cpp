#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "TcpClient.h"
#include"PackDef.h"
#include<process.h>
#include"TcpClientMediator.h"

TcpClientNet::TcpClientNet(INetMediator* pMediator):m_sock(INVALID_SOCKET),m_handle(NULL),m_bStop(false)
{
	m_pMediator = pMediator;
}

TcpClientNet::~TcpClientNet()
{
	UnInitNet();
}

bool TcpClientNet::InitNet()
{
	//1、加载库
	WSAData data;
	WORD wVersion = MAKEWORD(2,2);
	int err = WSAStartup(wVersion, &data);
	if (0 != err){
		cout << "WSAStartup error"<< endl ;
			return false;
	}
	if (2 != LOBYTE(data.wVersion) || 2 != HIBYTE(data.wVersion))
	{
		cout << "WSAStartup version error" << endl;
		return false;
	}
	else
	{
		cout << "WSAStartup success" << endl;
	}

	//2、创建套接字
	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_sock)
	{
		cout << " socket error" << endl;
		return false;
	}
	else
	{
		cout << "socket success" << endl;
	}
	// 3、连接
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(_DEF_TCP_PORT);
	serverAddr.sin_addr.S_un.S_addr = inet_addr(_DEF_TCP_SERVER_IP);
	err = connect(m_sock, (sockaddr*)&serverAddr, sizeof(serverAddr));
	if (SOCKET_ERROR == err)
	{
		cout << "connect error" << endl;
		return false;
	}
	else
	{
		cout << "connect success" << endl;
	}

	// 4、创建接收数据的线程
	m_handle = (HANDLE)_beginthreadex(NULL, 0, &RecvThread, this, 0, NULL);

	return true;
}

void TcpClientNet::UnInitNet()
{
	//1、回收内核空间(结束线程和关闭句柄)
	m_bStop = true;
	if (m_handle)
	{
		if (WAIT_TIMEOUT == WaitForSingleObject(m_handle, 100))
		{
			TerminateThread(m_handle, -1);
		}
		CloseHandle(m_handle);
		m_handle = NULL;
	}

	//2、关闭套接字
	if (m_sock && INVALID_SOCKET != m_sock)
	{
		closesocket(m_sock);
	}
	//3、卸载库
	WSACleanup();
}

bool TcpClientNet::SendData(char* buf, int nLen, long lSend)
{

	// 1、判断参数是否合法
	if (!buf || nLen <= 0)
	{
		cout << "TcpClientNet::SendaData 参数错误" << endl;
		return false;
	}

	//2、先发包大小
	if (send(m_sock, (char*)&nLen, sizeof(int), 0) <= 0)
	{
		cout << "TcpClientNet::SendaData send error:" << WSAGetLastError() << endl;
		return false;
	}

	//3、再发包内容
	if (send(m_sock, buf, nLen, 0) <= 0)
	{
		cout << "TcpClientNet::Sendaata send error:" << WSAGetLastError() << endl;
		return false;
	}
		
	return true;
}

unsigned __stdcall TcpClientNet::RecvThread(void* lpVoid)
{
	TcpClientNet* pThis = (TcpClientNet*)lpVoid;
	pThis->RecvData();
	return 0;
}

void TcpClientNet::RecvData()
{
	int nPackSize = 0;
	int nRecvNum = 0;
	//记录当前累积接收到多少数据
	int nOffset = 0;
	while (!m_bStop)
	{
		//1、接收包大小
		
		nRecvNum = recv(m_sock, (char*)&nPackSize, sizeof(int), 0);
		if (nRecvNum > 0)
		{
			// 2、按照包大小new接收数据的空间
			char* buf = new char[nPackSize];
			// 3、接收包内容
			nOffset = 0;
			while (nPackSize > 0)
			{
				nRecvNum = recv(m_sock, buf + nOffset, nPackSize, 0);
				if (nRecvNum > 0)
				{
					nOffset += nRecvNum;
					nPackSize -= nRecvNum;
				}
				else
				{
					cout << "TcpClientNet::RecvData recv error :" << WSAGetLastError() << endl;
					return;
				}
			}
			//4、接收数据成功，把数据发给中介者类
			m_pMediator->DealData(buf, nOffset, m_sock);
		}
		else
		{
			cout << "TcpClientNet::RecvData recv error :" << WSAGetLastError() << endl;
			break;
		}
	
	}
}
