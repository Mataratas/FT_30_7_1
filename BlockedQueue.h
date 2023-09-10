#pragma once
#include <queue>
#include<future>
#include<condition_variable>

template<typename T>
class BlockedQueue{
public:
    void push(T& item) { // обычный потокобезопасный push
        std::lock_guard<std::mutex> l(m_locker);
        m_task_queue.push(item);
        m_notifier.notify_one(); // делаем оповещение, чтобы поток, вызвавший pop, проснулся и забрал элемент из очереди
    }
    
    void pop(T& item) { // блокирующий метод получения элемента из очереди
        std::unique_lock<std::mutex> l(m_locker);
        if (m_task_queue.empty())            
            m_notifier.wait(l, [this] {return !m_task_queue.empty(); }); // ждем, пока вызовут push
        item = m_task_queue.front(); //push вызван
        m_task_queue.pop();
    }
    bool fast_pop(T& item) {// unblocking work stealing
        std::lock_guard<std::mutex> l(m_locker);
        if (m_task_queue.empty())// просто выходим
            return false;
        item = m_task_queue.front();// забираем элемент
        m_task_queue.pop();
        return true;
    }
private:
    std::mutex m_locker;
    std::queue<T> m_task_queue;          // очередь задач
    std::condition_variable m_notifier;  // уведомитель
};

