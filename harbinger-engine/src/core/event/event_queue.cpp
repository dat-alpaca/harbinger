#include "event_queue.hpp"

#include <condition_variable>
#include <mutex>

namespace har
{   
    event_queue::event_queue(const event_queue_desc& desc)
        : mQueueCapacity(desc.queueCapacity / 2)
    {
        mPeepers.reserve(desc.peeperCount);
        mProcessingQueue = &mProcessingQueueBuffer;
        mAccumulatorQueue = &mAccumulatorQueueBuffer;
    }

        void event_queue::register_peeper(event_peeper&& peeps)
        {
            mPeepers.emplace_back(std::move(peeps));
        }

        void event_queue::process_events()
        {
            event_queue_t* currentQueue = nullptr;
            {
                std::lock_guard<std::mutex> lock(mQueueMutex);
                std::swap(mProcessingQueue, mAccumulatorQueue);

                mAccumulatorQueue->clear();

                mAccumulatorNotFull.notify_all();
                currentQueue = mProcessingQueue;
            }

            for (u64 i = 0; i < currentQueue->size(); ++i)
            {
                event currentEvent = currentQueue->at(i);
                if (currentEvent.type == event_type::invalid || currentEvent.handled)
                    continue;

                for (const auto& peeper : mPeepers)
                    peeper.callback(currentEvent);
            }
        }

        void event_queue::push(event&& event)
        {
            std::unique_lock<std::mutex> lock(mQueueMutex);
            mAccumulatorNotFull.wait(lock, [this]()
            {
                return mAccumulatorQueue->size() < mQueueCapacity;
            });

            mAccumulatorQueue->push_back(std::move(event));
        }

}