/**********************************************************
* Holo_ToolSet
* http://github.com/HasseltVR/Holo_ToolSet
* http://www.uhasselt.be/edm
*
* Distributed under LGPL v2.1 Licence
* http ://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
**********************************************************/
#pragma once

#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>

/*
 * ThreadSafe Queue: allows for threadsafe adding elements to a FIFO queue. It also allows for multiple
 * threads to process items for the queue without having race conditions. 
 *
 * Based on "C++ Concurrency in Action" by Anthony Williams ISBN: 978-1-933988-77-1
 */
template<typename T>
class ThreadSafe_Queue
{
private:
    mutable std::mutex mtx;
    std::queue<T> data_queue;
    std::condition_variable data_cond;
    
public:
    ThreadSafe_Queue() {}
    ThreadSafe_Queue(ThreadSafe_Queue const& other)
    {
        std::lock_guard<std::mutex> lock(other.mtx);
        data_queue=other.data_queue;
    }
    
    void push(T new_value)
    {
        std::lock_guard<std::mutex> lock(mtx);
        data_queue.push(new_value);
        data_cond.notify_one();
    }
    
    void wait_and_pop(T& value)
    {
        std::unique_lock<std::mutex> lock(mtx);
        data_cond.wait(lock,[this]{return !data_queue.empty();});
        value=data_queue.front();
        data_queue.pop();
    }
    
    std::shared_ptr<T> wait_and_pop()
    {
        std::unique_lock<std::mutex> lock(mtx);
        data_cond.wait(lock,[this]{return !data_queue.empty();});
        std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
        data_queue.pop();
        return res;
    }
    
    bool try_pop(T& value)
    {
        std::lock_guard<std::mutex> lock(mtx);
        if(data_queue.empty())
            return false;
        value=data_queue.front();
        data_queue.pop();
        return true;
    }
    
    std::shared_ptr<T> try_pop()
    {
        std::lock_guard<std::mutex> lock(mtx);
        if(data_queue.empty())
            return std::shared_ptr<T>();
        std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
        data_queue.pop();
        return res;
    }
    
    bool empty() const
    {
        std::lock_guard<std::mutex> lock(mtx);
        return data_queue.empty();
    }
    
    std::size_t size()
    {
        std::lock_guard<std::mutex> lock(mtx);
        return data_queue.size();
    }

    void clear()
    {
        std::lock_guard<std::mutex> lock(mtx);
        std::queue<T> empty;
        std::swap(data_queue, empty);
    }
};