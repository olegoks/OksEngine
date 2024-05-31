#pragma once
#include <string>
#include <thread>
#include <map>
#include <concepts>

#include <Common.hpp>
#include <Datastructures.ThreadSafeQueue.hpp>

#include <boost/lockfree/queue.hpp>

namespace OksEngine {


	//Система, которая позволяет передавать произвольные данные между разными потоками.

	template<class Type>
	class MTDataExchangeSystem {
	public:

		using ThreadId = Common::UInt8;
		using DataQueue = boost::lockfree::queue<Type>;//DS::ThreadSafeQueue<DataInfo>;
		using DataQueuePtr = std::shared_ptr<DataQueue>;

		struct ThreadInfo {
			std::atomic<ThreadId> threadId_;
			//std::atomic<ThreadType> sender_ = ThreadType::Undefined;
			std::atomic<DataQueuePtr> inDataQueue_ = nullptr;
			//std::atomic<ThreadType> receiver_ = ThreadType::Undefined;
			std::atomic<DataQueuePtr> outDataQueue_ = nullptr;
			DS::Vector<Type> cachedOutData_;
		};

		MTDataExchangeSystem() {
			
		}

		constexpr static inline ThreadId invalidThreadId_ = 0;
		constexpr static inline ThreadId maxThreadId_ = std::numeric_limits<ThreadId>::max();


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
			OS::AssertMessage(threadId != invalidThreadId_, "Got invalid thread id.");
			return threadInfos_[threadId];
		}

		ThreadInfo threadInfos_[maxThreadId_ + 1];

		void AddInQueue() {
			ThreadInfo& threadInfo = GetThreadInfo();
			const ThreadId threadId = GetThreadId();
			if(threadInfo.threadId_.load() != invalidThreadId_) {
				OS::AssertMessage(
					threadInfo.threadId_ == threadId,
					"Incorrect logic of setting thread id.");
			} else {
				threadInfo.threadId_.store(threadId);
			}
			if (!threadInfo.inDataQueue_.load()) {
				threadInfo.inDataQueue_ = std::make_shared<DataQueue>();
			}
		}

		void AddOutQueue() {
			ThreadInfo& threadInfo = GetThreadInfo();
			const ThreadId threadId = GetThreadId();
			if (threadInfo.threadId_.load() != invalidThreadId_) {
				OS::AssertMessage(
					threadInfo.threadId_.load() == threadId,
					"Incorrect logic of setting thread id.");
			} else {
				threadInfo.threadId_.store(threadId);
			}
			if (!threadInfo.outDataQueue_.load()) {
				threadInfo.outDataQueue_ = std::make_shared<DataQueue>();
			}
		}

	public:

		[[nodiscard]]
		bool IsThreadRegistered() const {
			const ThreadInfo& threadInfo = GetThreadInfo();
			return (!threadInfo.inDataQueue_ && !threadInfo.outDataQueue_);
		}


		void Update() {

			for (ThreadInfo& threadInfo : threadInfos_) {
				if (!threadInfo.inDataQueue_.load()) { continue; }
				DataQueuePtr inQueue = threadInfo.inDataQueue_;

				while (inQueue->CanPop()) {
					DataInfo dataInfo;
					const bool isPoped = inQueue->TryPop(dataInfo);
					ThreadType receiverThreadType = dataInfo.receiverThreadType_;
					for (ThreadInfo& threadInfo : threadInfos_) {
						if (!threadInfo.outDataQueue_.load()) { continue; }
						if (threadInfo.receiver_ == receiverThreadType) {
							threadInfo.outDataQueue_.load()->Push(dataInfo);
						}
					}
				}
			}
		}

		void AddData(Type&& data) {
			ThreadInfo& senderThreadInfo = GetThreadInfo();
			senderThreadInfo.inDataQueue_.load()->Push(std::move(data));
		}

		std::optional<Type> GetData(std::function<bool(const DataInfo& dataInfo)> filter) {
			ThreadInfo& threadInfo = GetThreadInfo();
			std::lock_guard guard{ threadInfo.mutex_ };
			for(Common::Index i = 0; i < threadInfo.cachedOutData_.GetSize(); i++) {
				const Type& data = threadInfo.cachedOutData_[i];
				const bool isFound = filter(dataInfo);
				if(isFound){
					Type foundData = data;
					threadInfo.cachedOutData_.Erase(i);
					return foundData;
				}
			}
			Type maybeData;
			while (threadInfo.outDataQueue_.load()->TryPop(maybeData)) {
				if(filter(maybeDataInfo)) {
					return maybeDataInfo;
				} else {
					threadInfo.cachedOutData_.PushBack(std::move(maybeData));
				}
			}
			return {};
			
		}

		[[nodiscard]]
		DataInfo WaitForData(std::function<bool(const DataInfo& dataInfo)> filter) {
			while(true) {
				auto maybeDataInfo = GetData(filter);
				if(!maybeDataInfo.has_value()) {
					std::this_thread::yield();
					continue;
				}
				DataInfo dataInfo = maybeDataInfo.value();
				if(filter(dataInfo)) {
					return dataInfo;
				} else {
					ThreadInfo& threadInfo = GetThreadInfo();
					std::lock_guard guard{threadInfo.mutex_};
					threadInfo.cachedOutData_.PushBack(dataInfo);
				}
			}

		}


		[[nodiscard]]
		std::optional<DataInfo> GetData(ThreadType senderThreadType, ThreadType receiverThreadType) {

			OS::AssertMessage(senderThreadType != ThreadType::Undefined, "");
			OS::AssertMessage(receiverThreadType != ThreadType::Undefined, "");
			OS::AssertMessage(
				senderThreadType != receiverThreadType,
				"Attempt to use different names for one thread.");


			ThreadInfo& receiverThreadInfo = GetThreadInfo();
			
			if (receiverThreadInfo.receiver_ == ThreadType::Undefined) {
				receiverThreadInfo.receiver_ = receiverThreadType;
			}

			AddOutQueue();
			DataQueuePtr queue = receiverThreadInfo.outDataQueue_;

			DataInfo dataInfo;
			bool isPoped = queue->TryPop(dataInfo);
			if (isPoped) {
				OS::LogInfo("Debug/MTExch", { "GOT data" });
				return dataInfo/*.data_*/;
			}
			return { };
		}

	};

}
