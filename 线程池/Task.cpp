#include "Task.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
using namespace std;

Task::Task() :_priority(5)//���캯��
{}

Task::Task(int priority) : _priority(priority)//�����ȼ������Ĺ��캯��
{}

Task::~Task()//��������
{}

void Task::setPriority(int priority)//�����������ȼ�
{
	_priority = priority;
}

int Task::getPriority()const//��ȡ�������ȼ�
{
	return _priority;
}

MyTask::MyTask() :_data(0)//���캯��
{}

MyTask::MyTask(int data) : _data(data)//�����ݲ����Ĺ��캯��
{}

MyTask::MyTask(int data, int priority) : Task(priority), _data(data)//�����ݺ����ȼ������Ĺ��캯��
{}

MyTask::~MyTask()//��������
{}

void MyTask::setData(int data)//��������
{
	_data = data;
}

int MyTask::getData()//��ȡ����
{
	return _data;
}

void MyTask::run()//�������к���
{
	this_thread::sleep_for(chrono::milliseconds(200));//����0.2s��ʾ����ʱ��
	cout << "thread :" << this_thread::get_id() << " deal with data :" << _data << " priority: " << getPriority()<<endl;//���������Ϣ
}
