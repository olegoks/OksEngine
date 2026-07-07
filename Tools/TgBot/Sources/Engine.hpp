#pragma once 

#include <memory>
#include <chrono>
#include <list>
#include <utility>

#include <ECS2.World.hpp>

#include <OksEngine.ECS.Component.Utils.hpp>

namespace OksEngine {

	class Engine {
	public:

		struct CreateInfo {
			int argc_ = 0;
			char** argv_ = nullptr;
		};

		explicit Engine(const CreateInfo& createInfo) noexcept;

		void Run() {
			isRunning_ = true;
			while (IsRunning()) {
				Update();
			}

		}

		inline bool IsRunning() const noexcept {
			return isRunning_;
		}

		void Update() noexcept;

	private:
		bool isRunning_ = false;
		std::shared_ptr<class ECS2::World> world2_ = nullptr;
	};

}
