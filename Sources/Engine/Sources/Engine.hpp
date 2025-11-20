#pragma once 

#include <memory>
#include <chrono>
#include <list>
#include <utility>

#include <Common/auto_OksEngine.Position3D.hpp>
#include <Common/auto_OksEngine.Rotation3D.hpp>
#include <Common/auto_OksEngine.Active.hpp>
#include <Render/auto_OksEngine.ImmutableRenderGeometry.hpp>

#include <OS.CommandLineParameters.hpp>

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
		std::shared_ptr<class Context> context_;
	};

}
