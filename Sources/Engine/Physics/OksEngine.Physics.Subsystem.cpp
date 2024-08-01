
#include <Physics/OksEngine.Physics.Subsystem.hpp>
#include <PAL.hpp>
#include <Physics/OksEngine.Physics.System.hpp>
#include <Resources/OksEngine.Resource.Subsystem.hpp>

#include "Geometry.Model.hpp"

namespace OksEngine {


	PhysicsSubsystem::PhysicsSubsystem(const CreateInfo& createInfo) :
		Subsystem{
			Subsystem::Type::Physics,
			createInfo.context_ },
			physicsEngine_{ nullptr } {

		auto& context = GetContext();
		auto ecsWorld = context.GetECSWorld();
		ecsWorld->RegisterSystem<PhysicsSystem>(context);

		physicsEngine_ = std::make_shared<PE::PhysicsEngine>();
		PAL::World::CreateInfo worldCreateInfo{
			.gravity_ = { 0.0, -9.81, 0.0 }
		};
		physicsEngine_->CreateWorld(worldCreateInfo);
	}

	Common::Index PhysicsSubsystem::CreateRigidBody(const PAL::RigidBody::CreateInfo& createInfo) {
		auto rigidBody = physicsEngine_->CreateRigidBody(createInfo);
		rigidBodies_.push_back(rigidBody);
		return rigidBodies_.size() - 1;
	}

	Common::Index PhysicsSubsystem::CreateStaticRigidBody(const PAL::StaticRigidBody::CreateInfo& createInfo) {
		auto rigidBody = physicsEngine_->CreateStaticRigidBody(createInfo);
		staticRigidBodies_.push_back(rigidBody);
		return staticRigidBodies_.size() - 1;
	}

	std::shared_ptr<PAL::Shape> PhysicsSubsystem::CreateShape(const PAL::Shape::CreateInfoBox& createInfo) {
		auto shape = physicsEngine_->CreateShape(createInfo);
		return shape;
	}
	std::shared_ptr<PAL::Shape> PhysicsSubsystem::CreateShape(const PAL::Shape::CreateInfoCapsule& createInfo) {
		auto shape = physicsEngine_->CreateShape(createInfo);
		return shape;
	}

	std::shared_ptr<PAL::Shape> PhysicsSubsystem::CreateShape(const PAL::Shape::CreateInfoMesh& createInfo) {
		auto shape = physicsEngine_->CreateShape(createInfo);
		return shape;
	}

	void PhysicsSubsystem::AddRigidBodyToWorld(Common::Index rbIndex) {
		physicsEngine_->GetWorld()->AddRigidBody(rigidBodies_[rbIndex]);
	}

	void PhysicsSubsystem::AddStaticRigidBodyToWorld(Common::Index rbIndex) {
		physicsEngine_->GetWorld()->AddStaticRigidBody(staticRigidBodies_[rbIndex]);
	}

	void PhysicsSubsystem::ApplyForce(Common::Index rbIndex, const glm::vec3& direction, float force) {
		rigidBodies_[rbIndex]->ApplyForce(direction, force);
	}

	void PhysicsSubsystem::SetVelocity(Common::Index rbIndex, const glm::vec3& direction, float velocity) {
		rigidBodies_[rbIndex]->SetVelocity(direction, velocity);
	}

	[[nodiscard]]
	std::shared_ptr<Geom::Model<Geom::Vertex3f>> PhysicsSubsystem::GetGeom(const std::string& geomName) {

		auto& context = GetContext();
		auto resourceSubsystem = context.GetResourceSubsystem();


		const auto blockModelObjTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/" + geomName + ".obj");
		ResourceSubsystem::Resource modelCubeObjResource = resourceSubsystem->GetResource(Subsystem::Type::Physics, blockModelObjTaskId);

		std::string obj{ modelCubeObjResource.GetData<char>(), modelCubeObjResource.GetSize() };
		auto geom = std::make_shared<Geom::Model<Geom::Vertex3f, Geom::Index16>>(Geometry::ParseObjVertex3fIndex16(obj));
		return geom;
	}

	void PhysicsSubsystem::Update() noexcept {
		using namespace std::chrono_literals;
		static std::chrono::high_resolution_clock::time_point previousUpdate = std::chrono::high_resolution_clock::now();
		static std::chrono::high_resolution_clock::duration remainder = 0ms;
		const auto simulationGranularity = 10ms;
		const auto now = std::chrono::high_resolution_clock::now();
		const auto delta = (now - previousUpdate);
		auto toSimulate = delta + remainder;

		while (toSimulate >= simulationGranularity) {
			physicsEngine_->Simulate(simulationGranularity.count() / 1000.f);
			toSimulate -= simulationGranularity;
		}
		remainder = toSimulate;
		previousUpdate = std::chrono::high_resolution_clock::now();


	}


}
