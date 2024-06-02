#include "UdpNet.h"
#include"../mediator/UdpMediator.h"

#include<process.h>
#include "PackDef.h"


UdpNet::UdpNet(INetMediator* pMediator):m_sock(INVALID_SOCKET),m_bStop(false),m_handle(NULL)
{
	m_pMediator = pMediator;
}

UdpNet::~UdpNet()
{
	UnInitNet();
}
// 初始化网络:加载库、创建套接字、绑定ip地址、申请广播权限、创建一个接收数据的线程
bool UdpNet::InitNet()
{
	//1、加载库
	WORD version = MAKEWORD(2, 2);//制造一个word类型数据
	WSAData data;

	int error = WSAStartup(version, &data);
	if (0 != error)
	{
		cout << "WSAStartup error" << endl;
		return 1;
	}

	//判断加载的库版本是否是2.2版本
	if (2 != LOBYTE(data.wVersion) || 2 != HIBYTE(data.wVersion))
	{
		cout << "version error " << endl;
		
		return false;

	}
	else
	{
		cout << "WSAStartup success" << endl;
	}

	//2、创建套接字
	m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == m_sock)
	{
		cout << "socket error:" << WSAGetLastError() << endl;
		
		return false;
	}
	else
	{
		cout << "socket success" << endl;
	}


	//3、绑定ip地址一一告诉操作系统，从哪些网卡以及发送给哪个端口的数据是当前应用程序需要的数据

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_DEF_UDP_PORT);//转换成网络字节序


	addr.sin_addr.S_un.S_addr = INADDR_ANY;//绑定任意网卡
	error = bind(m_sock, (sockaddr*)&addr, sizeof(addr));

	if (SOCKET_ERROR == error)
	{
		cout << "bind error:" << WSAGetLastError() << endl;
		
		return false;
	}
	else
	{
		cout << "bind success" << endl;
	}

	//申请有限广播权限
	BOOL value = true;
	setsockopt(m_sock, SOL_SOCKET, SO_BROADCAST, (char*)&value, sizeof(value));

	// 5、创建一个接收数据的线程
	//使用CreateThread创建线程，回收线程就会自动调用ExitThread，在ExitThread回收线程的时候,
	//不会回收C++运行时库 (strcpy) 申请的空间，就会造成内泄漏。
	// 使用_beginthreadex创建线程，回收线程就会自动调用_endthreadex，在这个函数里会先回收申请的空间，然后再调用ExitThread。
	m_handle = (HANDLE)_beginthreadex(NULL, 0, &RecvThread, this, 0, NULL);



	return true;
}

unsigned __stdcall UdpNet::RecvThread(void* lpvoid)
{
	UdpNet* pThis = (UdpNet*)lpvoid;
	pThis->RecvData();
	return 1;
}

// 关闭网络:回收内核对象(结束线程、关闭句柄)、关闭套接字、卸载库
void UdpNet::UnInitNet()
{
	//1、回收内核对象结束线程工作、关闭句柄)
	m_bStop = true;
	if (m_handle)
	{
		//等一会，让线程执行到while判断，自己退出while循环
		if (WAIT_TIMEOUT == WaitForSingleObject(m_handle, 500))
		{
			//如果超时，就强制结束线程
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
bool UdpNet::SendData(char* buf, int nLen, long lSend)
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_DEF_UDP_PORT);
	addr.sin_addr.S_un.S_addr = lSend;
	
	if (sendto(m_sock,buf,nLen,0,(sockaddr*)&addr,sizeof(addr))>0)
	{
		return true;
	}
	else
	{
		cout << "UdpNet::SendData:" << WSAGetLastError() << endl;
		return false;
	}
	
}

void UdpNet::RecvData()
{

	int nRecvNum = 0;
	char recvBuf[4096] = "";
	sockaddr_in addr;
	int addrSize = sizeof(addr);
	while (!m_bStop)
	{
		nRecvNum = recvfrom(m_sock, recvBuf, sizeof(recvBuf), 0, (sockaddr*)&addr, &addrSize);
		if (nRecvNum>0)
		{
			//根据接收到的数据大小new一块空间
			char* pachBuf = new char[nRecvNum];
			// 把接收到的数据拷贝到新的空间中
			memcpy(pachBuf, recvBuf, nRecvNum);
			// 把新申请空间的数据传给中介者类
			m_pMediator->DealData(pachBuf, nRecvNum, addr.sin_addr.S_un.S_addr);
		}
		else
		{
			cout << "udpnet::recvdata:" << WSAGetLastError() << endl;
			break;
		}
	}

}
