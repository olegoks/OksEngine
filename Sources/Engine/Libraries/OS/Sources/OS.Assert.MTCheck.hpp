
#pragma once

#include <functional>
#include <atomic>
#include <OS.Assert.hpp>

namespace OS {

	class MTCheck {
	public:

		using Callback = std::function<void()>;

		MTCheck(Callback&& callback) : 
			callback_{std::move(callback)},
			isCaptured_{ false } {

		}

		void Capture() {
			bool expected = false;
			if (!isCaptured_.compare_exchange_strong(expected, true,
				std::memory_order_acquire)) {

				if (callback_) {
					callback_();
				}					
			}
		}

		void Release() {
			isCaptured_.store(false, std::memory_order_release);
		}

		MTCheck& operator=(MTCheck&&) = delete;
		MTCheck& operator=(const MTCheck&) = delete;

	private:
		Callback callback_;
		std::atomic_bool isCaptured_;
	};


	
}


#define MT_ASSERT()\
	static OS::MTCheck mtCheck{[](){\
		ASSERT_FAIL_MSG("Threads intersection.");\
		} };