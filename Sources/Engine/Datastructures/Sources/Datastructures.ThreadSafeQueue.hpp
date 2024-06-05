#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

namespace DataStructures::ThreadSafe {
	template<typename T>
	class Queue {
	private:
		std::mutex mutex_;
		std::queue<T> dataQueue_;
		std::condition_variable conditionVariable_;
	public:

		void Push(T new_value){
			std::unique_lock lock(mutex_);
			dataQueue_.push(new_value);
			lock.unlock();
			conditionVariable_.notify_one();
		}

		void WaitAndPop(T& value) {
			std::unique_lock lockGuard(mutex_);
			conditionVariable_.wait(lockGuard, [this] { return !dataQueue_.empty(); });
			value = dataQueue_.front();
			dataQueue_.pop();
		}

		[[nodiscard]]
		bool TryPop(T& value) {
			std::lock_guard lockGuard(mutex_);
			if (!dataQueue_.empty()) { 
				value = dataQueue_.front();
				dataQueue_.pop();
				return true;
			}
			return false;
		}

		//[[nodiscard]]
		//bool Back(std::function<void(const T&)> check, T& value) {
		//	std::lock_guard lockGuard(mutex_);
		//	if (!dataQueue_.empty()) {
		//		value = dataQueue_.back();
		//		const bool 
		//		dataQueue_.pop();
		//		return true;
		//	}
		//}

		[[nodiscard]]
		bool CanPop() {
			std::lock_guard lockGuard(mutex_);
			if (!dataQueue_.empty()) {
				return true;
			}
			return false;
		}
	};
	namespace TS = ThreadSafe;
}

namespace DS = DataStructures;

