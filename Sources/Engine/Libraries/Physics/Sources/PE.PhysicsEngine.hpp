#pragma once 

#include <PAL.hpp>

namespace PE {

	class PhysicsEngine {
	public:

		PhysicsEngine() {

			std::shared_ptr<PAL::API> api = PAL::CreateAPI();
			api_ = api;
			PAL::World::CreateInfo worldCreateInfo {
				.gravity_ = { 0.f, -9.81f, 0.f }
			};
			

		}
		
		[[nodiscard]]
		auto CreateWorld(const PAL::World::CreateInfo& createInfo) {
			return world_ = api_->CreateWorld(createInfo);
		}

		[[nodiscard]]
		auto GetWorld() {
			return world_;
		}

		[[nodiscard]]
		auto GetAPI() noexcept {
			return api_;
		}

		//[[nodiscard]]
		//auto CreateRigidBody(const PAL::DynamicRigidBody::CreateInfo& createInfo) {
		//	return api_->CreateRigidBody(createInfo);
		//}

		//[[nodiscard]]
		//auto CreateStaticRigidBody(const PAL::StaticRigidBody::CreateInfo& createInfo) {
		//	return api_->CreateStaticRigidBody(createInfo);
		//}

		//[[nodiscard]]
		//auto CreateShape(const PAL::Shape::CreateInfoBox& createInfo) {
		//	return api_->CreateShape(createInfo);
		//}

		//[[nodiscard]]
		//auto CreateShape(const PAL::Shape::CreateInfoCapsule& createInfo) {
		//	return api_->CreateShape(createInfo);
		//}

		//[[nodiscard]]
		//auto CreateShape(const PAL::Shape::CreateInfoMesh& createInfo) {
		//	return api_->CreateShape(createInfo);
		//}


		//void Simulate(float elapsedTime) {
		//	GetWorld()->Simulate(elapsedTime);
		//	//OS::LogInfo("physics", { "Simulation time {}", elapsedTime });
		//}


	private:
		std::shared_ptr<PAL::API> api_ = nullptr;
		std::shared_ptr<PAL::World> world_ = nullptr;
	};
}