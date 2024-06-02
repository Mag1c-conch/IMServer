#define _CRT_SECURE_NO_WARNINGS

#include "CKernel.h"
#include"TcpServerMediator.h"
//静态全局变量类外初始化
CKernel* CKernel::pKernel = NULL;
CKernel::CKernel()
{
	cout << __func__ << endl;
	m_pMediator = new TcpServerMediator;
	pKernel = this;
	setProtocolMap();
}

CKernel::~CKernel()
{
	cout << __func__ << endl;
	//回收资源
	if (m_pMediator)
	{
		m_pMediator->CloseNet();
		delete m_pMediator;
		m_pMediator = NULL;
	}
}

bool CKernel::startServer()
{
	cout << __func__ << endl;
	//1、打开网络
	if (!m_pMediator->OpenNet())
	{
		cout << "打开网络失败" << endl;
		return false;

	}

	// 2、连接数据库
	if (!sql.ConnectMySql("127.0.0.1", "root", "020624", "20221022im",45645))
	{
		cout << "连接数据库失败" << endl;
		return false;
	}
	return true;
}

void CKernel::setProtocolMap()
{
	cout << __func__ << endl;
	//给数组初始化
	memset(m_mapProtocol, 0, sizeof(m_mapProtocol));

	//绑定协议头和处理函数
	m_mapProtocol[_PROTOCOL_REGISTER_RQ - _PROTOCOL_BASE - 1] = &CKernel::dealRegisterRq;
	m_mapProtocol[_PROTOCOL_LOGIN_RQ - _PROTOCOL_BASE - 1] = &CKernel::dealLoginRq;
	m_mapProtocol[_PROTOCOL_CHAT_RQ - _PROTOCOL_BASE - 1] = &CKernel::dealChatRq;
	m_mapProtocol[_PROTOCOL_OFFLINE_RQ - _PROTOCOL_BASE - 1] = &CKernel::dealOfflineRq;
	m_mapProtocol[_PROTOCOL_ADD_FRIEND_RQ - _PROTOCOL_BASE - 1] = &CKernel::dealAddFriendRq;
	m_mapProtocol[_PROTOCOL_ADD_FRIEND_RS - _PROTOCOL_BASE - 1] = &CKernel::dealAddFriendRs;
}

void CKernel::readyData(char* buf, int nLen, long lFrom)
{
	cout << __func__ << endl;

	////测试代码:给客户端发送CKernel::readyData
	//m_pMediator->SendData("CKernel::readyData", sizeof("CKernel::readyData"), lFrom);

	// 1、取出协议头
	PackType type = *(PackType*)buf;

	//2、根据协议头判断是哪个结构体，走哪个处理函数
	if (type > _PROTOCOL_BASE && type <= _PROTOCOL_BASE + _DEF_PROTOCOL_COUNT)
	{
		//计算数组下标
		int index = type - _PROTOCOL_BASE - 1;
		// 取出函数指针
		PFUN pf = m_mapProtocol[index];
		//调用函数
		if (pf)
		{
			(this->*pf)(buf, nLen, lFrom);
		}
		else
		{
			cout << "函数指针为空，type" <<type<< endl;
		}
	}
	else
	{
		cout << "type" << type<<endl;
	}

}

void CKernel::dealRegisterRq(char* buf, int nLen, long lFrom)
{
	cout << __func__ << endl;
	// 1、拆包
	_STRU_REGISTER_RQ* rq = (_STRU_REGISTER_RQ*)buf;

	// 2、校验用户输入的参数是否正确(数据是从网络传过来的，可能会有仿冒客户端的请求)

	// 3、根据手机号查询数据库
	list<string>listRes;
	char sqlBuf[1024] = "";
	sprintf(sqlBuf, "select tel from t_user where tel='%s';", rq->tel);
	if (!sql.SelectMySql(sqlBuf, 1, listRes)) {
		cout << "查询数据库失败，sglBuf:" << sqlBuf << endl;
		return;
	}


	_STRU_REGISTER_RS rs;
	// 4、判断查询结果大于0，说明手机号已经被注册过了

	if (listRes.size()>0) {
		// 5、注册失败，失败原因tel_is_used
		rs.result = tel_is_used;

	}
	else
	{
		// 6、如果查询结果为0，说明手机号没有被注册，就根据昵称查询数据库
		sprintf(sqlBuf, "select name from t_user where name='%s';", rq->name);
			if (!sql.SelectMySql(sqlBuf, 1, listRes)) {
				cout << "查询数据库失败，sglBuf:" << sqlBuf << endl;
				return;
			}
		// 7、判断查询结果大于0，说明昵称被注册过
		if (listRes.size() > 0)
		{
			// 8、注册失败，失败原因name_is_exist
			rs.result = name_is_exist;
		}
		else
		{
			// 9、如果查询结果为0，说明昵称没有被使用过，就写入当前用户的信息到数据库
			sprintf(sqlBuf, "insert into t_user(name,tel,password,iconid,feeling)values('%s','%s','%s',1,'哈哈和');", rq->name,rq->tel,rq->password);
			if (!sql.UpdateMySql(sqlBuf)) {
				cout << "更新数据库失败，sglBuf:" << sqlBuf << endl;
				return;
			}
			// 10、注册成功register success
			rs.result = register_success;
		}
	}
	// 给客户端回顾注册回复
	m_pMediator->SendData((char*)&rs, sizeof(rs), lFrom);
}

void CKernel::dealLoginRq(char* buf, int nLen, long lFrom)
{
	cout << __func__ << endl;
	// 1、拆包
	_STRU_LOGIN_RQ* rq = (_STRU_LOGIN_RQ*)buf;

	// 2、校验用户输入的数据是否合法

	//3、根据tel查询密码
	list<string>listRes;
	char sqlBuf[1024] = "";
	sprintf(sqlBuf, "select id,password from t_user where tel='%s';", rq->tel);
	if (!sql.SelectMySql(sqlBuf, 2, listRes)) {
		cout << "查询数据库失败，sglBuf:" << sqlBuf << endl;
		return;
	}
	_STRU_LOGIN_RS rs;
	//4、判断查询结构是否为0，如果是0说明用户不存在
	if (0==listRes.size()) {
		//登录失败，用户不存在no_this_user
		rs.result = no_this_user;

	}else{
		//取id
		int userId = atoi(listRes.front().c_str());
		listRes.pop_front();
		//取出密码
		string password = listRes.front();
		listRes.pop_front();


		//5、如果用户存在，比较密码是否一致
		if (0 == strcmp(password.c_str(),rq->password))
		{
			// 6、如果密码一致，登录成功login success
			rs.result = login_success;
			rs.id = userId;
			//8、给客户端回复登录结果
			m_pMediator->SendData((char*)&rs, sizeof(rs), lFrom);
			//保存当前登录成功用户的id和对应的socket
			m_mapIdToSocket[userId] = lFrom;

			// 给客户端发送自己以及好友的信息
			getUserAndFriendInfo(userId);

			return;
		}
		else {
			//7、如果密码不一致，登录失败，密码错误password_error
			rs.result = password_error;
		}
	}

	//8、给客户端回复登录结果
	m_pMediator->SendData((char*)&rs, sizeof(rs), lFrom);
}

void CKernel::getUserAndFriendInfo(int userId)
{
	cout <<__func__<< endl;
	// 1、根据用户id查询自己的信息
	_STRU_FRIEND_INFO userInfo;
	getUserInfoByUserId(userId, &userInfo);

	// 2、把自己的信息发给客户端
	m_pMediator->SendData((char*)&userInfo, sizeof(userInfo), m_mapIdToSocket[userId]);

	// 3、根据用户id查询好友的id列表
	list<string>listRes;
	char sqlBuf[1024] = "";
	sprintf(sqlBuf, "select idB from t_friend where idA=%d;", userId);
	if (!sql.SelectMySql(sqlBuf, 1, listRes)) {
		cout << "查询数据库失败，sglBuf:" << sqlBuf << endl;
		return;
	}

	// 4、遍历好友的id列表

	int friendId = 0;
	_STRU_FRIEND_INFO friendInfo;
	while (listRes.size()>0)
	{
		//5、取出好友的id
		friendId = atoi(listRes.front().c_str());
		listRes.pop_front();

		// 6、根据好友的id查询好友的信息
		getUserInfoByUserId(friendId, &friendInfo);
		// 7、把好友的信息发给客户端
		m_pMediator->SendData((char*)&friendInfo, sizeof(friendInfo), m_mapIdToSocket[userId]);

		// 8、判断好友是否在线
		if (m_mapIdToSocket.count(friendId) > 0)
		{
			//9、如果好友在线，把当前登录用的信息发给好友
			m_pMediator->SendData((char*)&userInfo, sizeof(userInfo), m_mapIdToSocket[friendId]);
		}

	}

}

void CKernel::getUserInfoByUserId(int id,_STRU_FRIEND_INFO*info)
{
	cout << __func__ << endl;
	info->friendId = id;
	if (m_mapIdToSocket.count(id) > 0)
	{
		// 在线
		info->status = status_online;
	}
	else
	{
		info->status = status_offline;
	}
	// 从数据库查询name、feeling、iconid
	list<string>listRes;
	char sqlBuf[1024] = "";
	sprintf(sqlBuf, "select name,feeling,iconid from t_user where id=%d;", id);
	if (!sql.SelectMySql(sqlBuf, 3, listRes)) {
		cout << "查询数据库失败，sqlBuf:" << sqlBuf << endl;
		return;
	}

	//给结构体赋值
	if (3 == listRes.size())
	{
		strcpy(info->name, listRes.front().c_str());
		listRes.pop_front();

		strcpy(info->feeling, listRes.front().c_str());
		listRes.pop_front();

		info->inconId = atoi(listRes.front().c_str());
		listRes.pop_front();
	}
}

void CKernel::dealChatRq(char* buf, int nLen, long lFrom)
{
	cout << __func__ << endl;
	//1、拆包
	_STRU_CHAT_RQ* rq = (_STRU_CHAT_RQ*)buf;

	//2、找到跟A客户端的聊天窗口，显示聊天窗口，设置聊天内容到窗口上
	if (m_mapIdToSocket.count(rq->friendId) > 0)
	{
		//如果在线，直接转发给聊天对象
		m_pMediator->SendData(buf, nLen, m_mapIdToSocket[rq->friendId]);

	}
	else
	{
		//如果不在线，就回复客户端，发送失败
		_STRU_CHAT_RS rs;
		rs.result = send_fail;
		rs.friendId = rq->friendId;
		m_pMediator->SendData((char*)&rs, sizeof(rs), lFrom);
	}
}

void CKernel::dealOfflineRq(char* buf, int nLen, long lFrom)
{
	cout << __func__ << endl;
	//1、拆包
	_STRU_OFFLINE_RQ* rq = (_STRU_OFFLINE_RQ*)buf;

	//2、根据用户id查询好友id列表
	list<string>listRes;
	char sqlBuf[1024]="";
	sprintf(sqlBuf, "select idB from t_friend where idA=%d;", rq->userId);
	if (!sql.SelectMySql(sqlBuf, 1, listRes)) {
		cout << "查询数据库失败，sqlBuf:" << sqlBuf << endl;
		return;
	}
	// 3、遍历好友id列表
	int friendId = 0;
	while (listRes.size() > 0)
	{
		//4、取出好友的id
		friendId = atoi(listRes.front().c_str());
		listRes.pop_front();

		//5、给好友发送下线请求
		if (m_mapIdToSocket.count(friendId) > 0)
		{
			m_pMediator->SendData(buf, nLen, m_mapIdToSocket[friendId]);
		}
	}
	//6、把自己从map中移除
	if (m_mapIdToSocket.count(rq->userId) > 0)

	{
		closesocket(m_mapIdToSocket[rq->userId]);
		m_mapIdToSocket.erase(rq->userId);
	}
}

//处理添加好友请求
void CKernel::dealAddFriendRq(char* buf, int nLen, long lFrom)
{
	cout << __func__ << endl;
	//1、拆包
	_STRU_ADD_FRIEND_RQ* rq = (_STRU_ADD_FRIEND_RQ*)buf;

	//2、根据好友的昵称查询好友的id
	list<string>listRes;
	char sqlBuf[1024] = "";
	sprintf(sqlBuf, "select idB from t_friend where name='%s';", rq->userId);
	if (!sql.SelectMySql(sqlBuf, 1, listRes)) {
		cout << "查询数据库失败，sqlBuf:" << sqlBuf << endl;
		return;
	}


	//3、判断查询结果是否在0
	_STRU_ADD_FRIEND_RS rs;
	if (0==listRes.size())
	{
		// 4、如果查询结果为0，说明添加好友失败，friend not exists
		rs.result = friend_not_exists;
		strcpy(rs.friendName, rq->friendName);
		m_pMediator->SendData((char*)&rs, sizeof(rs), lFrom);
	}
	else
	{
		int friendId = atoi(listRes.front().c_str());
		listRes.pop_front();


		//5 如果好友存在，判断好友是否在线
		if (m_mapIdToSocket.count(friendId)> 0)
		{
			//6、如果好友在线，转发添加好友请求
			m_pMediator->SendData(buf, nLen, m_mapIdToSocket[friendId]);
		}
		else
		{

			// 7、如果好友不在线，添加好友失败，friend_offline
			rs.result = friend_offline;
			strcpy(rs.friendName, rq->friendName);
			m_pMediator->SendData((char*)&rs, sizeof(rs), lFrom);
		}
	}
}

//处理添加好友回复
void CKernel::dealAddFriendRs(char* buf, int nLen, long lFrom)
{
	cout << __func__ << endl;
	//1、拆包
	_STRU_ADD_FRIEND_RS* rs = (_STRU_ADD_FRIEND_RS*)buf;

	//2、判断添加好友是否同意
	if (add_friend_success == rs->result)
	{
		//3、如果同意把好友关系写进数据库
		char sqlBuf[1024] = "";
		sprintf(sqlBuf, "inset into t_friend values(%d,%d)", rs->friendId, rs->userId);
		if (!sql.UpdateMySql(sqlBuf))
		{
			cout << "插入数据库失败，sqlBuf:" << sqlBuf << endl;
			return;
		}
		//更新好友列表
		getUserAndFriendInfo(rs->friendId);
	}

	//不管添加好友是否成功，都需要给客户端A回复添加结果
	m_pMediator->SendData(buf, nLen, m_mapIdToSocket[rs->userId]);
}