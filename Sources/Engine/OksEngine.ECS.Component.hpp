#pragma once 

#include <ECS.Component.hpp>
#include <OksEngine.Context.hpp>

namespace OksEngine {

	template<class ComponentType>
	class ECSComponent : public ECS::IComponent<ComponentType> {
	public:

		ECSComponent(Context* context) : 
			context_{ context } {

		}

		[[nodiscard]]
		auto& GetContext() noexcept {
			return *context_;
		}
	private:
		Context* context_;
	};

	template<class ComponentType>
	void Edit() { OS::NotImplemented();  }

}