/*
1:1 연결 용으로만 사용 하길...
*/


#ifndef __DATA_QUEUE_H__
#define __DATA_QUEUE_H__

#include <queue>
#include <mutex>
#include "ara/core/promise.h"



namespace katech
{
namespace etc
{

template <typename T>
class dataQueue
{
private:
    int size;
    std::queue<T> array;
    std::mutex m_Mutex_eventQueue;
    std::shared_ptr<ara::core::Promise<void>> m_promise_wait;


public:
    dataQueue(int count)
    {
        size = count;
    }
    T pop()
    {
        std::lock_guard<std::mutex> guard(m_Mutex_eventQueue);
        auto temp = array.front();
        array.pop();
        return temp;
    }

    void push(T data)
    {
        std::lock_guard<std::mutex> guard(m_Mutex_eventQueue);

        if(static_cast<int>(array.size()) >= size) {
            //ERROR("Overflow");
            array.pop();
        }

        array.push(data);

        if(m_promise_wait != nullptr) {
            m_promise_wait->set_value();
            //m_promise_wait = nullptr;
        }
    }
    int length()
    {
        std::lock_guard<std::mutex> guard(m_Mutex_eventQueue);
        return static_cast<int>(array.size());
    }

    bool isEmpty()
    {
        std::lock_guard<std::mutex> guard(m_Mutex_eventQueue);
        return (array.size() == 0);
    }

    bool waiting(int deadLine)
    {
        ara::core::Future<void> waiting_future;
        bool triggerSet;
        {
            std::lock_guard<std::mutex> guard(m_Mutex_eventQueue);

            if((array.size() == 0) && (deadLine != 0)) {
                m_promise_wait = std::make_shared< ara::core::Promise<void>>();
                waiting_future = m_promise_wait->get_future();
                triggerSet = true;

            } else {
                triggerSet = false;
            }
        }

        if(triggerSet) {
            waiting_future.wait_for(std::chrono::milliseconds(deadLine));
        }

        return !isEmpty();
    }
};



}
}

#endif
