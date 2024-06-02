#pragma once
#include<iostream>
#include"INetMediator.h"
#include"CMysql.h"
#include"PackDef.h"
#include<map>

using namespace std;

//定义函数指针
class CKernel;
typedef void (CKernel::* PFUN)(char* buf, int nLen, long lFrom);
class CKernel
{
public:
	CKernel();
	~CKernel();
	bool startServer();
	// 初始化函数指针
	void setProtocolMap();
	void readyData(char* buf, int nLen, long lFrom);

	// 获取自己以及好友的信息
	void getUserAndFriendInfo(int userId);

	// 根据用户id查询用户信息
	void getUserInfoByUserId(int id, _STRU_FRIEND_INFO* info);
	//处理注册请求
	void dealRegisterRq(char* buf, int nLen, long lFrom);
	//处理登录请求
	void dealLoginRq(char* buf, int nLen, long lFrom);
	//处理聊天请求
	void dealChatRq(char* buf, int nLen, long lFrom);

	void CKernel::dealOfflineRq(char* buf, int nLen, long lFrom);

	void CKernel::dealAddFriendRq(char* buf, int nLen, long lFrom);
	//处理添加好友回复
	void CKernel::dealAddFriendRs(char* buf, int nLen, long lFrom);
private:
	INetMediator* m_pMediator;
	CMySql sql;
public:
	static CKernel* pKernel;
	//定义函数指针数组
	PFUN m_mapProtocol[_DEF_PROTOCOL_COUNT];

	// 保存当前登录用户的socket
	map<int, SOCKET>m_mapIdToSocket;
};

