#pragma once

#include <memory>


#include <OksEngine.CommandLineParameters.hpp>

#include <ECS.World.hpp>
#include <OksEngine.Storage.hpp>
#include <Geometry.Storage.hpp>
#include <Geometry.Texture.hpp>
#include <Lua.Context.hpp>

namespace OksEngine {

	struct Context final {
	public:

		struct CreateInfo {
			CommandLineParameters commandLineParameters_;
		};

		Context(const CreateInfo& createInfo);

		[[nodiscard]]
		auto GetResourceSubsystem() noexcept {
			return resourceSubsystem_;
		}

		[[nodiscard]]
		auto GetECSWorld() noexcept {
			return world_;
		}

		[[nodiscard]]
		auto GetConfig() noexcept {
			return config_;
		}

		[[nodiscard]]
		auto GetUISubsystem() noexcept {
			return uiSubsystem_;
		}

		[[nodiscard]]
		auto GetRenderSubsystem() noexcept {
			return renderSubsystem_;
		}

		[[nodiscard]]
		auto GetDebugSubsystem() noexcept {
			return debugSubsystem_;
		}


		[[nodiscard]]
		auto GetPhysicsSubsystem() noexcept {
			return physicsSubsystem_;
		}

		[[nodiscard]]
		const auto& GetCommandLineParameters() const noexcept {
			return commandLineParameters_;
		}

	private:
		CommandLineParameters commandLineParameters_;
		std::shared_ptr<class LogSubsystem> logSubsystem_ = nullptr;
		std::shared_ptr<class AsyncResourceSubsystem> resourceSubsystem_ = nullptr;
		std::shared_ptr<class RenderSubsystem> renderSubsystem_ = nullptr;
		std::shared_ptr<class PhysicsSubsystem> physicsSubsystem_ = nullptr;
		std::shared_ptr<class UISubsystem> uiSubsystem_ = nullptr;
		std::shared_ptr<class DebugSubsystem> debugSubsystem_ = nullptr;
		std::shared_ptr<class ECS::World> world_ = nullptr;
		std::shared_ptr<class Config> config_ = nullptr;
	};


}