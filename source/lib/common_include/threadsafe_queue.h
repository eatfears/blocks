#ifndef TREADSAFE_QUEUE_H
#define TREADSAFE_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

template<typename T>
class threadsafe_queue
{
public:
    threadsafe_queue() {}
    ~threadsafe_queue() {}

    void lock(){
        m_Lock.lock();
    }

    void unlock(){
        m_Lock.unlock();
    }

    bool empty(){
        std::lock_guard<std::mutex> lock(m_Lock);
        return m_Queue.empty();
    }

    void push(T val){
        m_Lock.lock();
        m_Queue.push(val);
        m_Lock.unlock();
        m_Condition.notify_all();
    }

    T front_pop(){
        std::lock_guard<std::mutex> lock(m_Lock);
        auto tmp = m_Queue.front();
        m_Queue.pop();
        return tmp;
    }

    void wait_for_data(std::function<bool()> func = []{return false;}, uint32_t ms = 100){
        std::unique_lock<std::mutex> lock(m_Lock);
        while (m_Queue.empty() && !func())
        {
            m_Condition.wait_for(lock, std::chrono::milliseconds(ms), [this, &func]{return !this->m_Queue.empty() || func();});
        }
    }

private:
    std::queue<T> m_Queue;
    std::mutex m_Lock;
    std::condition_variable m_Condition;
};

#endif // TREADSAFE_QUEUE_H
