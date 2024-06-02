#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "TcpServer.h"
#include"TcpServerMediator.h"
#include"PackDef.h"
#include <process.h>

TcpServerNet::TcpServerNet(INetMediator* pMediator) :m_sock(INVALID_SOCKET), m_bStop(false)
{
	m_pMediator = pMediator;
}

TcpServerNet::~TcpServerNet()
{
	UnInitNet();
}
// TCP客户端初始化网络: 1、加载库 2、创建套接字 3、连接 4、创建接收数据的线程
bool TcpServerNet::InitNet()
{
	// 1、加载库
	WSAData data;
	WORD wVersion = MAKEWORD(2, 2);
	int err = WSAStartup(wVersion, &data);
	if (0 != err) {
		cout << "load dll fail" << endl;
		return false;
	}
	if (2 != LOBYTE(data.wVersion) || 2 != HIBYTE(data.wVersion))
	{
		cout << "dll version error" << endl;
		return false;
	}
	else
	{
		cout << "load dll success" << endl;
	}
	// 2、创建套接字
	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_sock)
	{
		cout << " socket fail" << endl;
		return false;
	}
	else
	{
		cout << "socket success" << endl;
	}
	// 3、绑定ip地址
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_DEF_TCP_PORT);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	err = bind(m_sock, (sockaddr*)&addr, sizeof(addr));
	if (SOCKET_ERROR == err)
	{
		cout << "bind fail" << endl;
		return false;
	}
	else
	{
		cout << "bind success" << endl;
	}
	// 4、监听
	err = listen(m_sock, 10);
	if (SOCKET_ERROR == err)
	{
		cout << "listen fail" << WSAGetLastError() << endl;
		return false;

	}
	else
	{
		cout << "listen success"<< endl;
	}
	//5、创建接受连接的线程
	HANDLE handle = (HANDLE)_beginthreadex(NULL, 0 ,&AcceptThread, this,0,NULL);
	if (handle)
	{
		m_listHandle.push_back(handle);
	}
	return true;
}

void TcpServerNet::UnInitNet()
{
	// 1、回收内核空间(结束线程和关闭句柄)
	m_bStop = true;
	HANDLE handle;
	for (auto ite = m_listHandle.begin(); ite != m_listHandle.end();)
	{
		handle = *ite;
		if (handle)
		{
			if (WAIT_TIMEOUT == WaitForSingleObject(handle, 100))
			{
				TerminateThread(handle, -1);
			}
			CloseHandle(handle);
			handle = NULL;
		}
		//把当前节点从list里面删除
		ite = m_listHandle.erase(ite);
	}

	//2、关闭套接字
	if (m_sock && INVALID_SOCKET != m_sock)
	{
		closesocket(m_sock);
	}
	SOCKET sock = INVALID_SOCKET;
	for (auto ite = m_mapThreadIdToSocket.begin(); ite != m_mapThreadIdToSocket.end();)
	{
		sock = ite->second;
		if (sock && INVALID_SOCKET != sock)
		{
			closesocket(sock);
		}
		// 把当前节点从map里面删除
		ite = m_mapThreadIdToSocket.erase(ite);
	}
	// 3、卸载库
	WSACleanup();
}

bool TcpServerNet::SendData(char* buf, int nLen, long lSend)
{
	//1、判断参数是否合法
	if (!buf || nLen <= 0)
	{
		cout << "TcpServerNet::SendaData 参数错误" << endl;
		return false;

	}
	//2、先发包大小
	if (send(lSend, (char*)&nLen, sizeof(int), 0) <= 0)
	{
		cout << "TcpServerNet::SendaData send error:" << WSAGetLastError() << endl;
		return false;
	}

	//3、再发包内容
	if (send(lSend, buf, nLen, 0) <= 0)
	{
		cout << "TcpServerNet::Sendaata send error:" << WSAGetLastError() << endl;
		return false;
	}
	return true;
}

unsigned __stdcall TcpServerNet::RecvThread(void* lpVoid)
{
	TcpServerNet* pThis = (TcpServerNet*)lpVoid;
	pThis->RecvData();
	return 1;
}

unsigned __stdcall TcpServerNet::AcceptThread(void* lpVoid)
{
	TcpServerNet* pThis = (TcpServerNet*)lpVoid;
	sockaddr_in clientAddr;
	int clientAddrSize = sizeof(clientAddr);
	unsigned int threadId = 0;
	SOCKET sock = INVALID_SOCKET;
	HANDLE handle = NULL;
	while (!pThis->m_bStop)
	{
		// 接收连接
		sock = accept(pThis->m_sock, (sockaddr*)&clientAddr, &clientAddrSize);
		if (INVALID_SOCKET == sock)
		{
			// 出错，打印日志
			cout << "TcpServerNet::AcceptThread error:" << WSAGetLastError() << endl;
			break;
		}
		else
		{
			//一个客户端连接成功
			cout << "client ip:" << inet_ntoa(clientAddr.sin_addr) << endl;
			//创建一个接收这个客户端数据的线程
			handle = (HANDLE)_beginthreadex(NULL, 0 ,& RecvThread, pThis, 0, &threadId);
			if (handle)
			{
				pThis->m_listHandle.push_back(handle);
			}
			// 保存线程id和对应的socket
			pThis->m_mapThreadIdToSocket[threadId] = sock;
		}
	}
	return 1;
}

void TcpServerNet::RecvData()
{
	//先休眠一会，防止socket还没有存入map中就来取数据了
	Sleep(100);

	//从map中取出socket
	SOCKET sock = m_mapThreadIdToSocket[GetCurrentThreadId()];

	//判断socket的合法性
	if (!sock || INVALID_SOCKET == sock)
	{
		cout << "TcpServerNet::RecvData socket error" << endl;
		return;
	}
	int nPackSize = 0;
	int nRecvNum = 0;
	//记录当前累积接收到多少数据
	int nOffset = 0;
	while (!m_bStop)
	{
		//1、接收包大小

		nRecvNum = recv(sock, (char*)&nPackSize, sizeof(int), 0);
		if (nRecvNum > 0)
		{
			// 2、按照包大小new接收数据的空间
			char* buf = new char[nPackSize];
			// 3、接收包内容
			nOffset = 0;
			while (nPackSize > 0)
			{
				nRecvNum = recv(sock, buf + nOffset, nPackSize, 0);
				if (nRecvNum > 0)
				{
					nOffset += nRecvNum;
					nPackSize -= nRecvNum;
				}
				else
				{
					cout << "TcpServerNet::RecvData recv error :" << WSAGetLastError() << endl;
					return;
				}
			}
			//4、接收数据成功，把数据发给中介者类
			m_pMediator->DealData(buf, nOffset, sock);
		}
		else
		{
			cout << "TcpServerNet::RecvData recv error :" << WSAGetLastError() << endl;
			break;
		}

	}
}
