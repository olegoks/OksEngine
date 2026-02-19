#pragma once
#include <Resources\auto_OksEngine.ResourceSystem.hpp>
#include <Resources/OksEngine.ResourceSystem.Utils.hpp>

#include <Common/Async/auto_OksEngine.Async.hpp>
#include <Common/Async/OksEngine.Async.Utils.hpp>

namespace OksEngine
{
	namespace Resource
	{
		namespace Manager
		{
			void Create::Update(
				ECS2::Entity::Id entity0id,
				const OksEngine::CommandLineParameters* commandLineParameters0,
				const OksEngine::ConfigFilePath* configFilePath0,

				ECS2::Entity::Id entity1id,
				const OksEngine::Config* config1,
				const OksEngine::LuaScript* luaScript1) {

				const ECS2::Entity::Id entityId = CreateEntity();
				CreateComponent<Manager::Tag>(entityId);


				::Lua::Context context;

				::Lua::Script script{ luaScript1->text_ };
				context.LoadScript(script);

				const std::filesystem::path configFileOsPath = configFilePath0->path_;

				auto resourcesRootPath = context.GetGlobalAs<std::string>("ResourceSystem.resourcesRootDirectory");
				auto fullResourcesPath = configFileOsPath.parent_path() / resourcesRootPath;

				auto scriptsRootPath = context.GetGlobalAs<std::string>("ResourceSystem.scriptsRootDirectory");
				auto scriptsFullResourcesPath = configFileOsPath.parent_path() / scriptsRootPath;

				auto ecsFilesRootPath = context.GetGlobalAs<std::string>("ECSCallGraphEditor.ecsFilesRootDirectory");
				auto ecsFilesFullResourcesPath = configFileOsPath.parent_path() / ecsFilesRootPath;

				auto scenesFilesRootPath = context.GetGlobalAs<std::string>("scenesRootDirectory");
				auto scenesFilesFullResourcesPath = configFileOsPath.parent_path() / scenesFilesRootPath;

				CreateComponent<ResourcesPath>(entityId, fullResourcesPath);


				//resourceSystem->system_->SetRoot(Subsystem::Type::Engine,
				//	{ scenesFilesFullResourcesPath,
				//		scriptsFullResourcesPath,
				//		fullResourcesPath,
				//		ecsFilesFullResourcesPath });

			};


			void ProcessLoadResourceRequest::Update(
				ECS2::Entity::Id entity0id, const OksEngine::Resource::Manager::Tag* manager__Tag0,
				ECS2::Entity::Id entity1id, const OksEngine::Resource::Manager::Request::Tag* request__Tag1,
				const OksEngine::Resource::Manager::Request::LoadResource* request__LoadResource1) {

				const ECS2::ComponentsFilter requestCF = GetComponentsFilter(entity1id);

				ASSERT_FMSG(requestCF.IsSetOnlyOneOf<RESOURCE__MANAGER__REQUEST__STATES>(),
					"Request must contain one of state components.");

				if (requestCF.IsSet<Request::State::Pending>()) {
					RemoveComponent<Request::State::Pending>(entity1id);
					CreateComponent<Request::State::InProgress>(entity1id);
					std::filesystem::path resourcePath = request__LoadResource1->resourcePath_;
					const ECS2::Entity::Id taskEntityId = COMMON__ASYNC__CREATE_TASK([resourcePath]() {

						auto file = std::make_unique<OS::BinaryFile>(resourcePath);
						file->Open();
						file->Load();
						const Common::Byte* data = file->GetData();
						Common::Size size = file->GetSize();

						return std::any(std::vector<Common::Byte>(data, data + size));
						});
					CreateComponent<Async::Manager::Task::EntityId>(entity1id, taskEntityId);

				}
				else if (requestCF.IsSet<Request::State::InProgress>()) {
					ASSERT_FMSG(requestCF.IsSet<Async::Manager::Task::EntityId>(),
						"If request in progress it must contain async task id.");

					const ECS2::Entity::Id asyncTaskEntityId = GetComponent<Async::Manager::Task::EntityId>(entity1id)->id_;

					const ECS2::ComponentsFilter asyncTaskCF = GetComponentsFilter(asyncTaskEntityId);

					if (asyncTaskCF.IsSet<Async::Manager::Task::State::Finished>()) {
						auto* taskInfo = GetComponent<Async::Manager::Task::Info>(asyncTaskEntityId);
						std::any result = taskInfo->future_._Get_value();
						std::vector<Common::Byte> loadedData = std::any_cast<std::vector<Common::Byte>>(result);
						
						const ECS2::Entity::Id resourceEntityId = RESOURCE__CREATE_RESOURCE(
							request__LoadResource1->resourcePath_,
							loadedData);
						CreateComponent<Resource::EntityId>(entity1id, resourceEntityId);

						//Change state to Finished
						RemoveComponent<Request::State::InProgress>(entity1id);
						CreateComponent<Request::State::Finished>(entity1id);

					}


				}

			}

		}

	} // namespace Resource

} // namespace OksEngine