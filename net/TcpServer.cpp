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
// TCP�ͻ��˳�ʼ������: 1�����ؿ� 2�������׽��� 3������ 4�������������ݵ��߳�
bool TcpServerNet::InitNet()
{
	// 1�����ؿ�
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
	// 2�������׽���
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
	// 3����ip��ַ
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
	// 4������
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
	//5�������������ӵ��߳�
	HANDLE handle = (HANDLE)_beginthreadex(NULL, 0 ,&AcceptThread, this,0,NULL);
	if (handle)
	{
		m_listHandle.push_back(handle);
	}
	return true;
}

void TcpServerNet::UnInitNet()
{
	// 1�������ں˿ռ�(�����̺߳͹رվ��)
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
		//�ѵ�ǰ�ڵ��list����ɾ��
		ite = m_listHandle.erase(ite);
	}

	//2���ر��׽���
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
		// �ѵ�ǰ�ڵ��map����ɾ��
		ite = m_mapThreadIdToSocket.erase(ite);
	}
	// 3��ж�ؿ�
	WSACleanup();
}

bool TcpServerNet::SendData(char* buf, int nLen, long lSend)
{
	//1���жϲ����Ƿ�Ϸ�
	if (!buf || nLen <= 0)
	{
		cout << "TcpServerNet::SendaData ��������" << endl;
		return false;

	}
	//2���ȷ�����С
	if (send(lSend, (char*)&nLen, sizeof(int), 0) <= 0)
	{
		cout << "TcpServerNet::SendaData send error:" << WSAGetLastError() << endl;
		return false;
	}

	//3���ٷ�������
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
		// ��������
		sock = accept(pThis->m_sock, (sockaddr*)&clientAddr, &clientAddrSize);
		if (INVALID_SOCKET == sock)
		{
			// ������ӡ��־
			cout << "TcpServerNet::AcceptThread error:" << WSAGetLastError() << endl;
			break;
		}
		else
		{
			//һ���ͻ������ӳɹ�
			cout << "client ip:" << inet_ntoa(clientAddr.sin_addr) << endl;
			//����һ����������ͻ������ݵ��߳�
			handle = (HANDLE)_beginthreadex(NULL, 0 ,& RecvThread, pThis, 0, &threadId);
			if (handle)
			{
				pThis->m_listHandle.push_back(handle);
			}
			// �����߳�id�Ͷ�Ӧ��socket
			pThis->m_mapThreadIdToSocket[threadId] = sock;
		}
	}
	return 1;
}

void TcpServerNet::RecvData()
{
	//������һ�ᣬ��ֹsocket��û�д���map�о���ȡ������
	Sleep(100);

	//��map��ȡ��socket
	SOCKET sock = m_mapThreadIdToSocket[GetCurrentThreadId()];

	//�ж�socket�ĺϷ���
	if (!sock || INVALID_SOCKET == sock)
	{
		cout << "TcpServerNet::RecvData socket error" << endl;
		return;
	}
	int nPackSize = 0;
	int nRecvNum = 0;
	//��¼��ǰ�ۻ����յ���������
	int nOffset = 0;
	while (!m_bStop)
	{
		//1�����հ���С

		nRecvNum = recv(sock, (char*)&nPackSize, sizeof(int), 0);
		if (nRecvNum > 0)
		{
			// 2�����հ���Сnew�������ݵĿռ�
			char* buf = new char[nPackSize];
			// 3�����հ�����
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
			//4���������ݳɹ��������ݷ����н�����
			m_pMediator->DealData(buf, nOffset, sock);
		}
		else
		{
			cout << "TcpServerNet::RecvData recv error :" << WSAGetLastError() << endl;
			break;
		}

	}
}
