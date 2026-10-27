#pragma once
#include "core/defines.hpp"
#include "core/event/event.hpp"
#include "core/event/event_type.hpp"
#include "core/memory/memory_tag.hpp"
#include "core/containers/containers.hpp"
#include "core/containers/safe_containers.hpp"

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
        
        constexpr inline u64 get_required_transient_memory()
        {
            return sizeof(event) * queueCapacity;
        }

    public:
        u8 queueCapacity = 50;
        u8 peeperCount = 3;
    };

    class event_queue
    {
    public:
        event_queue(const event_queue_desc& desc)
            : mEventRing(desc.queueCapacity)
        {
            mPeepers.reserve(desc.peeperCount);
        }

    public:
        void register_peeper(event_peeper&& peeps)
        {
            mPeepers.emplace_back(std::move(peeps));
        }

    public:
        void process_events()
        {
            while (!mEventRing.is_empty())
            {
                // TODO: It currently behaves like a stack. Perhaps a queue would fit better.
                event nextEvent;
                if (!mEventRing.pop_front(nextEvent))
                    break;

                if (nextEvent.type == event_type::invalid || nextEvent.handled)
                    continue;

                for (const auto& peeper : mPeepers)
                    peeper.callback(nextEvent);
            }
        }
    
        void push(event&& event)
        {
            mEventRing.push_back(std::move(event));
        }

    private:
        thread_safe::safe_transient_vector<event, memory_tag::event_queue> mEventRing;
        har_static_vector<memory_tag::event_queue, event_peeper> mPeepers;
    };
}