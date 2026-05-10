
#include <auto_OksEngine.RunSystems.hpp>

//
//#include <pix3.h>

#include <Engine.hpp>

#include <Common/CommandLineParameters/auto_OksEngine.CommandLineParameters.hpp>
#include <ECS/File/auto_OksEngine.ECS.File.hpp>

namespace OksEngine {

	Engine::Engine(const CreateInfo& createInfo) noexcept {

		OS::InitializeLogger(createInfo.argc_, createInfo.argv_);
		world2_ = std::make_shared<ECS2::World>();

		{
			const int argc = createInfo.argc_;
			char** argv = createInfo.argv_;

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
					world2_->CreateComponent<ECS::ProjectFilePath>(commandLineParametersEntity, argv[i + 1]);
				}
			}

		}

	}

	void Engine::Update() noexcept {

		HRESULT r;
		r = SetThreadDescription(GetCurrentThread(), L"Main thread");

		//CreateThreads(world2_);

		RunInitializeSystems(world2_);

		/*while (IsRunning()) {

			RunSystems(world2_);

		}*/

	}


}