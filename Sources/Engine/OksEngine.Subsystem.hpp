#pragma once

#include <memory>

#include <OksEngine.Context.hpp>

namespace OksEngine {

	class Subsystem {
	public:

		enum class Type : Common::UInt64 {
			Resource,
			UI,
			Render,
			Engine,
			Log,
			Undefined
		};


		Subsystem(Type type, Context& context) noexcept :
			context_{ context } { }

		virtual void Update() = 0;

		virtual ~Subsystem() = default;

		[[nodiscard]]
		auto GetResourceSubsystem() {
			return context_.resourceSubsystem_;
		}

		[[nodiscard]]
		auto GetECSWorld() {
			return context_.world_;
		}

		[[nodiscard]]
		auto& GetConfig() {
			return context_.config_;
		}

		[[nodiscard]]
		auto GetUISubsystem() {
			return context_.uiSubsystem_;
		}

	private:
		Type type_;
		Context& context_;
	};

}