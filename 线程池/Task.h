#ifndef _TASK_H_
#define _TASK_H_
#include <memory>

//任务的基类
class Task
{
public:
	Task();//构造函数
	Task(int priority);//带有优先级参数的构造函数
	virtual ~Task();//析构函数
	virtual void run()=0;//任务需要运行的函数，纯虚函数，子类需要实现这个函数
	void setPriority(int priority);//设置优先级
	int getPriority()const;//获取优先级
protected:
	int _priority;//任务的优先级
};

//具体的任务类
class MyTask :public Task
{
public:
	MyTask();//构造函数
	MyTask(int data);//带有数据参数的构造函数
	MyTask(int data,int priority);//带有数据参数和优先级的构造函数
	~MyTask();//析构函数
	virtual void run();//任务运行的函数
	void setData(int data);//设置数据
	int getData();//获取数据
private:
	int _data;//数据
};


#endif