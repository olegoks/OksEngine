#include <thread>
#include <mutex>
#include <shared_mutex>

#include <OS.Assert.hpp>

namespace OS {

	class MTCheck {
	public:

		MTCheck() {
			std::lock_guard guard{ mutex };
			auto this_id = std::this_thread::get_id();
#pragma region Assert
				ASSERT_FMSG(this_id == ownerThread, "");
#pragma endregion
			ownerThread = this_id;
		}

		~MTCheck() {

		}

	private:
		std::mutex mutex;
		std::thread::id ownerThread;
	};

}