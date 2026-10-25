#include "worker_pool.hpp"

namespace har
{
    void worker_pool::initialize(u32 threadCount)
    {
        mThreadAmount = std::min(threadCount, std::thread::hardware_concurrency());
        
        mThreads.reserve(mThreadAmount);
        for (u32 i = 0; i < mThreadAmount; ++i)
            mThreads.emplace_back(std::thread(&worker_pool::worker_loop, this));
    }

    void worker_pool::shutdown()
    {
        {
            std::unique_lock<std::mutex> lock(mQueueMutex);
            mIsAvailable = false;
        }

        mConditionVar.notify_all();
        for (auto& thread : mThreads)
            thread.join();
    }

    void worker_pool::insert_job(const worker_function& job)
    {
        {
            std::unique_lock<std::mutex> lock(mQueueMutex);
            mJobs.push(job);
        }

        mConditionVar.notify_one();
    }

    void worker_pool::worker_loop()
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
}