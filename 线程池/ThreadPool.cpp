#include "ThreadPool.h"
#include <iostream>
#include <thread>
#include <cassert>
WorkThread::WorkThread(ThreadPool* pool) :thread_pool(pool), task(nullptr), isEnd(false)
{
	/*
	thread t(&WorkThread::run, this);
	t.detach();
	����������д�ģ�����Ϊ�����̳߳ؽ�����ȷ������Դ�����Բ�����detach��
	detach�����е��߳����н������л�����Դ�����Ǳ��̳߳أ�WorkThread��new��ʽ������
	�������deleteʱ���̻߳�û�н���������
	*/
	myThread = thread(&WorkThread::run, this);
}

WorkThread::~WorkThread()
{
}

void WorkThread::run()
{
	while (!isEnd)
	{
		unique_lock<mutex> lock(mut);
		cv.wait(lock);//�ȴ�����
		if (task != nullptr)//�������������ִ�У�����������Ѳ�ִ�У�Ϊ�˽����̣߳�
		{
			//cout << "Thread:" << this_thread::get_id() << endl;
			task->run();
			task = nullptr;
			thread_pool->addIdleThread(this);
		}
	}	
}

void WorkThread::notify()//�����Լ�
{
	cv.notify_one();
}

void WorkThread::finish()//�������߳�
{
	isEnd = true;
	cv.notify_one();
}
void WorkThread::assign(shared_ptr<Task> atask)//ָ��������߳�
{
	task = atask;
}

ThreadPool::ThreadPool(int num) : thread_number(num), isEnd(false)//�̳߳ع��캯��
{
	assert(num > 0);
	all_threads.reserve(num);//����Ԥ��һ����С
	for (int i = 0; i < num; i++)//�½�һ�������߳�
	{
		WorkThread* wt = new WorkThread(this);
		all_threads.push_back(wt);//���������̶߳�����
		idle_threads.push_back(wt);//��������̶߳�����
	}
	pool_thread = thread(&ThreadPool::run,this);//�����̳߳����̣߳����ڽ��շ�������
}

ThreadPool::~ThreadPool()//��������
{
	for (auto t : all_threads)
	{
		t->myThread.join();//�ȴ��������߳̽���
	}
	for (auto t : all_threads)
	{
		delete t;//ɾ��ָ��
	}
}

void ThreadPool::run()
{
	while (!isEnd)
	{
		unique_lock<mutex> lock1(task_mutex);
		if (task_queue.empty())//����������Ϊ�գ��������ȴ�����
		{
			cv_task.wait(lock1);
		}
		if (isEnd)break;//�жϽ�����־
		shared_ptr<Task> task = task_queue.top();
		task_queue.pop();
		lock1.unlock();

		unique_lock<mutex> lock2(task_mutex);
		if (idle_threads.empty())//��������̶߳���Ϊ�գ��������ȴ�����
		{
			cv_idle.wait(lock2);
		}
		if (isEnd)break;//�жϽ�����־
		WorkThread* wt = idle_threads.front();
		idle_threads.pop_front();
		lock2.unlock();

		wt->assign(task);//ָ������
		wt->notify();//����ָ����������߳�
	}
}

void ThreadPool::finish()//�����̳߳�
{
	isEnd = true;//���ý�����־
	cv_task.notify_one();//�������߳�
	cv_idle.notify_one();//�������߳�
	for (auto t : all_threads)//�����������߳�
	{
		t->finish();
	}
	pool_thread.join();//�ȴ����߳̽�����������ȴ���������ô�п��ܵ���finnish��ThreadPool���������������߳�û�����ͱ�����
}

void ThreadPool::addTask(shared_ptr<Task> task)//�������
{
	unique_lock<mutex> lock(task_mutex);
	task_queue.push(task);
	cout <<"thread:"<<this_thread::get_id() << "  addTask: priority:" << task->getPriority() << ",data:" << dynamic_cast<MyTask*>(task.get())->getData() << endl;
	lock.unlock();
	cv_task.notify_one();//��Ӻ�������в�Ϊ�գ����Ի������̴߳���
}

void ThreadPool::addIdleThread(WorkThread* idleThread)//��ӵ����е��̶߳���
{
	unique_lock<mutex> lock(idle_mutex);
	idle_threads.push_back(idleThread);
	lock.unlock();
	cv_idle.notify_one();//��Ӻ󣬿����̶߳��в�Ϊ�գ����Ի������̴߳���
}
//shared_ptr<Task>�ȽϺ���
bool operator<(const shared_ptr<Task>& task1, const shared_ptr<Task>& task2)
{
	return task1->getPriority() < task2->getPriority();
}