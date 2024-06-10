#pragma once
#include <string>
#include <thread>
#include <map>
#include <concepts>
#include <atomic>

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

		struct ThreadData {
		
			std::atomic<ThreadName>		name_ = ThreadName::Undefined;
		private:
			DS::TS::Queue<DataInfo>		inDataQueue_; // this thread data -> another thread
			DS::TS::Queue<DataInfo>		outDataQueue_; // this thread <- another thread data
			DS::TS::Vector<DataInfo>	cachedOutDataInfos_;
		public:

			void AddIncomeDataInfo(DataInfo&& incomeDataInfo) {
				inDataQueue_.Push(std::move(incomeDataInfo));
			}

			[[nodiscard]]
			bool TryGetIncomeDataInfo(DataInfo& outcomeDataInfo) {
				return inDataQueue_.TryPop(outcomeDataInfo);
			}

			void AddOutcomeDataInfo(DataInfo& incomeDataInfo) {
				if(name_ == ThreadName::Render) {
					name_ = name_.load();//__debugbreak();
				}
				outDataQueue_.Push(incomeDataInfo);
			}

			[[nodiscard]]
			bool TryGetOutcomeDataInfo(DataInfo& outDataInfo, std::function<bool(const DataInfo& dataInfo)> filter) {
				const bool isFoundInCache = FindDataInfoInOutCache(outDataInfo, filter);
				if (isFoundInCache) {
					return true;
				}
				if (name_ == ThreadName::Render) {
					name_ = name_.load();//__debugbreak();
				}
				DataInfo dataInfo;
				while (outDataQueue_.TryPop(dataInfo)) {
					if (filter(dataInfo)) {
						outDataInfo = std::move(dataInfo);
						return true;
					}
					else {
						AddDataInfoToOutCache(std::move(dataInfo));
					}
				}
				return false;
			}

			void WaitForOutcomeDataInfo(DataInfo& outDataInfo, std::function<bool(const DataInfo& dataInfo)> filter) {
				const bool isGot = TryGetOutcomeDataInfo(outDataInfo, filter);
				if (isGot) {
					return;
				}
				bool isWaited = false;
				DataInfo dataInfo;
				while (!isWaited) {
					//OS::LogInfo("MT", { "Waiting for DataInfo from OutQueue of %d subsystem", magic_enum::enum_name(name_.load())});
					outDataQueue_.WaitAndPop(dataInfo);
					//OS::LogInfo("MT", { "DataInfo was poped from OutQueue of %d subsystem", magic_enum::enum_name(name_.load()) });
					if (filter(dataInfo)) {
						outDataInfo = std::move(dataInfo);
						isWaited = true;
					}
					else {
						AddDataInfoToOutCache(std::move(dataInfo));
					}
				}
			}

		private:

			[[nodiscard]]
			bool FindDataInfoInOutCache(DataInfo& outData, std::function<bool(const DataInfo& dataInfo)> filter) {
				for (Common::Index i = 0; i < cachedOutDataInfos_.GetSize(); i++) {
					DataInfo dataInfo;
					const bool isThereDataInfo = cachedOutDataInfos_.Get(dataInfo, i);
					if (!isThereDataInfo) { continue; }
					const bool isFound = filter(dataInfo);
					if (isFound) {
						outData = std::move(dataInfo);
						cachedOutDataInfos_.Erase(i);
						return true;
					}
				}
				return false;
			}

			void AddDataInfoToOutCache(DataInfo&& dataInfo) {
				cachedOutDataInfos_.PushBack(std::move(dataInfo));
			}
		};

	private:

		using ThreadId = Common::UInt8;
		constexpr static inline Common::Size maxReceiversNumber_ = 8;
		constexpr static inline ThreadId invalidThreadId_ = 0;
		constexpr static inline ThreadId maxThreadId_ = std::numeric_limits<ThreadId>::max();

		[[nodiscard]]
		static ThreadId GetThreadId() noexcept {
			static std::atomic lastThreadId_ = invalidThreadId_;
			static thread_local ThreadId thisThreadId_ = invalidThreadId_;
			if(thisThreadId_ == invalidThreadId_) {
				thisThreadId_ = 1 + lastThreadId_.fetch_add(1, std::memory_order_release);
			}
			return thisThreadId_;
		}

		ThreadData threadInfos_[maxThreadId_ + 1];
		DS::Vector<DataInfo> cachedDataInfo_; // DataInfo that waits receivers.
	public:



		void Update() {
			for (ThreadData& threadInfo : threadInfos_) {
				DataInfo dataInfo;
				const bool isGot = threadInfo.TryGetIncomeDataInfo(dataInfo);
				if (!isGot) { continue; }
				DS::Vector<ThreadName> notFoundReceivers;
				for (Common::Index i = 0; i < dataInfo.receivers_.GetSize(); i++) {
					const ThreadName receiver = dataInfo.receivers_[i];
					bool receiverIsFound = false;
					for (ThreadData& threadInfo : threadInfos_) {
						if (threadInfo.name_ == receiver) {
							threadInfo.AddOutcomeDataInfo(dataInfo);
							//OS::LogInfo("MT", { "DataInfo was moved from inQueue %d to outQueue of %d", dataInfo.sender_, receiver });
							receiverIsFound = true;
							break;
						}
					}
					if (!receiverIsFound) {
						notFoundReceivers.PushBack(receiver);
						continue;
					}
				}
				if (notFoundReceivers.GetSize() > 0) {
					dataInfo.receivers_ = notFoundReceivers;
					cachedDataInfo_.PushBack(std::move(dataInfo));
				}
			}

			DS::Vector<Common::Index> toErase;
			for (Common::Index i = 0; i < cachedDataInfo_.GetSize(); i++) {
				DataInfo& dataInfo = cachedDataInfo_[i];
				auto receivers = dataInfo.receivers_;
				for (Common::Index i = 0; i < dataInfo.receivers_.GetSize(); i++) {
					const ThreadName receiver = dataInfo.receivers_[i];
					bool receiverIsFound = false;
					for (ThreadData& threadInfo : threadInfos_) {
						if (threadInfo.name_ == receiver) {
							threadInfo.AddOutcomeDataInfo(dataInfo);
							receiverIsFound = true;
							break;
						}
					}
					if (!receiverIsFound) {
						continue;
					}
					receivers.Erase(receiver);
				}
				if (receivers.GetSize() == 0) {
					toErase.PushBack(i);
				}
			}
			for (Common::Index i = 0; i < toErase.GetSize(); i++) {
				cachedDataInfo_.Erase(toErase[i]);
			}
		}


		void AddData(ThreadName sender, ThreadName receiver, Type&& data) {
			DS::Vector<ThreadName> receivers;
			receivers.PushBack(receiver);
			AddData(sender, receivers, std::forward<Type>(data));
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
			OS::AssertMessage(
				sender != ThreadName::Undefined,
				"Sender can't be undefined."
			);


			ThreadData& senderThreadData = GetThreadData();

			if (senderThreadData.name_ == ThreadName::Undefined) {
				senderThreadData.name_ = sender;
			}

			DataInfo dataInfo;
			{
				dataInfo.sender_ = sender;
				dataInfo.receivers_ = receivers;
				dataInfo.data_ = std::move(data);
			};

			senderThreadData.AddIncomeDataInfo(std::move(dataInfo));
		}


		[[nodiscard]]
		bool TryGetData(ThreadName receiver, Type& outData, std::function<bool(const DataInfo& dataInfo)> filter) {
			ThreadData& threadData = GetThreadData();
			threadData.name_ = receiver;
			DataInfo dataInfo;
			const bool isGot = threadData.TryGetOutcomeDataInfo(dataInfo, filter);
			if(isGot) {
				outData = std::move(dataInfo.data_);
			}
			return isGot;
		}

		[[nodiscard]]
		Type WaitForData(ThreadName receiver, std::function<bool(const DataInfo& dataInfo)> filter) {
			ThreadData& threadData = GetThreadData();
			DataInfo dataInfo;
			threadData.WaitForOutcomeDataInfo(dataInfo, filter);
			return std::move(dataInfo.data_);
		}

		//void AddCachedDataInfo(DataInfo&& dataInfo) {
		//	ThreadData& threadInfo = GetThreadData();
		//	threadInfo.cachedOutDataInfos_.PushBack(std::move(dataInfo));
		//}

		//[[nodiscard]]
		//DataInfo WaitForData(ThreadName receiver, std::function<bool(const DataInfo& dataInfo)> filter) {
		//	Type data;
		//	while (true) {
		//		const bool isGot = GetData(receiver, data, filter);

		//		if (!isGot) { std::this_thread::yield(); }
		//	}

		//}
	private:

		ThreadData& GetThreadData() {
			const ThreadId threadId = GetThreadId();
			return threadInfos_[threadId];

		}
		/*[[nodiscard]]
		bool GetThreadData(ThreadData** threadInfoPtr, ThreadName receiverName) {
			for (ThreadData& threadInfo : threadInfos_) {
				if (threadInfo.name_ == receiverName) {
					*threadInfoPtr = &threadInfo;
					return true;
				}
			}
			return false;
		}*/

	};

}
