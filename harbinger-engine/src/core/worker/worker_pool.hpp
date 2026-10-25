#pragma once
#include <queue>
#include <mutex>
#include <thread>
#include <functional>
#include <condition_variable>

#include "core/defines.hpp"
#include "core/event/event.hpp"
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
        void initialize(u32 threadCount = std::thread::hardware_concurrency());
        void on_event(event& event);
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