
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
	//// �����������
	//INetMediator* m_pServer = new TcpServerMediator;
	//if (!m_pServer->OpenNet())
	//{
	//	cout << "�򿪷���������ʧ��" << endl;
	//	return 1;
	//}
	
	////�������ͻ���
	// INetMediator* m_pClient = new TcpClientMediator;
	//if (!m_pClient->OpenNet())
	//{
	//	cout << "�򿪿ͻ�������ʧ��" << endl;
	//	return 1;
	//}

	//// �ͻ��˸���������������
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
	//// �������ݿ�
	//if (!sql.ConnectMySql("127.0.0.1", "root", "Ss123456", "1022student"))
	//{
	//	cout << "�������ݿ�ʧ��" << endl;
	//	return 1;
	//}

	//////��������
	////string name = "�ﳩ";
	////int age = 19;
	////char sex[] = "Ů";
	////char sqlBuf[1024] = "";
	////sprintf_s(sqlBuf, "insert into t_studentinfo(name,age,sex)values('%s',%d,'%s');", name.c_str(), age, sex);
	////if (!sql.UpdateMySql(sqlBuf))
	////{
	////	cout << "�������ݿ�ʧ�ܣ�sqlBuf" << sqlBuf << endl;
	////}

	//list<string>listRes;

	//char sqlBuf[1024] = "";
	//sprintf_s(sqlBuf, "select name,id,age,sex from t_studentinfo");
	//if (!sql.SelectMySql(sqlBuf,4,listRes))
	//{
	//	cout << "��ѯ���ݿ�ʧ�ܣ�sqlBuf" << sqlBuf << endl;
	//	sql.DisConnect();
	//	return 1;
	//}
	////�Ͽ�����
	//sql.DisConnect();

	//struct student st;
	//while (listRes.size() > 0)
	//{
	//	//ȡname
	//	st.name = listRes.front();
	//	listRes.pop_front();

	//	//ȡid
	//	st.id = atoi(listRes.front().c_str());
	//	listRes.pop_front();

	//	//ȡage
	//	st.age = atoi(listRes.front().c_str());
	//	listRes.pop_front();

	//	//ȡsex
	//	strcpy_s(st.sex, listRes.front().c_str());
	//	listRes.pop_front();

	//	cout << "id:" << st.id << " ,name:" << st.name << " ,age:" << st.age << " ,sex:" << st.sex << endl;
	//}
	return 0;
}