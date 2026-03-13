#include <iostream>
#include "ThreadPool.h"

using namespace std;

ThreadPool::ThreadPool(size_t threads)
{
    stop = false;//线程池开始时不停止

    for(size_t i = 0; i < threads; ++i)
    {
        //创建工作线程
        workers.emplace_back(
        //[this] 捕获 this 指针，使得线程函数可以访问线程池的成员变量和方法
        [this]
        {
            //工作线程不断从任务队列中取任务并执行，直到线程池停止
            while(true)
            {
                //std::function 是一个可调用对象的包装器，可以存储函数、lambda表达式、绑定表达式或其他函数对象。
                std::function<void()> task;//function<void()> 表示一个不接受参数且没有返回值的可调用对象

                {
                    std::unique_lock<std::mutex> lock(this->queueMutex);
                    //wait() 加锁状态检查条件，如果条件不满足 解锁并睡眠等待，直到被 notify 唤醒后重新加锁并检查条件
                    this->condition.wait(lock,
                    [this]
                    {
                        //等待条件：线程池停止或者任务队列不为空
                        return this->stop || !this->tasks.empty();
                    });

                    if(this->stop && this->tasks.empty())
                        return;

                    //从任务队列中取出一个任务
                    task = std::move(this->tasks.front());//std::move 将任务从队列中移动到 task 变量中。避免函数对象复制的性能优化。
                    this->tasks.pop();
                }

                task();//task 是一个可调用对象，直接调用它就会执行任务中的代码
            }
        });
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }

    condition.notify_all();//唤醒所有线程

    for(std::thread &worker : workers)
        worker.join();//等待所有线程结束,join() 会阻塞当前线程，直到 worker 线程执行完毕并退出
}

void ThreadPool::enqueue(std::function<void()> task)
{
    {
        std::unique_lock<std::mutex> lock(queueMutex);

        tasks.emplace(task);
    }

    condition.notify_one();//唤醒一个线程
}
