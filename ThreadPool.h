#pragma once
#include<iostream>
#include "BlockedQueue.h"
//=================================================================================
void taskFunc(int* array, long left, long right, bool);

typedef std::function<void()> task_type;

typedef void(*FuncType)(int*, long,long, bool);
//=================================================================================
class ThreadPool{
public:
    ThreadPool();
    void start();
    void stop();
    void push_task(FuncType f, int* ,long, long, bool);
    void threadFunc(int);
    bool wait();
private:    
    int m_thread_count{0};          
    int m_index{0};                 
    std::vector<std::thread> m_threads; 
    std::vector<BlockedQueue<task_type>> m_thread_queues; 
    std::condition_variable m_event_holder; 
    std::shared_ptr<std::promise<bool>> _ParentTaskPrmPtr{nullptr};
    std::atomic<int> _chld_cnt{ 0 };
 };

