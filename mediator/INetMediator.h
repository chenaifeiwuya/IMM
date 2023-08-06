#pragma once

class CKernel; //���߱�����������࣬���ǻ�δ����
class INet;//���߱�����������࣬���ǻ�δ����
class INetMediator
{
   
public:
    INetMediator();
    virtual ~INetMediator();
	//������
    virtual bool OpenNet()=0;
	//��������
	virtual bool SendData(long ISendIp, char* buf, int nLen) = 0;
	//�ر����硢
	virtual void CloseNet() = 0;
	//��������
	virtual void DealData(long ISendIp, char* buf, int nLen) = 0;

protected:
	INet* m_pNet;    //����ӿ���ָ��
	CKernel* p_Kernel;
};
