#ifndef _TASK_H_
#define _TASK_H_
#include <memory>

//����Ļ���
class Task
{
public:
	Task();//���캯��
	Task(int priority);//�������ȼ������Ĺ��캯��
	virtual ~Task();//��������
	virtual void run()=0;//������Ҫ���еĺ��������麯����������Ҫʵ���������
	void setPriority(int priority);//�������ȼ�
	int getPriority()const;//��ȡ���ȼ�
protected:
	int _priority;//��������ȼ�
};

//�����������
class MyTask :public Task
{
public:
	MyTask();//���캯��
	MyTask(int data);//�������ݲ����Ĺ��캯��
	MyTask(int data,int priority);//�������ݲ��������ȼ��Ĺ��캯��
	~MyTask();//��������
	virtual void run();//�������еĺ���
	void setData(int data);//��������
	int getData();//��ȡ����
private:
	int _data;//����
};


#endif