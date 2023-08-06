//#include "stdafx.h"
#include "CMySql.h"


CMySql::CMySql(void)
{
    /*�����������������߳�ʼ��һ��MYSQL������������mysql����ˡ�
    ����㴫��Ĳ�����NULLָ�룬�����Զ�Ϊ�����һ��MYSQL����
    ������MYSQL���������Զ�����ģ���ô�ڵ���mysql_close��ʱ�򣬻��ͷ��������*/
    m_sock = new MYSQL;
    mysql_init(m_sock);  
    mysql_set_character_set(m_sock, "gb2312"); //gb2312 �л����񹲺͹������ֱ�׼
}


CMySql::~CMySql(void)
{
    if(m_sock) {
		delete m_sock;
		m_sock = NULL;
    }
}

void CMySql::DisConnect()
{
    mysql_close(m_sock);
}

bool CMySql::ConnectMySql(const char *host, const char *user,const char *pass,const char *db, short nport)
{
	if (!mysql_real_connect(m_sock, host, user, pass, db, nport, NULL, CLIENT_MULTI_STATEMENTS)) {
        //���Ӵ���
		return false;
	}
    return true;
}
 bool  CMySql::GetTables(char* szSql, list<string>& lstStr)
 {
    if(mysql_query(m_sock, szSql)) {
		return false;
	}

	m_results = mysql_store_result(m_sock);
    if(NULL == m_results) {
		return false;
	}
	while (m_record = mysql_fetch_row(m_results)) {
		lstStr.push_back(m_record[0]);
	}
    return true;
 }
bool CMySql::SelectMySql(char* szSql, int nColumn, list<string>& lstStr)
{
    //mysql_query() ���������� MySQL ���Ͳ�ִ�� SQL ���
	if(mysql_query(m_sock, szSql)) {
		return false;
	}

     /*��mysql_store_result ���ڳɹ����������ݵ�ÿ����ѯ(SELECT��SHOW��DESCRIBE��EXPLAIN��CHECK TABLE��)
     ����ֵ:
     . CR_COMMANDS_OUT_OF_SYNC �����Բ�ǡ����˳��ִ�������
 ������ CR_OUT_OF_MEMORY �����ڴ������
 ������ CR_SERVER_GONE_ERROR ����MySQL�����������á�
 ������ CR_SERVER_LOST �����ڲ�ѯ�����У�������������Ӷ�ʧ��
 ������ CR_UNKNOWN_ERROR ��������δ֪����*/
	m_results = mysql_store_result(m_sock);
    if(NULL == m_results)return false;
	//�������е���һ�У�ȡ�����ݷ���m_record �����
	while (m_record = mysql_fetch_row(m_results)) {
        
		for(int i = 0; i < nColumn; i++) {
			if(!m_record[i]) {
				lstStr.push_back("");
			} else {
				lstStr.push_back(m_record[i]);
			}
        }
	}
    return true;
}

 bool  CMySql::UpdateMySql(char* szSql)
 {
    if(!szSql) {
		return false;
	}
    if(mysql_query(m_sock, szSql)) {
		return false;
	}
    return true;
 }

 /*
 
 
 
 
 
 
 */


