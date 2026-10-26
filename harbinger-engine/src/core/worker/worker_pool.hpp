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

    struct worker_pool_desc
    {
    public:
        inline u64 required_dynamic_memory_worker_pool()
        {
            return sizeof(std::thread) * threadCount;
        }

    public:
        u32 threadCount = std::thread::hardware_concurrency();
    };

    class worker_pool
    {
    public:
        void initialize(const worker_pool_desc& desc);
        void shutdown();

    public:
        void insert_job(const worker_function& job);

    private:
        void worker_loop();

    private:
        har_vector<memory_tag::worker_system, std::thread> mThreads;
        std::condition_variable mConditionVar;
        std::queue<worker_function> mJobs;
        std::mutex mQueueMutex;
        u32 mThreadAmount = 0;

        bool mIsAvailable : 1 = true;
    };
}