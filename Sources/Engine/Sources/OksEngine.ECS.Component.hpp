#pragma once 

#include <imgui.h>
#include <ECS.Component.hpp>
#include <OksEngine.Context.hpp>

#include <Lua.Context.hpp>

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

	//template<class ComponentType>
	//class ECS2Component : public ECS2::IComponent<ComponentType> {
	//public:
	//};

	template<class ComponentType>
	void Edit(ComponentType* component) { ImGui::TextDisabled("Edit function is not realized."); }

	template<class ComponentType>
	void Add(ECS::World* world, ECS::Entity::Id id) { ImGui::TextDisabled("Add function is not realized."); };

	template<class ComponentType>
	inline void Bind(Lua::Context& context) { OS::NotImplemented(); }

}