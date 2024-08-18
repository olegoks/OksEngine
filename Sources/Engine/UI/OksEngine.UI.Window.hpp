#pragma once 

#include <OksEngine.ECS.Component.hpp>
#include <OksEngine.ECS.System.hpp>

namespace OksEngine {

	struct Window : public ECSComponent<Window> {
		Window() : ECSComponent{ nullptr } { }
	}; 

	template<>
	inline void Edit<Window>(Window* window) {

	}

	class WindowSystem : public ECSSystem {
	public:

		WindowSystem(Context& context) noexcept :
			ECSSystem{ context } { }

	
		virtual void Update(ECS::World* world, ECS::Entity::Id entityId) override {

			world->CreateComponent<KeyboardInput>(ECS::Entity::Filter{}.Include<HandleKeyboardInputMarker>());


		}

		Common::TypeId GetTypeId() const noexcept {
			return Common::TypeInfo<WindowSystem>().GetId();
		}
		virtual ECS::Entity::Filter GetFilter() const noexcept override {
			return ECS::Entity::Filter{}.Include<Window>();
		}

	};



}