
#include <OksEngine.Subsystem.hpp>
#include <PAL.API.hpp>

namespace OksEngine {

	class PhysicsSubsystem : public Subsystem {
	public:


		struct CreateInfo {
			Context& context_;
		};

		PhysicsSubsystem(const CreateInfo& createInfo);

		virtual void Update() noexcept override;


	private:
		std::shared_ptr<PAL::API> api_ = nullptr; 
		std::shared_ptr<PAL::World> world_ = nullptr;
	};

}