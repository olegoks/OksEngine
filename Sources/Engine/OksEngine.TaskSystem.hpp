#pragma once
#include <string>
#include <thread>
#include <map>
#include <concepts>

#include <Common.hpp>
#include <Datastructures.ThreadSafeQueue.hpp>

#include <boost/thread/shared_mutex.hpp>
#include <boost/lockfree/queue.hpp>

namespace OksEngine {

	template<class Type, Common::Enum ThreadName>
	class MTDataExchangeSystem {
	public:
		struct DataInfo {
			ThreadName				sender_;
			DS::Vector<ThreadName>	receivers_;
			Type					data_;
		};
	private:

		using ThreadId = Common::UInt8;
		using DataQueue = DS::ThreadSafeQueue<DataInfo>;//boost::lockfree::queue<Type>;
		using DataQueuePtr = std::shared_ptr<DataQueue>;
		constexpr static inline Common::Size maxReceiversNumber_ = 8;

		class ThreadInfos {
		private:

			ThreadInfos() {
				
			}

			struct ThreadInfo {
			private:
				ThreadName					name_ = ThreadName::Undefined;
				DataQueuePtr				inDataQueue_ = nullptr;
				DataQueuePtr				outDataQueue_ = nullptr;
				DS::Vector<DataInfo>		cachedOutDataInfos_;

				std::lock_guard guard{ mutex_ };
			};

			void SetThreadName() {

			}
		private:

			[[nodiscard]]
			ThreadInfo& GetThreadInfo() {
				const ThreadId threadId = GetThreadId();
				OS::AssertMessage(threadId != invalidThreadId_, "Got invalid thread id.");
				return threadInfos_[threadId];
			}

			[[nodiscard]]
			static ThreadId GetThreadId() {
				static std::atomic<ThreadId> threadId_ = invalidThreadId_;
				static thread_local ThreadId id = invalidThreadId_;
				if (id == invalidThreadId_) {
					OS::AssertMessage(threadId_.load() != maxThreadId_, "");
					id = ++threadId_;
				}
				return id;
			}

		private:
			boost::shared_mutex mutex_;
			ThreadInfo threadInfos_[];
		};


		constexpr static inline ThreadId invalidThreadId_ = 0;
		constexpr static inline ThreadId maxThreadId_ = std::numeric_limits<ThreadId>::max();

		std::once_flag initializeThreadInfosFlag_;
		void InitializeThreadInfo() {
			for (ThreadInfo& threadInfo : threadInfos_) {
				threadInfo.cachedOutDataInfos_.Reserve(16);
				threadInfo.inDataQueue_ = std::make_shared<DataQueue>();
				threadInfo.outDataQueue_ = std::make_shared<DataQueue>();
			}
		}

		
		ThreadInfo threadInfos_[maxThreadId_ + 1];
		DS::Vector<DataInfo> dataCache_;
	public:
		MTDataExchangeSystem() {
			std::call_once(initializeThreadInfosFlag_, &MTDataExchangeSystem::InitializeThreadInfo, this);
		}
	public:

		void Update() {
			for (ThreadInfo& threadInfo : threadInfos_) {
				DataInfo dataInfo;
				const bool isPoped = threadInfo.inDataQueue_.load()->TryPop(dataInfo);
				if (!isPoped) { continue; }
				DS::Vector<ThreadName> notFoundReceivers;
				for (Common::Index i = 0; i < dataInfo.receivers_.GetSize(); i++) {
					const ThreadName receiver = dataInfo.receivers_[i];
					ThreadInfo* receiverThreadInfo = nullptr;
					const bool isFound = GetThreadInfo(&receiverThreadInfo, receiver);
					if (!isFound) {
						notFoundReceivers.PushBack(receiver);
						continue;
					}
					receiverThreadInfo->outDataQueue_.load()->Push(dataInfo);
					OS::LogInfo("MTSystem", { "Update: data moved from one queue to second." });
				}
				if (notFoundReceivers.GetSize() > 0) {
					dataInfo.receivers_ = notFoundReceivers;
					dataCache_.PushBack(std::move(dataInfo));
				}

				for (Common::Index i = 0; i < dataCache_.GetSize(); i++) {
					DataInfo& dataInfo = dataCache_[i];
					auto receivers = dataInfo.receivers_;
					for (Common::Index i = 0; i < dataInfo.receivers_.GetSize(); i++) {
						const ThreadName receiver = dataInfo.receivers_[i];
						ThreadInfo* receiverThreadInfo = nullptr;
						const bool isFound = GetThreadInfo(&receiverThreadInfo, receiver);
						if (!isFound) {
							continue;
						}
						receiverThreadInfo->outDataQueue_.load()->Push(dataInfo);
						OS::LogInfo("MTSystem", { "Update: data moved from one queue to second." });
						[[maybe_unused]] const Common::Size beforeEraseSize = receivers.GetSize();
						receivers.Erase(receiver);
						[[maybe_unused]] const Common::Size afterEraseSize = receivers.GetSize();
						OS::AssertMessage(beforeEraseSize - afterEraseSize == 1, "");
					}
				}
			}
		}


		[[nodiscard]]
		bool GetThreadInfo(ThreadInfo** threadInfoPtr, ThreadName receiverName) {
			for (ThreadInfo& threadInfo : threadInfos_) {
				if (threadInfo.name_ == receiverName) {
					*threadInfoPtr = &threadInfo;
					return true;
				}
			}
			return false;
		}

		void AddData(ThreadName sender, const DS::Vector<ThreadName>& receivers, Type&& data) {
			OS::AssertMessage(
				receivers.GetSize() <= maxReceiversNumber_,
				"Number of receivers is more tham limit value."
			);
			OS::AssertMessage(
				receivers.GetSize() > 0,
				"Receivers are not set."
			);
			DataInfo dataInfo;
			{
				dataInfo.sender_ = sender;
				dataInfo.receivers_ = receivers;
				dataInfo.data_ = std::move(data);
			};
			ThreadInfo& senderThreadInfo = GetThreadInfo();
			if (senderThreadInfo.name_ == ThreadName::Undefined) { senderThreadInfo.name_ = sender; }
			senderThreadInfo.inDataQueue_.load()->Push(std::move(dataInfo));
		}


		void AddData(ThreadName sender, ThreadName receiver, Type&& data) {
			DS::Vector<ThreadName> receivers;
			receivers.PushBack(receiver);
			AddData(sender, receivers, std::forward<Type>(data));
		}

		[[nodiscard]]
		bool GetData(ThreadName receiver, Type& outDataInfo, std::function<bool(const DataInfo& dataInfo)> filter) {
			bool isFound = false;
			{
				DataInfo dataInfo;
				isFound = GetCachedDataInfo(dataInfo, filter);
				if (isFound) {
					outDataInfo = std::move(dataInfo.data_);
					return true;
				}
			}
			{
				ThreadInfo& threadInfo = GetThreadInfo();
				if (threadInfo.name_ == ThreadName::Undefined) { threadInfo.name_ = receiver; }
				DataInfo dataInfo;
				while (GetDataInfo(dataInfo)) {
					if (filter(dataInfo)) {
						outDataInfo = std::move(dataInfo.data_);
						return true;
					}
					else {
						threadInfo.cachedOutDataInfos_.PushBack(std::move(dataInfo));
					}
				}
				return false;
			}
		}

		[[nodiscard]]
		bool GetCachedDataInfo(DataInfo& outDataInfo, std::function<bool(const DataInfo& dataInfo)> filter) {
			ThreadInfo& threadInfo = GetThreadInfo();
			for (Common::Index i = 0; i < threadInfo.cachedOutDataInfos_.GetSize(); i++) {
				DataInfo& dataInfo = threadInfo.cachedOutDataInfos_[i];
				const bool isFound = filter(dataInfo);
				if (isFound) {
					outDataInfo = std::move(dataInfo);
					threadInfo.cachedOutDataInfos_.Erase(i);
					return true;
				}
			}
			return false;
		}

		[[nodiscard]]
		DataInfo WaitForData(ThreadName receiver, std::function<bool(const DataInfo& dataInfo)> filter) {
			Type data;
			while (true) {
				const bool isGot = GetData(receiver, data, filter);

				if (!isGot) { std::this_thread::yield(); }
			}

		}


		[[nodiscard]]
		bool GetDataInfo(DataInfo& outDataInfo) {
			ThreadInfo& receiverThreadInfo = GetThreadInfo();
			DataInfo dataInfo;
			const bool isPoped = receiverThreadInfo.outDataQueue_.load()->TryPop(dataInfo);
			if (isPoped) {
				outDataInfo = std::move(dataInfo);
			}
			return isPoped;
		}

	};

}
