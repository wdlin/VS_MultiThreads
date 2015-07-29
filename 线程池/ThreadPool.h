#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_
#include <iostream>
#include <mutex>
#include <vector>
#include <list>
#include <queue>
#include <memory>
#include <condition_variable>
#include <thread>
#include "Task.h"
using namespace std;
class ThreadPool;

//�����̣߳����̳߳��������ɹ����߳�
class WorkThread
{
public:
	WorkThread(ThreadPool* pool);//�����̳߳�ָ��Ĺ��캯��
	~WorkThread();//��������
	void run();//���߳����еĺ���
	void notify();//���ѱ��̵߳ĺ���
	void finish();//�������̵߳ĺ���
	void assign(shared_ptr<Task> atask);//ָ���̵߳�����
	thread myThread;//���̵߳Ķ���
private:
	ThreadPool* thread_pool;//�̳߳�
	shared_ptr<Task> task;//����
	bool isEnd;//�����̱߳�־
	mutex mut;//����
	condition_variable cv;//��������
};

class cmp//���ڱȽ�����Task����ָ�����ȼ� ����1
{
public:
	bool operator()(const shared_ptr<Task>& task1, const shared_ptr<Task>& task2)
	{
		return task1->getPriority() > task2->getPriority();
	}
};

//���ڱȽ�����Task����ָ�����ȼ� ����2
bool operator<(const shared_ptr<Task>& task1, const shared_ptr<Task>& task2);

//�̳߳�
class ThreadPool
{
public:
	ThreadPool(int num = 5);//���캯��,�����߳�����
	~ThreadPool();//����������������Դ
	void run();//�̳߳����߳����еĺ���
	void finish();//�����̳߳صĺ���
	void addTask(shared_ptr<Task> task);//�������
	void addIdleThread(WorkThread* idleThread);//�������߳���ӵ����ж���
private:
	int thread_number;//�̵߳���Ŀ
	bool isEnd;//�̳߳ؽ������
	thread pool_thread;//�̳߳����߳�
	mutex task_mutex;//������еĻ�����
	mutex idle_mutex;//�����̶߳��еĻ�����
	condition_variable cv_task;//������е���������
	condition_variable cv_idle;//�����̶߳��е���������
	vector<WorkThread*> all_threads;//���������߳�
	list<WorkThread*> idle_threads;//���е��߳�
	priority_queue<shared_ptr<Task>, vector<shared_ptr<Task>>> task_queue;//������У�����������ȼ�
	//priority_queue<shared_ptr<Task>, vector<shared_ptr<Task>>,cmp> task_queue;//�������ȶ�����һ�ַ�ʽ
};

#endif