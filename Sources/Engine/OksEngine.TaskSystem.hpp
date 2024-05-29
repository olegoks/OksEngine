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
	public:

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
			std::atomic<ThreadId> threadId_;
			std::atomic<ThreadType> sender_ = ThreadType::Undefined;
			std::atomic<DataQueuePtr> inQueue_ = nullptr;
			std::atomic<ThreadType> receiver_ = ThreadType::Undefined;
			std::atomic<DataQueuePtr> outQueue_ = nullptr;
			std::mutex mutex_;
			DS::Vector<DataInfo> cachedOutData_;
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
			if (!threadInfo.inQueue_.load()) {
				threadInfo.inQueue_ = std::make_shared<DataQueue>();
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
			if (!threadInfo.outQueue_.load()) {
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

			for (ThreadInfo& threadInfo : threadInfos_) {
				if (!threadInfo.inQueue_.load()) { continue; }
				DataQueuePtr inQueue = threadInfo.inQueue_;

				while (inQueue->CanPop()) {
					DataInfo dataInfo;
					const bool isPoped = inQueue->TryPop(dataInfo);
					ThreadType receiverThreadType = dataInfo.receiverThreadType_;
					for (ThreadInfo& threadInfo : threadInfos_) {
						if (!threadInfo.outQueue_.load()) { continue; }
						if (threadInfo.receiver_ == receiverThreadType) {
							threadInfo.outQueue_.load()->Push(dataInfo);
						}
					}
				}
			}
		}

		void AddData(ThreadType senderThreadType, ThreadType receiverThreadType, Type&& data) {

			OS::AssertMessage(senderThreadType != ThreadType::Undefined, "");
			OS::AssertMessage(receiverThreadType != ThreadType::Undefined, "");
			OS::AssertMessage(
				senderThreadType != receiverThreadType,
				"Attempt to use different names for one thread.");

			ThreadInfo& senderThreadInfo = GetThreadInfo();
			senderThreadInfo.sender_ = senderThreadType;
			senderThreadInfo.receiver_ = senderThreadType;
			if (!senderThreadInfo.inQueue_.load()) { AddInQueue(); };
			if (!senderThreadInfo.outQueue_.load()) { AddOutQueue(); };

			DataInfo dataInfo{
				senderThreadType,
				receiverThreadType,
				std::move(data)
			};

			senderThreadInfo.inQueue_.load()->Push(dataInfo);
			OS::LogInfo("Debug/MTExch", "Added data");
		}

		std::optional<DataInfo> GetData(std::function<bool(const DataInfo& dataInfo)> filter) {
			ThreadInfo& threadInfo = GetThreadInfo();
			std::lock_guard guard{ threadInfo.mutex_ };
			for(Common::Index i = 0; i < threadInfo.cachedOutData_.GetSize(); i++) {
				const DataInfo& dataInfo = threadInfo.cachedOutData_[i];
				const bool isFound = filter(dataInfo);
				if(isFound){
					DataInfo foundDataInfo = dataInfo;
					threadInfo.cachedOutData_.Erase(i);
					return foundDataInfo;
				}
			}

			if (!threadInfo.inQueue_.load()) { AddInQueue(); };
			if (!threadInfo.outQueue_.load()) { AddOutQueue(); };

			//if (threadInfo.outQueue_.load() == nullptr) { return {}; }
			//If in cache not found:
			DataInfo maybeDataInfo;
			while (threadInfo.outQueue_.load()->TryPop(maybeDataInfo)) {
				if(filter(maybeDataInfo)) {
					return maybeDataInfo;
				} else {
					threadInfo.cachedOutData_.PushBack(maybeDataInfo);
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
			DataQueuePtr queue = receiverThreadInfo.outQueue_;

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
