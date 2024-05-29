#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

namespace Datastructures {

	template<typename T>
	class ThreadSafeQueue {
	private:
		std::mutex mutex_;
		std::queue<T> dataQueue_;
		std::condition_variable conditionVariable_;
	public:

		void Push(T new_value){
			std::lock_guard lockGuard(mutex_);
			dataQueue_.push(new_value);
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
				value = dataQueue_.back();
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

}

namespace DS = Datastructures;
