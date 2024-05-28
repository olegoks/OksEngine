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
			Undefined
		};

		Subsystem(Type type, Context& context) noexcept :
			context_{ context } { }

		virtual void Update() = 0;

		virtual ~Subsystem() = default;

		std::shared_ptr<AsyncResourceSubsystem> GetResourceSubsystem() {
			return context_.resourceSubsystem_;
		}

	private:
		Type type_;
		Context& context_;
	};

}