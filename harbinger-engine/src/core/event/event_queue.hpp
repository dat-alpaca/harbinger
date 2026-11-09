#pragma once
#include <condition_variable>
#include <mutex>

#include "core/defines.hpp"
#include "core/event/event.hpp"
#include "core/memory/memory_tag.hpp"
#include "core/containers/containers.hpp"

namespace har
{   
    struct event_peeper
    {
        event_callback callback;
    };

    struct event_queue_desc
    {
    public:
        constexpr inline u64 get_required_static_memory()
        {
            return sizeof(event_peeper) * peeperCount;
        }

    public:
        u8 queueCapacity = 16;
        u8 peeperCount = 3;
    };

    class event_queue
    {
        using event_queue_t = har_vector<memory_tag::event_queue, event>;

    public:
        explicit event_queue(const event_queue_desc& desc);

    public:
        void register_peeper(event_peeper&& peeps);

    public:
        void process_events();

    public:
        void push(event&& event);

    private:
        event_queue_t* mProcessingQueue = nullptr;
        event_queue_t* mAccumulatorQueue = nullptr;
        event_queue_t mProcessingQueueBuffer;
        event_queue_t mAccumulatorQueueBuffer;
        har_static_vector<memory_tag::event_queue, event_peeper> mPeepers;
        
    private:
        std::condition_variable mAccumulatorNotFull;
        std::mutex mQueueMutex;

    private:
        u32 mQueueCapacity = 0;
    };
}