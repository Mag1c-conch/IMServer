
//#include"CMySql.h"

#include<Windows.h>
#include"CKernel.h"

struct student {
	student()
	{
		memset(sex, 0, sizeof(sex));
	}
	int id;
	string name;
	int age;
	char sex[3];
};
int main()
{
	CKernel kernel;
	if (kernel.startServer())
	{
		while (true)
		{
			Sleep(5000);
			cout << "server is running" << endl;
		}
	}
	else
	{
		cout << "start server fail" << endl;
	}
	//// 创建个服务端
	//INetMediator* m_pServer = new TcpServerMediator;
	//if (!m_pServer->OpenNet())
	//{
	//	cout << "打开服务器网络失败" << endl;
	//	return 1;
	//}
	
	////创建个客户端
	// INetMediator* m_pClient = new TcpClientMediator;
	//if (!m_pClient->OpenNet())
	//{
	//	cout << "打开客户端网络失败" << endl;
	//	return 1;
	//}

	//// 客户端给服务器发送数据
	//
	//m_pClient->SendData("hello world", sizeof("hello world"), 900);

	/*while (true)
	{
		Sleep(5000);
		cout << "server is running" << endl;
	}*/

	// 
	// 
	//CMySql sql;
	//// 连接数据库
	//if (!sql.ConnectMySql("127.0.0.1", "root", "Ss123456", "1022student"))
	//{
	//	cout << "链接数据库失败" << endl;
	//	return 1;
	//}

	//////插入数据
	////string name = "孙畅";
	////int age = 19;
	////char sex[] = "女";
	////char sqlBuf[1024] = "";
	////sprintf_s(sqlBuf, "insert into t_studentinfo(name,age,sex)values('%s',%d,'%s');", name.c_str(), age, sex);
	////if (!sql.UpdateMySql(sqlBuf))
	////{
	////	cout << "更新数据库失败，sqlBuf" << sqlBuf << endl;
	////}

	//list<string>listRes;

	//char sqlBuf[1024] = "";
	//sprintf_s(sqlBuf, "select name,id,age,sex from t_studentinfo");
	//if (!sql.SelectMySql(sqlBuf,4,listRes))
	//{
	//	cout << "查询数据库失败，sqlBuf" << sqlBuf << endl;
	//	sql.DisConnect();
	//	return 1;
	//}
	////断开连接
	//sql.DisConnect();

	//struct student st;
	//while (listRes.size() > 0)
	//{
	//	//取name
	//	st.name = listRes.front();
	//	listRes.pop_front();

	//	//取id
	//	st.id = atoi(listRes.front().c_str());
	//	listRes.pop_front();

	//	//取age
	//	st.age = atoi(listRes.front().c_str());
	//	listRes.pop_front();

	//	//取sex
	//	strcpy_s(st.sex, listRes.front().c_str());
	//	listRes.pop_front();

	//	cout << "id:" << st.id << " ,name:" << st.name << " ,age:" << st.age << " ,sex:" << st.sex << endl;
	//}
	return 0;
}