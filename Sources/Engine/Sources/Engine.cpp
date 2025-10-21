
#include <auto_OksEngine.RunSystems.hpp>

//
//#include <pix3.h>

#include <Engine.hpp>

#include <OksEngine.Context.hpp>

#include <Resources/OksEngine.Resource.Subsystem.hpp>
#include <Physics/OksEngine.Physics.Subsystem.hpp>

#include <OksEngine.ECS.hpp>



namespace OksEngine {

	Engine::Engine(const CreateInfo& createInfo) noexcept {
		Context::CreateInfo contextCreateInfo{
			createInfo.argc_,
			createInfo.argv_
		};
		context_ = std::make_shared<Context>(contextCreateInfo);
	}

	void Engine::Update() noexcept {

		std::shared_ptr<ECS2::World> world2 = context_->GetECS2World();


		HRESULT r;
		r = SetThreadDescription(GetCurrentThread(), L"Main thread");
	
		CreateThreads(world2);

		RunInitializeSystems(world2);

		while (IsRunning()) {

			RunSystems(world2);

		}

	}


}