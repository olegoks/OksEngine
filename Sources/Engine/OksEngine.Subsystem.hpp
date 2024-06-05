#pragma once

#include <memory>

#include <OksEngine.Context.hpp>

namespace OksEngine {

	class Subsystem {
	public:

		enum class Type : Common::UInt64 {
			Resource = 100,
			UI,
			Render = 200,
			Undefined
		};

		static const char* TypeToString(Type type) {
			switch (type) {
			case Type::Resource:
				{
				return "Resource";
				}
			case Type::Render:
			{
				return "Render";
			}
			case Type::Undefined:
			{
				return "Undefined";
			}
			};
		}

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