#pragma once

#define _DEF_UDP_PORT (78345)
#define _DEF_TCP_PORT (87656)
#define _DEF_TCP_SERVER_IP ("127.0.0.1")

#define _DEF_NAME_MAX (100)
#define _DEF_CONTENT_LENGTH (4096)

#define _DEF_PROTOCOL_COUNT (10)
//协议头
#define _PROTOCOL_BASE (1000)
// 注册

#define _PROTOCOL_REGISTER_RQ (_PROTOCOL_BASE +1)
#define _PROTOCOL_REGISTER_RS (_PROTOCOL_BASE +2)

// 登录
#define _PROTOCOL_LOGIN_RQ (_PROTOCOL_BASE +3)
#define _PROTOCOL_LOGIN_RS (_PROTOCOL_BASE +4)

//好友信息
#define _PROTOCOL_FRIEND_INFO (_PROTOCOL_BASE+5)
// 聊天
#define _PROTOCOL_CHAT_RQ (_PROTOCOL_BASE+6)
#define _PROTOCOL_CHAT_RS (_PROTOCOL_BASE+7)

// 添加好友
#define _PROTOCOL_ADD_FRIEND_RQ (_PROTOCOL_BASE+8)
#define _PROTOCOL_ADD_FRIEND_RS (_PROTOCOL_BASE+9)

// 下线
#define _PROTOCOL_OFFLINE_RQ (_PROTOCOL_BASE+10)
//返回结果
//注册结果

#define register_success (0)
#define tel_is_used (1)
#define name_is_exist (2)
//登录结果
#define login_success (0)
#define no_this_user (1)
#define password_error (2)
//用户状态
#define status_online (0)
#define status_offline (1)

//发送结果
#define send_success (0)
#define send_fail (1)

//添加好友结果
#define add_friend_success (0)
#define friend_refuse (1)
#define friend_not_exists (2)
#define friend_offline (3)

//重新命名协议头
typedef int PackType;

//请求结构体
//注册请求:协议头、昵称、手机号、密码
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


//注册回复:协议头、注册结果

typedef struct _STRU_REGISTER_RS {
	_STRU_REGISTER_RS():type(_PROTOCOL_REGISTER_RS),result(name_is_exist)
	{}
	PackType type;
	int result;
}_STRU_REGISTER_RS;
// 登录请求:协议头、电话号、密码

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

//登录回复:协议头、登录结果
typedef struct _STRU_LOGIN_RS {
	_STRU_LOGIN_RS():type(_PROTOCOL_LOGIN_RS),result(password_error),id(0)
	{}
	PackType type;
	int id;
	int result;
}_STRU_LOGIN_RS;
// 好友信息:协议头、用户id、昵称、签名、头像id、状态

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

// 聊天请求:协议头、聊天内容、A用户id、B用户的id
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

// 聊天回复: 协议头、发送结果 (如果对方不在线，发送失败)
typedef struct _STRU_CHAT_RS {
	_STRU_CHAT_RS():type(_PROTOCOL_CHAT_RS),result(send_fail)
	{}
	PackType type;
	int result;
	int friendId;
}_STRU_CHAT_RS;

// 添加好友请求:协议头、自己的id、好友昵称、自己的昵称
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

// 添加好友回复:协议头、添加结果、好友的呢称

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
//下线请求:协议头、自己的id
typedef struct _STRU_OFFLINE_RQ {
	_STRU_OFFLINE_RQ() :type(_PROTOCOL_OFFLINE_RQ),userId(0)
	{
	}
	PackType type;

	int userId;

}_STRU_OFFLINE_RQ;