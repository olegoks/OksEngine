#pragma once 

#include <memory>
#include <chrono>
#include <list>
#include <utility>

#include <OksEngine.Entity.hpp>
#include <Behaviour/OksEngine.Behaviour.hpp>
#include <Common/OksEngine.Position.hpp>
#include <Render/OksEngine.ImmutableRenderGeometry.hpp>

#include <OksEngine.CommandLineParameters.hpp>

namespace OksEngine {

	class Engine {
	public:

		struct CreateInfo {
			CommandLineParameters commandLineParameters_;
		};

		explicit Engine(const CreateInfo& createInfo) noexcept;

		class Scene {
		public:
		private:

		};

		Entity CreateEntity() noexcept;

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
