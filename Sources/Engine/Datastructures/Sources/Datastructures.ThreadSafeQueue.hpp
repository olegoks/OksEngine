#pragma once

#include <queue>
#include <mutex>

namespace Datastructures {

	template<class Type>
	class ThreadSafeQueue {
	public:

		Type& Push(const Type& object) {
			std::lock_guard<std::mutex> lockGuard{mutex_};
			queue_.push(object);
			return queue_.front();
		}

		bool IsEmpty()
		{
			std::lock_guard<std::mutex> lockGuard{ mutex_ };
			return queue_.empty();
		}

		std::optional<Type> Pop() {
			std::lock_guard<std::mutex> lockGuard{ mutex_ };
			if (queue_.empty()) { return {}; }
			Type data = queue_.back();
			queue_.pop();
			return std::optional{ data };
		}

	private:
		std::mutex mutex_;
		std::queue<Type> queue_;
	};

}

namespace DS = Datastructures;
