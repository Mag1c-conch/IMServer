#pragma once
#include<iostream>
#include"INetMediator.h"
#include"CMysql.h"
#include"PackDef.h"
#include<map>

using namespace std;

//���庯��ָ��
class CKernel;
typedef void (CKernel::* PFUN)(char* buf, int nLen, long lFrom);
class CKernel
{
public:
	CKernel();
	~CKernel();
	bool startServer();
	// ��ʼ������ָ��
	void setProtocolMap();
	void readyData(char* buf, int nLen, long lFrom);

	// ��ȡ�Լ��Լ����ѵ���Ϣ
	void getUserAndFriendInfo(int userId);

	// �����û�id��ѯ�û���Ϣ
	void getUserInfoByUserId(int id, _STRU_FRIEND_INFO* info);
	//����ע������
	void dealRegisterRq(char* buf, int nLen, long lFrom);
	//�����¼����
	void dealLoginRq(char* buf, int nLen, long lFrom);
	//������������
	void dealChatRq(char* buf, int nLen, long lFrom);

	void CKernel::dealOfflineRq(char* buf, int nLen, long lFrom);

	void CKernel::dealAddFriendRq(char* buf, int nLen, long lFrom);
	//������Ӻ��ѻظ�
	void CKernel::dealAddFriendRs(char* buf, int nLen, long lFrom);
private:
	INetMediator* m_pMediator;
	CMySql sql;
public:
	static CKernel* pKernel;
	//���庯��ָ������
	PFUN m_mapProtocol[_DEF_PROTOCOL_COUNT];

	// ���浱ǰ��¼�û���socket
	map<int, SOCKET>m_mapIdToSocket;
};

