#pragma once

#include <filesystem>

#include <ECS.hpp>
#include <Common/OksEngine.Position.hpp>
#include <OksEngine.Context.hpp>
#include <OksEngine.ECS.System.hpp>
#include <OksEngine.ECS.Component.hpp>

#include <Render/OksEngine.ImmutableRenderGeometry.hpp>
#include <Physics/OksEngine.DynamicRigidBody.hpp>
#include <Physics/OksEngine.DynamicRigidBodyBox.hpp>
#include <Physics/OksEngine.DynamicRigidBodyCapsule.hpp>
#include <Render/OksEngine.Camera.hpp>
#include <Resources/OksEngine.Resource.Subsystem.hpp>
#include <UI/OksEngine.UI.Components.hpp>

namespace OksEngine {

	class LuaEntity {
	public:

		Position* GetPosition() {
			auto position = world_->GetComponent<Position>(id_);
			return position;
		}

		ImmutableRenderGeometry* GetImmutableRenderGeometry() {
			auto position = world_->GetComponent<ImmutableRenderGeometry>(id_);
			return position;
		}

		//DynamicRigidBodyBox* GetRigidBodyBox() {
		//	auto rigidBody = world_->GetComponent<DynamicRigidBodyBox>(id_);
		//	return rigidBody;
		//}

		//DynamicRigidBodyCapsule* GetRigidBodyCapsule() {
		//	auto rigidBody = world_->GetComponent<DynamicRigidBodyCapsule>(id_);
		//	return rigidBody;
		//}

		Camera* GetCamera() {
			auto camera = world_->GetComponent<Camera>(id_);
			return camera;
		}

		void SetWorld(ECS::World* world) {
			world_ = world;
		}

		void SetId(std::uint64_t id) { id_ = id; }

	private:
		ECS::World* world_ = nullptr;
		ECS::Entity::Id id_ = 0;
	};

	struct Behaviour : public ECSComponent<Behaviour> {
		Behaviour(const std::string& scriptName, const std::string& objectName) :
			ECSComponent{ nullptr },
			scriptName_{ scriptName },
			objectName_{ objectName } {}
		std::string scriptName_;
		std::string objectName_;
	};

	template<>
	void Edit<Behaviour>(Behaviour* behaviour);

	template<>
	void Add<Behaviour>(ECS::World* world, ECS::Entity::Id id);


	struct LuaScript : public ECSComponent<LuaScript> {
		std::string tag_;
		Lua::Script::Id id_;

		LuaScript(const std::string& tag, Lua::Script::Id scriptId) :
			ECSComponent{ nullptr },
			tag_{ tag },
			id_{ scriptId } {

		}

	};


	template<>
	void Edit<LuaScript>(LuaScript* script);

	template<>
	void Add<LuaScript>(ECS::World* world, ECS::Entity::Id id);


	struct LuaScriptLoadRequest : public ECSComponent<LuaScriptLoadRequest> {
		ECS::Entity::Id resourceEntityId_ = ECS::Entity::Id::invalid_;

		LuaScriptLoadRequest() :
			ECSComponent{ nullptr },
			resourceEntityId_{ ECS::Entity::Id::invalid_ } {

		}

		LuaScriptLoadRequest(ECS::Entity::Id resourceEntityId) :
			ECSComponent{ nullptr },
			resourceEntityId_{ resourceEntityId } {

		}

	};

	class LoadLuaScript : public ECSSystem {
	private:
	public:

		LoadLuaScript(Context& context) noexcept : ECSSystem{ context } {

		}

		virtual void StartUpdate() override { }

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override;

		virtual void EndUpdate() override { }

		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override;

		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<LoadLuaScript>().GetId();
		}

	private:

	};


	struct LuaContext : public ECSComponent<LuaContext> {
		Lua::Context  context_;

		LuaContext(Lua::Context&& context) :
			ECSComponent{ nullptr }, context_{ std::move(context) } { }
	};

	class CreateLuaContext : public ECSSystem {
	private:
	public:

		CreateLuaContext(Context& context) noexcept : ECSSystem{ context } {

		}

		virtual void StartUpdate() override { }

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override;

		virtual void EndUpdate() override { }

		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override {
			return { ECS::Entity::Filter{}.Include<LuaScript>().Exclude<LuaContext>(), ECS::Entity::Filter{}.ExcludeAll() };
		}

		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<CreateLuaContext>().GetId();
		}

	private:

	};



	//struct BehaviourDeprecated : public ECSComponent<BehaviourDeprecated> {
	//	std::string scriptName_;
	//	std::string objectName_;
	//	lua_State* state_ = nullptr;
	//	luabridge::LuaRef updater_;
	//	luabridge::LuaRef inputProcessor_;
	//	luabridge::LuaRef object_;
	//	LuaEntity entity_;
	//	Lua::Context  context_;
	//	std::chrono::high_resolution_clock::time_point previousUpdateTimePoint_;

	//	BehaviourDeprecated() = default;
	//	BehaviourDeprecated(Context* context,
	//		ECS::Entity::Id entityId,
	//		std::string scriptName,
	//		std::string objectName
	//	);

	//	void CallUpdater(Common::Size ms);
	//	void CallInputProcessor(const char* inputKey, const char* inputEvent, double offsetX, double offsetY);
	//};

	//template<>
	//void Edit<BehaviourDeprecated>(BehaviourDeprecated* behaviour);


	//template<>
	//void Add<BehaviourDeprecated>(ECS::World* world, ECS::Entity::Id id) {
	//}

	class BehaviourSystem : public ECSSystem {
	private:
	public:

		BehaviourSystem(Context& context) noexcept : ECSSystem{ context } {

		}

		virtual void StartUpdate() override { }

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override;

		virtual void EndUpdate() override { }

		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override;

		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<BehaviourSystem>().GetId();
		}

	private:
		
	};

	class CallUpdateMethod : public ECSSystem {
	private:
	public:

		CallUpdateMethod(Context& context) noexcept : ECSSystem{ context } {

		}

		virtual void StartUpdate() override { }

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override;

		virtual void EndUpdate() override { }

		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override;

		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<CallUpdateMethod>().GetId();
		}

	private:

	};

	class CallInputProcessor : public ECSSystem {
	private:
	public:

		CallInputProcessor(Context& context) noexcept : ECSSystem{ context } {

		}

		virtual void StartUpdate() override { }

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override;

		virtual void EndUpdate() override { }

		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override;

		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<CallInputProcessor>().GetId();
		}

	private:

	};

}