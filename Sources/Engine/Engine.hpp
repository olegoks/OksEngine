#pragma once 

#include <memory>
#include <chrono>
#include <list>
#include <utility>


#include <ECS.hpp>

namespace OksEngine {

	class Engine {
	public:

		explicit Engine() noexcept;

	private:
		ECS::World world_;
	};

	//class Entity final {
	//public:

	//	Entity(ECS::World* world, ECS::Entity::Id id) noexcept :
	//		world_{ world },
	//		id_{ id } {

	//	}


	//	template<class ...Args>
	//	void AddBehaviour(Args&& ...args) {
	//		world_->CreateComponent<Behaviour>(GetId(), GetWorld(), GetId(), std::forward<Args>(args)...);
	//	}

	//	template<class ...Args>
	//	void AddPosition(Args&& ...args) {
	//		world_->CreateComponent<Position>(GetId(), std::forward<Args>(args)...);
	//	}

	//	template<class ...Args>
	//	void AddRenderableGeometry(Args&& ...args) {
	//		world_->CreateComponent<RenderableGeometry>(GetId(), std::forward<Args>(args)...);
	//	}

	//private:
	//	[[nodiscard]]
	//	ECS::World* GetWorld() noexcept { return world_; }
	//	[[nodiscard]]
	//	ECS::Entity::Id GetId() noexcept { return id_; }
	//private:
	//	ECS::World* world_;
	//	ECS::Entity::Id id_;
	//};

	//class EngineInstance {
	//public:

	//	EngineInstance() {

	//		world_.RegisterSystem<BehaviourSystem>();
	//		world_.RegisterSystem<RenderSystem>();

	//	}

	//	void Run() {
	//		isRunning_ = true;
	//		while (IsRunning()) {
	//			Update();
	//		}

	//	}

	//	bool IsRunning() const {
	//		return isRunning_;
	//	}

	//	Entity CreateEntity() noexcept {
	//		ECS::Entity::Id id = world_.CreateEntity();
	//		return Entity{ &world_, id };
	//	}

	//	template<class System>
	//	void RegisterSystem() {
	//		
	//	}

	//private:

	//	void Update() noexcept {
	//		while (IsRunning()) {
	//			world_.Process();
	//		}
	//	}

	//private:

	//	bool isRunning_ = false;
	//	ECS::World world_;
	//};

}