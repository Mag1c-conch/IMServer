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
// ��ʼ������:���ؿ⡢�����׽��֡���ip��ַ������㲥Ȩ�ޡ�����һ���������ݵ��߳�
bool UdpNet::InitNet()
{
	//1�����ؿ�
	WORD version = MAKEWORD(2, 2);//����һ��word��������
	WSAData data;

	int error = WSAStartup(version, &data);
	if (0 != error)
	{
		cout << "WSAStartup error" << endl;
		return 1;
	}

	//�жϼ��صĿ�汾�Ƿ���2.2�汾
	if (2 != LOBYTE(data.wVersion) || 2 != HIBYTE(data.wVersion))
	{
		cout << "version error " << endl;
		
		return false;

	}
	else
	{
		cout << "WSAStartup success" << endl;
	}

	//2�������׽���
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


	//3����ip��ַһһ���߲���ϵͳ������Щ�����Լ����͸��ĸ��˿ڵ������ǵ�ǰӦ�ó�����Ҫ������

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_DEF_UDP_PORT);//ת���������ֽ���


	addr.sin_addr.S_un.S_addr = INADDR_ANY;//����������
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

	//�������޹㲥Ȩ��
	BOOL value = true;
	setsockopt(m_sock, SOL_SOCKET, SO_BROADCAST, (char*)&value, sizeof(value));

	// 5������һ���������ݵ��߳�
	//ʹ��CreateThread�����̣߳������߳̾ͻ��Զ�����ExitThread����ExitThread�����̵߳�ʱ��,
	//�������C++����ʱ�� (strcpy) ����Ŀռ䣬�ͻ������й©��
	// ʹ��_beginthreadex�����̣߳������߳̾ͻ��Զ�����_endthreadex���������������Ȼ�������Ŀռ䣬Ȼ���ٵ���ExitThread��
	m_handle = (HANDLE)_beginthreadex(NULL, 0, &RecvThread, this, 0, NULL);



	return true;
}

unsigned __stdcall UdpNet::RecvThread(void* lpvoid)
{
	UdpNet* pThis = (UdpNet*)lpvoid;
	pThis->RecvData();
	return 1;
}

// �ر�����:�����ں˶���(�����̡߳��رվ��)���ر��׽��֡�ж�ؿ�
void UdpNet::UnInitNet()
{
	//1�������ں˶�������̹߳������رվ��)
	m_bStop = true;
	if (m_handle)
	{
		//��һ�ᣬ���߳�ִ�е�while�жϣ��Լ��˳�whileѭ��
		if (WAIT_TIMEOUT == WaitForSingleObject(m_handle, 500))
		{
			//�����ʱ����ǿ�ƽ����߳�
			TerminateThread(m_handle, -1);
		}

		CloseHandle(m_handle);
		m_handle = NULL;

	}
	//2���ر��׽���
	if (m_sock && INVALID_SOCKET != m_sock)
	{
		closesocket(m_sock);
	}
	//3��ж�ؿ�
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
			//���ݽ��յ������ݴ�Сnewһ��ռ�
			char* pachBuf = new char[nRecvNum];
			// �ѽ��յ������ݿ������µĿռ���
			memcpy(pachBuf, recvBuf, nRecvNum);
			// ��������ռ�����ݴ����н�����
			m_pMediator->DealData(pachBuf, nRecvNum, addr.sin_addr.S_un.S_addr);
		}
		else
		{
			cout << "udpnet::recvdata:" << WSAGetLastError() << endl;
			break;
		}
	}

}
