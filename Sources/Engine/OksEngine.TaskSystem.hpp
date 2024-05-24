#pragma once
#include <string>
#include <thread>
#include <map>
#include <concepts>

#include <Common.hpp>
#include <Datastructures.ThreadSafeQueue.hpp>

namespace OksEngine {

	
	//Система, которая позволяет передавать произвольные данные между разными потоками.

	template<class Type, Common::Enum ThreadType>
	class MTDataExchangeSystem {
	private:

		struct DataInfo {
			ThreadType senderThreadType_ = ThreadType::Undefined;
			ThreadType receiverThreadType_ = ThreadType::Undefined;
			Type data_;
		};

		using DataQueue = DS::ThreadSafeQueue<DataInfo>;
		using DataQueuePtr = std::shared_ptr<DataQueue>;
		using ThreadId = Common::UInt8;

		constexpr static inline ThreadId invalidThreadId_ = 0;
		constexpr static inline ThreadId maxThreadId_ = std::numeric_limits<ThreadId>::max();

		struct ThreadInfo {
			ThreadId threadId_ = invalidThreadId_;
			ThreadType senderType_ = ThreadType::Undefined;
			DataQueuePtr inQueue_ = nullptr;

			ThreadType receiverType_ = ThreadType::Undefined;
			DataQueuePtr outQueue_ = nullptr;
		};

		[[nodiscard]]
		static ThreadId GetThreadId() {
			static std::atomic<ThreadId> threadId_ = invalidThreadId_;
			static thread_local ThreadId id = invalidThreadId_;
			if (id == invalidThreadId_) {
				OS::AssertMessage(threadId_.load() != std::numeric_limits<ThreadId>::max(), "");
				id = ++threadId_;
			}
			return id;
		}

		[[nodiscard]]
		ThreadInfo& GetThreadInfo() {
			const ThreadId threadId = GetThreadId();
			return threadInfos_[threadId];
		}

		ThreadInfo threadInfos_[maxThreadId_ + 1];

		void AddInQueue() {
			ThreadInfo& threadInfo = GetThreadInfo();
			const ThreadId threadId = GetThreadId();
			if(threadInfo.threadId_ != invalidThreadId_) {
				OS::AssertMessage(
					threadInfo.threadId_ == threadId,
					"Incorrect logic of setting thread id.");
			}
			threadInfo.threadId_ = threadId;
			if (!threadInfo.inQueue_) {
				threadInfo.inQueue_ = std::make_shared<DataQueue>();
			}
		}

		void AddOutQueue() {
			ThreadInfo& threadInfo = GetThreadInfo();
			const ThreadId threadId = GetThreadId();
			if (threadInfo.threadId_ != invalidThreadId_) {
				OS::AssertMessage(
					threadInfo.threadId_ == threadId,
					"Incorrect logic of setting thread id.");
			}
			threadInfo.threadId_ = threadId;
			if (!threadInfo.outQueue_) {
				threadInfo.outQueue_ = std::make_shared<DataQueue>();
			}
		}

	public:

		[[nodiscard]]
		bool IsThreadRegistered() const {
			const ThreadInfo& threadInfo = GetThreadInfo();
			return (!threadInfo.inQueue_ && !threadInfo.outQueue_);
		}


		void Update() {

			for(ThreadInfo& threadInfo : threadInfos_) {
				if (!threadInfo.inQueue_) { continue; }
				DataQueuePtr inQueue = threadInfo.inQueue_;
				while(!inQueue->IsEmpty()) {
					std::optional<DataInfo> data = inQueue->Pop();
					OS::AssertMessage(data.has_value(), "");
					DataInfo dataInfo = data.value();
					ThreadType receiverThreadType = dataInfo.receiverThreadType_;
					for(ThreadInfo& threadInfo : threadInfos_) {
						if (!threadInfo.outQueue_) { continue; }
						if(threadInfo.receiverType_ == receiverThreadType){
							threadInfo.outQueue_->Push(dataInfo);
						}
					}
				}
			}
		}

		void AddData(ThreadType senderThreadType, ThreadType receiverThreadType, Type&& data) {

			OS::AssertMessage(
				senderThreadType != receiverThreadType,
				"Attempt to use different names for one thread.");

			ThreadInfo& senderThreadInfo = GetThreadInfo();
			senderThreadInfo.senderType_ = senderThreadType;

			if (!senderThreadInfo.inQueue_) { AddInQueue(); };

			DataInfo dataInfo{
				senderThreadType,
				receiverThreadType,
				std::move(data)
			};

			senderThreadInfo.inQueue_->Push(dataInfo);
		}

		[[nodiscard]]
		std::optional<DataInfo> GetData(ThreadType senderThreadType, ThreadType receiverThreadType) {

			OS::AssertMessage(
				senderThreadType != receiverThreadType,
				"Attempt to use different names for one thread.");

			ThreadInfo& receiverThreadInfo = GetThreadInfo();
			if (receiverThreadInfo.receiverType_ == ThreadType::Undefined) {
				receiverThreadInfo.receiverType_ = receiverThreadType;
			}

			if (!receiverThreadInfo.outQueue_) { AddOutQueue(); };
			DataQueuePtr queue = receiverThreadInfo.outQueue_;

			std::optional<DataInfo> maybeDataInfo = queue->Pop();
			if(maybeDataInfo.has_value()) {
				DataInfo dataInfo = maybeDataInfo.value();
				return dataInfo/*.data_*/;
			}
			return { };
		}

	};

}
