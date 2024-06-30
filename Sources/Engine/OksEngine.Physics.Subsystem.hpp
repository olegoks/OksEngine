
#include <OksEngine.Subsystem.hpp>
#include <PAL.API.hpp>
#include <PE.PhysicsEngine.hpp>

namespace OksEngine {

	class PhysicsSubsystem : public Subsystem {
	public:


		struct CreateInfo {
			Context& context_;
		};

		PhysicsSubsystem(const CreateInfo& createInfo);

		virtual void Update() noexcept override;


	private:
		std::shared_ptr<PE::PhysicsEngine> physicsEngine_ = nullptr;
	};

}