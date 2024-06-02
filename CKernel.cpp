#define _CRT_SECURE_NO_WARNINGS

#include "CKernel.h"
#include"TcpServerMediator.h"
//��̬ȫ�ֱ��������ʼ��
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
	//������Դ
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
	//1��������
	if (!m_pMediator->OpenNet())
	{
		cout << "������ʧ��" << endl;
		return false;

	}

	// 2���������ݿ�
	if (!sql.ConnectMySql("127.0.0.1", "root", "020624", "20221022im",45645))
	{
		cout << "�������ݿ�ʧ��" << endl;
		return false;
	}
	return true;
}

void CKernel::setProtocolMap()
{
	cout << __func__ << endl;
	//�������ʼ��
	memset(m_mapProtocol, 0, sizeof(m_mapProtocol));

	//��Э��ͷ�ʹ�����
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

	////���Դ���:���ͻ��˷���CKernel::readyData
	//m_pMediator->SendData("CKernel::readyData", sizeof("CKernel::readyData"), lFrom);

	// 1��ȡ��Э��ͷ
	PackType type = *(PackType*)buf;

	//2������Э��ͷ�ж����ĸ��ṹ�壬���ĸ�������
	if (type > _PROTOCOL_BASE && type <= _PROTOCOL_BASE + _DEF_PROTOCOL_COUNT)
	{
		//���������±�
		int index = type - _PROTOCOL_BASE - 1;
		// ȡ������ָ��
		PFUN pf = m_mapProtocol[index];
		//���ú���
		if (pf)
		{
			(this->*pf)(buf, nLen, lFrom);
		}
		else
		{
			cout << "����ָ��Ϊ�գ�type" <<type<< endl;
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
	// 1�����
	_STRU_REGISTER_RQ* rq = (_STRU_REGISTER_RQ*)buf;

	// 2��У���û�����Ĳ����Ƿ���ȷ(�����Ǵ����紫�����ģ����ܻ��з�ð�ͻ��˵�����)

	// 3�������ֻ��Ų�ѯ���ݿ�
	list<string>listRes;
	char sqlBuf[1024] = "";
	sprintf(sqlBuf, "select tel from t_user where tel='%s';", rq->tel);
	if (!sql.SelectMySql(sqlBuf, 1, listRes)) {
		cout << "��ѯ���ݿ�ʧ�ܣ�sglBuf:" << sqlBuf << endl;
		return;
	}


	_STRU_REGISTER_RS rs;
	// 4���жϲ�ѯ�������0��˵���ֻ����Ѿ���ע�����

	if (listRes.size()>0) {
		// 5��ע��ʧ�ܣ�ʧ��ԭ��tel_is_used
		rs.result = tel_is_used;

	}
	else
	{
		// 6�������ѯ���Ϊ0��˵���ֻ���û�б�ע�ᣬ�͸����ǳƲ�ѯ���ݿ�
		sprintf(sqlBuf, "select name from t_user where name='%s';", rq->name);
			if (!sql.SelectMySql(sqlBuf, 1, listRes)) {
				cout << "��ѯ���ݿ�ʧ�ܣ�sglBuf:" << sqlBuf << endl;
				return;
			}
		// 7���жϲ�ѯ�������0��˵���ǳƱ�ע���
		if (listRes.size() > 0)
		{
			// 8��ע��ʧ�ܣ�ʧ��ԭ��name_is_exist
			rs.result = name_is_exist;
		}
		else
		{
			// 9�������ѯ���Ϊ0��˵���ǳ�û�б�ʹ�ù�����д�뵱ǰ�û�����Ϣ�����ݿ�
			sprintf(sqlBuf, "insert into t_user(name,tel,password,iconid,feeling)values('%s','%s','%s',1,'������');", rq->name,rq->tel,rq->password);
			if (!sql.UpdateMySql(sqlBuf)) {
				cout << "�������ݿ�ʧ�ܣ�sglBuf:" << sqlBuf << endl;
				return;
			}
			// 10��ע��ɹ�register success
			rs.result = register_success;
		}
	}
	// ���ͻ��˻ع�ע��ظ�
	m_pMediator->SendData((char*)&rs, sizeof(rs), lFrom);
}

void CKernel::dealLoginRq(char* buf, int nLen, long lFrom)
{
	cout << __func__ << endl;
	// 1�����
	_STRU_LOGIN_RQ* rq = (_STRU_LOGIN_RQ*)buf;

	// 2��У���û�����������Ƿ�Ϸ�

	//3������tel��ѯ����
	list<string>listRes;
	char sqlBuf[1024] = "";
	sprintf(sqlBuf, "select id,password from t_user where tel='%s';", rq->tel);
	if (!sql.SelectMySql(sqlBuf, 2, listRes)) {
		cout << "��ѯ���ݿ�ʧ�ܣ�sglBuf:" << sqlBuf << endl;
		return;
	}
	_STRU_LOGIN_RS rs;
	//4���жϲ�ѯ�ṹ�Ƿ�Ϊ0�������0˵���û�������
	if (0==listRes.size()) {
		//��¼ʧ�ܣ��û�������no_this_user
		rs.result = no_this_user;

	}else{
		//ȡid
		int userId = atoi(listRes.front().c_str());
		listRes.pop_front();
		//ȡ������
		string password = listRes.front();
		listRes.pop_front();


		//5������û����ڣ��Ƚ������Ƿ�һ��
		if (0 == strcmp(password.c_str(),rq->password))
		{
			// 6���������һ�£���¼�ɹ�login success
			rs.result = login_success;
			rs.id = userId;
			//8�����ͻ��˻ظ���¼���
			m_pMediator->SendData((char*)&rs, sizeof(rs), lFrom);
			//���浱ǰ��¼�ɹ��û���id�Ͷ�Ӧ��socket
			m_mapIdToSocket[userId] = lFrom;

			// ���ͻ��˷����Լ��Լ����ѵ���Ϣ
			getUserAndFriendInfo(userId);

			return;
		}
		else {
			//7��������벻һ�£���¼ʧ�ܣ��������password_error
			rs.result = password_error;
		}
	}

	//8�����ͻ��˻ظ���¼���
	m_pMediator->SendData((char*)&rs, sizeof(rs), lFrom);
}

void CKernel::getUserAndFriendInfo(int userId)
{
	cout <<__func__<< endl;
	// 1�������û�id��ѯ�Լ�����Ϣ
	_STRU_FRIEND_INFO userInfo;
	getUserInfoByUserId(userId, &userInfo);

	// 2�����Լ�����Ϣ�����ͻ���
	m_pMediator->SendData((char*)&userInfo, sizeof(userInfo), m_mapIdToSocket[userId]);

	// 3�������û�id��ѯ���ѵ�id�б�
	list<string>listRes;
	char sqlBuf[1024] = "";
	sprintf(sqlBuf, "select idB from t_friend where idA=%d;", userId);
	if (!sql.SelectMySql(sqlBuf, 1, listRes)) {
		cout << "��ѯ���ݿ�ʧ�ܣ�sglBuf:" << sqlBuf << endl;
		return;
	}

	// 4���������ѵ�id�б�

	int friendId = 0;
	_STRU_FRIEND_INFO friendInfo;
	while (listRes.size()>0)
	{
		//5��ȡ�����ѵ�id
		friendId = atoi(listRes.front().c_str());
		listRes.pop_front();

		// 6�����ݺ��ѵ�id��ѯ���ѵ���Ϣ
		getUserInfoByUserId(friendId, &friendInfo);
		// 7���Ѻ��ѵ���Ϣ�����ͻ���
		m_pMediator->SendData((char*)&friendInfo, sizeof(friendInfo), m_mapIdToSocket[userId]);

		// 8���жϺ����Ƿ�����
		if (m_mapIdToSocket.count(friendId) > 0)
		{
			//9������������ߣ��ѵ�ǰ��¼�õ���Ϣ��������
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
		// ����
		info->status = status_online;
	}
	else
	{
		info->status = status_offline;
	}
	// �����ݿ��ѯname��feeling��iconid
	list<string>listRes;
	char sqlBuf[1024] = "";
	sprintf(sqlBuf, "select name,feeling,iconid from t_user where id=%d;", id);
	if (!sql.SelectMySql(sqlBuf, 3, listRes)) {
		cout << "��ѯ���ݿ�ʧ�ܣ�sqlBuf:" << sqlBuf << endl;
		return;
	}

	//���ṹ�帳ֵ
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
	//1�����
	_STRU_CHAT_RQ* rq = (_STRU_CHAT_RQ*)buf;

	//2���ҵ���A�ͻ��˵����촰�ڣ���ʾ���촰�ڣ������������ݵ�������
	if (m_mapIdToSocket.count(rq->friendId) > 0)
	{
		//������ߣ�ֱ��ת�����������
		m_pMediator->SendData(buf, nLen, m_mapIdToSocket[rq->friendId]);

	}
	else
	{
		//��������ߣ��ͻظ��ͻ��ˣ�����ʧ��
		_STRU_CHAT_RS rs;
		rs.result = send_fail;
		rs.friendId = rq->friendId;
		m_pMediator->SendData((char*)&rs, sizeof(rs), lFrom);
	}
}

void CKernel::dealOfflineRq(char* buf, int nLen, long lFrom)
{
	cout << __func__ << endl;
	//1�����
	_STRU_OFFLINE_RQ* rq = (_STRU_OFFLINE_RQ*)buf;

	//2�������û�id��ѯ����id�б�
	list<string>listRes;
	char sqlBuf[1024]="";
	sprintf(sqlBuf, "select idB from t_friend where idA=%d;", rq->userId);
	if (!sql.SelectMySql(sqlBuf, 1, listRes)) {
		cout << "��ѯ���ݿ�ʧ�ܣ�sqlBuf:" << sqlBuf << endl;
		return;
	}
	// 3����������id�б�
	int friendId = 0;
	while (listRes.size() > 0)
	{
		//4��ȡ�����ѵ�id
		friendId = atoi(listRes.front().c_str());
		listRes.pop_front();

		//5�������ѷ�����������
		if (m_mapIdToSocket.count(friendId) > 0)
		{
			m_pMediator->SendData(buf, nLen, m_mapIdToSocket[friendId]);
		}
	}
	//6�����Լ���map���Ƴ�
	if (m_mapIdToSocket.count(rq->userId) > 0)

	{
		closesocket(m_mapIdToSocket[rq->userId]);
		m_mapIdToSocket.erase(rq->userId);
	}
}

//������Ӻ�������
void CKernel::dealAddFriendRq(char* buf, int nLen, long lFrom)
{
	cout << __func__ << endl;
	//1�����
	_STRU_ADD_FRIEND_RQ* rq = (_STRU_ADD_FRIEND_RQ*)buf;

	//2�����ݺ��ѵ��ǳƲ�ѯ���ѵ�id
	list<string>listRes;
	char sqlBuf[1024] = "";
	sprintf(sqlBuf, "select idB from t_friend where name='%s';", rq->userId);
	if (!sql.SelectMySql(sqlBuf, 1, listRes)) {
		cout << "��ѯ���ݿ�ʧ�ܣ�sqlBuf:" << sqlBuf << endl;
		return;
	}


	//3���жϲ�ѯ����Ƿ���0
	_STRU_ADD_FRIEND_RS rs;
	if (0==listRes.size())
	{
		// 4�������ѯ���Ϊ0��˵����Ӻ���ʧ�ܣ�friend not exists
		rs.result = friend_not_exists;
		strcpy(rs.friendName, rq->friendName);
		m_pMediator->SendData((char*)&rs, sizeof(rs), lFrom);
	}
	else
	{
		int friendId = atoi(listRes.front().c_str());
		listRes.pop_front();


		//5 ������Ѵ��ڣ��жϺ����Ƿ�����
		if (m_mapIdToSocket.count(friendId)> 0)
		{
			//6������������ߣ�ת����Ӻ�������
			m_pMediator->SendData(buf, nLen, m_mapIdToSocket[friendId]);
		}
		else
		{

			// 7��������Ѳ����ߣ���Ӻ���ʧ�ܣ�friend_offline
			rs.result = friend_offline;
			strcpy(rs.friendName, rq->friendName);
			m_pMediator->SendData((char*)&rs, sizeof(rs), lFrom);
		}
	}
}

//������Ӻ��ѻظ�
void CKernel::dealAddFriendRs(char* buf, int nLen, long lFrom)
{
	cout << __func__ << endl;
	//1�����
	_STRU_ADD_FRIEND_RS* rs = (_STRU_ADD_FRIEND_RS*)buf;

	//2���ж���Ӻ����Ƿ�ͬ��
	if (add_friend_success == rs->result)
	{
		//3�����ͬ��Ѻ��ѹ�ϵд�����ݿ�
		char sqlBuf[1024] = "";
		sprintf(sqlBuf, "inset into t_friend values(%d,%d)", rs->friendId, rs->userId);
		if (!sql.UpdateMySql(sqlBuf))
		{
			cout << "�������ݿ�ʧ�ܣ�sqlBuf:" << sqlBuf << endl;
			return;
		}
		//���º����б�
		getUserAndFriendInfo(rs->friendId);
	}

	//������Ӻ����Ƿ�ɹ�������Ҫ���ͻ���A�ظ���ӽ��
	m_pMediator->SendData(buf, nLen, m_mapIdToSocket[rs->userId]);
}