#pragma once
#include <queue>
#include <mutex>
#include <thread>
#include <functional>
#include <condition_variable>

#include "core/defines.hpp"
#include "core/memory/memory_tag.hpp"
#include "core/containers/containers.hpp"

namespace har
{
    using worker_function = std::function<void()>;

    class worker_pool
    {
    public:
        static inline u64 required_dynamic_memory_worker_pool(u32 threadCount)
        {
            return sizeof(std::thread) * threadCount;
        }

    public:
        explicit worker_pool() { }

        ~worker_pool()
        {
            {
                std::unique_lock<std::mutex> lock(mQueueMutex);
                mIsAvailable = false;
            }

            mConditionVar.notify_all();

            for (auto& thread : mThreads)
                thread.join();
        }

    public:
        void initialize(u32 threadCount = std::thread::hardware_concurrency())
        {
            mThreadAmount = std::min(threadCount, std::thread::hardware_concurrency());
            
            mThreads.reserve(mThreadAmount);
            for (u32 i = 0; i < mThreadAmount; ++i)
                mThreads.emplace_back(std::thread(&worker_pool::worker_loop, this));
        }

    public:
        void insert_job(const worker_function& job)
        {
            {
                std::unique_lock<std::mutex> lock(mQueueMutex);
                mJobs.push(job);
            }

            mConditionVar.notify_one();
        }

    private:
        void worker_loop()
        {
            while (true)
            {
                worker_function job;
                {
                    std::unique_lock<std::mutex> lock(mQueueMutex);
                    mConditionVar.wait(lock, [this] {
                        return !mJobs.empty() || !mIsAvailable;
                    });

                    if (!mIsAvailable)
                        break;

                    job = mJobs.front();
                    mJobs.pop();
                }

                job();
            }
        }

    private:
        har_vector<memory_tag::worker_system, std::thread> mThreads;
        std::condition_variable mConditionVar;
        std::queue<worker_function> mJobs;
        std::mutex mQueueMutex;
        bool mIsAvailable = true;
        u32 mThreadAmount = 0;
    };
}