#pragma once

#define _DEF_UDP_PORT (78345)
#define _DEF_TCP_PORT (87656)
#define _DEF_TCP_SERVER_IP ("127.0.0.1")

#define _DEF_NAME_MAX (100)
#define _DEF_CONTENT_LENGTH (4096)

#define _DEF_PROTOCOL_COUNT (10)
//Э��ͷ
#define _PROTOCOL_BASE (1000)
// ע��

#define _PROTOCOL_REGISTER_RQ (_PROTOCOL_BASE +1)
#define _PROTOCOL_REGISTER_RS (_PROTOCOL_BASE +2)

// ��¼
#define _PROTOCOL_LOGIN_RQ (_PROTOCOL_BASE +3)
#define _PROTOCOL_LOGIN_RS (_PROTOCOL_BASE +4)

//������Ϣ
#define _PROTOCOL_FRIEND_INFO (_PROTOCOL_BASE+5)
// ����
#define _PROTOCOL_CHAT_RQ (_PROTOCOL_BASE+6)
#define _PROTOCOL_CHAT_RS (_PROTOCOL_BASE+7)

// ��Ӻ���
#define _PROTOCOL_ADD_FRIEND_RQ (_PROTOCOL_BASE+8)
#define _PROTOCOL_ADD_FRIEND_RS (_PROTOCOL_BASE+9)

// ����
#define _PROTOCOL_OFFLINE_RQ (_PROTOCOL_BASE+10)
//���ؽ��
//ע����

#define register_success (0)
#define tel_is_used (1)
#define name_is_exist (2)
//��¼���
#define login_success (0)
#define no_this_user (1)
#define password_error (2)
//�û�״̬
#define status_online (0)
#define status_offline (1)

//���ͽ��
#define send_success (0)
#define send_fail (1)

//��Ӻ��ѽ��
#define add_friend_success (0)
#define friend_refuse (1)
#define friend_not_exists (2)
#define friend_offline (3)

//��������Э��ͷ
typedef int PackType;

//����ṹ��
//ע������:Э��ͷ���ǳơ��ֻ��š�����
typedef struct _STRU_REGISTER_RQ {
	_STRU_REGISTER_RQ():type(_PROTOCOL_REGISTER_RQ)
	{
		memset(name,0, _DEF_NAME_MAX);
		memset(tel, 0, _DEF_NAME_MAX);
		memset(password, 0, _DEF_NAME_MAX);
	}
	PackType type;
	char name[_DEF_NAME_MAX];
	char tel[_DEF_NAME_MAX];
	char password[_DEF_NAME_MAX];
}_STRU_REGISTER_RQ;


//ע��ظ�:Э��ͷ��ע����

typedef struct _STRU_REGISTER_RS {
	_STRU_REGISTER_RS():type(_PROTOCOL_REGISTER_RS),result(name_is_exist)
	{}
	PackType type;
	int result;
}_STRU_REGISTER_RS;
// ��¼����:Э��ͷ���绰�š�����

typedef struct _STRU_LOGIN_RQ {
	_STRU_LOGIN_RQ() :type(_PROTOCOL_LOGIN_RQ)
	{
		memset(tel, 0, _DEF_NAME_MAX);
		memset(password, 0, _DEF_NAME_MAX);

	}
	PackType type;
	char tel[_DEF_NAME_MAX];
	char password[_DEF_NAME_MAX];
}_STRU_LOGIN_RQ;

//��¼�ظ�:Э��ͷ����¼���
typedef struct _STRU_LOGIN_RS {
	_STRU_LOGIN_RS():type(_PROTOCOL_LOGIN_RS),result(password_error),id(0)
	{}
	PackType type;
	int id;
	int result;
}_STRU_LOGIN_RS;
// ������Ϣ:Э��ͷ���û�id���ǳơ�ǩ����ͷ��id��״̬

typedef struct _STRU_FRIEND_INFO {
	_STRU_FRIEND_INFO() :type(_PROTOCOL_FRIEND_INFO), inconId(-1), status(status_offline),friendId(0)
	{
		memset(name, 0, _DEF_NAME_MAX);
		memset(feeling, 0, _DEF_NAME_MAX);
	}
	PackType type;
	char name[_DEF_NAME_MAX];
	char feeling[_DEF_NAME_MAX];
	int inconId;
	int status;
	int friendId;
}_STRU_FRIEND_INFO;

// ��������:Э��ͷ���������ݡ�A�û�id��B�û���id
typedef struct _STRU_CHAT_RQ {
	_STRU_CHAT_RQ() :type(_PROTOCOL_CHAT_RQ), userId(-1), friendId(-1)
	{
		memset(content, 0, _DEF_CONTENT_LENGTH);
	}
	PackType type;
	char content[_DEF_CONTENT_LENGTH];
	int userId;
	int friendId;
}_STRU_CHAT_RQ;

// ����ظ�: Э��ͷ�����ͽ�� (����Է������ߣ�����ʧ��)
typedef struct _STRU_CHAT_RS {
	_STRU_CHAT_RS():type(_PROTOCOL_CHAT_RS),result(send_fail)
	{}
	PackType type;
	int result;
	int friendId;
}_STRU_CHAT_RS;

// ��Ӻ�������:Э��ͷ���Լ���id�������ǳơ��Լ����ǳ�
typedef struct _STRU_ADD_FRIEND_RQ {
	_STRU_ADD_FRIEND_RQ() :type(_PROTOCOL_ADD_FRIEND_RQ), userId(0)
	{
		memset(username, 0, _DEF_NAME_MAX);
		memset(friendName, 0, _DEF_NAME_MAX);

	}
	PackType type;
	int userId;
	char username[_DEF_NAME_MAX];
	char friendName[_DEF_NAME_MAX];

}_STRU_ADD_FRIEND_RQ;

// ��Ӻ��ѻظ�:Э��ͷ����ӽ�������ѵ��س�

typedef struct _STRU_ADD_FRIEND_RS {
	_STRU_ADD_FRIEND_RS() :type(_PROTOCOL_ADD_FRIEND_RS), result(friend_refuse),userId(0),friendId(0)
	{
		memset(friendName, 0, _DEF_NAME_MAX);
	}
	PackType type;
	char friendName[_DEF_NAME_MAX];
	int result;
	int userId;
	int friendId;

}_STRU_ADD_FRIEND_RS;
//��������:Э��ͷ���Լ���id
typedef struct _STRU_OFFLINE_RQ {
	_STRU_OFFLINE_RQ() :type(_PROTOCOL_OFFLINE_RQ),userId(0)
	{
	}
	PackType type;

	int userId;

}_STRU_OFFLINE_RQ;