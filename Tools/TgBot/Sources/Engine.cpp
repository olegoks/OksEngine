
#include <auto_ECSGenerator3.RunSystems.hpp>
//
//#include <pix3.h>

#include <Engine.hpp>

//
//#include <Common/CommandLineParameters/auto_OksEngine.CommandLineParameters.hpp>
//#include <ECS/File/auto_OksEngine.ECS.File.hpp>
//#include <Test/auto_Test.Module.hpp>

#include <PacketManager/auto_PacketManager.hpp>

Engine::Engine(const CreateInfo& createInfo) noexcept {

	
	world2_ = std::make_shared<ECS2::World>();

	{
		const int argc = createInfo.argc_;
		char** argv = createInfo.argv_;
		
		OS::InitializeLogger(argc, argv);

		const ECS2::Entity::Id commandLineParametersEntity = world2_->CreateEntity();

		world2_->CreateComponent<CommandLineParameters>(commandLineParametersEntity);
		world2_->CreateComponent<ExecutablePath>(commandLineParametersEntity, argv[0]);

		for (int i = 1; i < argc; i++) {

			const char* value = argv[i];
			using namespace std::string_literals;
			if (value == "-cfg"s) {
				world2_->CreateComponent<ConfigFilePath>(commandLineParametersEntity, argv[i + 1]);
			}
			if (value == "-scn"s) {
				world2_->CreateComponent<SceneParameter>(commandLineParametersEntity, argv[i + 1]);
			}
			if (value == "-ecs_project_file"s) {
				world2_->CreateComponent<PacketManager::ECSProjectFilePath>(commandLineParametersEntity, argv[i + 1]);
			}
		}
		world2_->ApplyDelayedRequests();

	}

}

void Engine::Run() noexcept {

	HRESULT r;
	r = SetThreadDescription(GetCurrentThread(), L"Main thread");

	RunInitializeSystems(world2_);

	//while (IsRunning()) {
	//	BEGIN_PROFILE("Frame");
	//	RunSystems(world2_);
	//	BEGIN_PROFILE("Apply delayed requests");
	//	world2_->ApplyDelayedRequests();
	//	END_PROFILE();
	//	std::cout << "newFrame" << std::endl;
	//	END_PROFILE();
	//}

}

