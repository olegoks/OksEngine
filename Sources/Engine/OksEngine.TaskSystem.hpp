#pragma once
#include <string>
#include <thread>
#include <map>

#include <Common.hpp>
#include <Datastructures.ThreadSafeQueue.hpp>

namespace OksEngine {

	template<class Type>
	class MTDataExchangeSystem {
	public:

		using Id = Common::UInt64;

		enum class ThreadType {
			IN_,
			OUT_,
			IN_OUT_
		};

		struct DataInfo {
			std::string mark_;
			Type data_;
		};

		void RegisterThread(std::string name, ThreadType type) {
			if (IsThreadRegistered(name, type)) { return; };
			OS::LogWarning("Engine/bad code", "There is a potencial bug!!!!!!!! ");
			std::thread::id threadId = std::this_thread::get_id();
			std::lock_guard lockQuard{ mutex_ };
			switch (type) {
			case ThreadType::IN_: {
				idToQueueIn_[threadId] = Info{ name,std::make_shared<DS::ThreadSafeQueue<DataInfo>>() };
				break;
			}
			case ThreadType::OUT_: {
				idToQueueOut_[threadId] = Info{ name, std::make_shared<DS::ThreadSafeQueue<DataInfo>>() };
				break;
			}
			case ThreadType::IN_OUT_: {
				idToQueueIn_[threadId] = Info{ name, std::make_shared<DS::ThreadSafeQueue<DataInfo>>() };
				idToQueueOut_[threadId] = Info{ name, std::make_shared<DS::ThreadSafeQueue<DataInfo>>() };
				break;
			}
			}

		}

		bool IsThreadRegistered(std::string name, ThreadType type) {
			std::lock_guard lockQuard{ mutex_ };
			std::thread::id threadId = std::this_thread::get_id();
			switch (type) {
			case ThreadType::IN_: {
				return idToQueueIn_.count(threadId);
				break;
			}
			case ThreadType::OUT_: {
				return idToQueueOut_.count(threadId);
				break;
			}
			case ThreadType::IN_OUT_: {
				return idToQueueIn_.count(threadId) || idToQueueOut_.count(threadId);
				break;
			}
			}
			return false;
		}


		void Update() {
			std::lock_guard lockQuard{ mutex_ };
			for (auto dataInfo : idToQueueIn_) {
				while (!dataInfo.second.queue_->IsEmpty()) {
					auto data = dataInfo.second.queue_->Pop();
					std::string mark = data.value().mark_;
					for (auto outEntry : idToQueueOut_) {
						if (outEntry.second.name_ == data.value().mark_) {
							outEntry.second.queue_->Push(data.value());
						}
					}
				}
			}
		}

		void AddData(std::string senderThreadName, std::string receiverThreadName, const Type& data) {
			RegisterThread(senderThreadName, ThreadType::IN_);
			std::thread::id threadId = std::this_thread::get_id();
			auto maybeInfo = GetInfoIn(threadId);
			if (maybeInfo.has_value()) {
				DataInfo dataInfo{ receiverThreadName, data };
				maybeInfo.value().queue_->Push(dataInfo);
			}
		}

		std::optional<Type> GetData(std::string senderThreadName, std::string receiverThreadName) {
			RegisterThread(receiverThreadName, ThreadType::OUT_);
			std::thread::id threadId = std::this_thread::get_id();
			auto maybeInfo = GetInfoOut(threadId);
			if (maybeInfo.has_value()) {
				if (!maybeInfo.value().queue_->IsEmpty()) {
					auto maybeDataInfo = maybeInfo.value().queue_->Pop();
					if (maybeDataInfo.has_value())
					{
						return maybeDataInfo.value().data_;
					}
				}
			}
			return {};
		}

	private:

		struct Info {
			std::string name_ = "";
			std::shared_ptr<DS::ThreadSafeQueue<DataInfo>> queue_ = nullptr;
		};

		std::optional<Info> GetInfoIn(std::thread::id threadId) {
			std::lock_guard lockQuard{ mutex_ };
			return idToQueueIn_.count(threadId) ? idToQueueIn_[threadId] : (std::optional<Info>{});
		}

		std::optional<Info> GetInfoOut(std::thread::id threadId) {
			std::lock_guard lockQuard{ mutex_ };
			return idToQueueOut_.count(threadId) ? idToQueueOut_[threadId] : (std::optional<Info>{});

		}

	private:

		std::mutex mutex_;
		std::map<std::thread::id, Info> idToQueueIn_;
		std::map<std::thread::id, Info> idToQueueOut_;
	};

	//class TaskSystem {
	//public:

	//	class Task {
	//	public:
	//		std::string name_;
	//		void* info_ = nullptr;
	//	private:

	//	};

	//private:
	//	Datastructures::ThreadSafeQueue<Task> taskQueue_;
	//};


}
