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
			Physics,
			Engine,
			Log,
			Debug,
			ChildThread,
			Undefined
		};


		Subsystem(Type type, Context& context) noexcept :
			context_{ context } { }

		virtual void Update() = 0;

		virtual ~Subsystem() = default;

		[[nodiscard]]
		Context& GetContext() noexcept { return context_; }

	private:
		Type type_;
		Context& context_;
	};

}