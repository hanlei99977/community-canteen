#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>

class ThreadPool {

public:

    ThreadPool(size_t threads = std::thread::hardware_concurrency());

    ~ThreadPool();

    void enqueue(std::function<void()> task);

private:

    std::vector<std::thread> workers;//工作中的线程

    std::queue<std::function<void()>> tasks;//任务队列

    std::mutex queueMutex;

    std::condition_variable condition;

    bool stop;//线程池是否停止

};

#endif