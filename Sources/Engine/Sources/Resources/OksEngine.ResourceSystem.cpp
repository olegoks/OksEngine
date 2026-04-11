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

				CreateComponent<ResourcesRootPath>(entityId, fullResourcesPath);
				CreateComponent<Resources>(entityId);

			};


			void ProcessLoadResourceRequest::Update(
				ECS2::Entity::Id entity0id,
				const OksEngine::Resource::Manager::Tag* manager__Tag0,
				const OksEngine::Resource::Manager::ResourcesRootPath* manager__ResourcesRootPath0,
				const OksEngine::Resource::Manager::Resources* manager__Resources0, 
				
				ECS2::Entity::Id entity1id,
				const OksEngine::Resource::Manager::Request::Tag* request__Tag1,
				const OksEngine::Resource::Manager::Request::LoadResource* request__LoadResource1) {

				const ECS2::ComponentsFilter requestCF = GetComponentsFilter(entity1id);

				ASSERT_FMSG(requestCF.IsSetOnlyOneOf<RESOURCE__MANAGER__REQUEST__STATES>(),
					"Request must contain one of state components.");
				


				if (requestCF.IsSet<Request::State::Pending>()) {
					BEGIN_PROFILE("Process pending state")
					RemoveComponent<Request::State::Pending>(entity1id);
					CreateComponent<Request::State::InProgress>(entity1id);

					ASSERT(Resource::IsResourcePath(request__LoadResource1->resourcePath_));

					auto resourceIt = manager__Resources0->resourcePathToResourceEntityId_.find(request__LoadResource1->resourcePath_);
					const bool isResourceLoaded = (resourceIt != manager__Resources0->resourcePathToResourceEntityId_.end());
					if (!isResourceLoaded) {
						const std::filesystem::path relativePath = GetRelativePath(request__LoadResource1->resourcePath_);
						std::filesystem::path resourcePath = manager__ResourcesRootPath0->path_ / relativePath;

						const ECS2::Entity::Id taskEntityId = COMMON__ASYNC__CREATE_TASK([resourcePath]() {

							BEGIN_PROFILE("Loading resource %s", resourcePath.c_str());
							auto file = std::make_unique<OS::BinaryFile>(resourcePath);
							file->Open();
							file->Load();
							const Common::Byte* data = file->GetData();
							Common::Size size = file->GetSize();
							END_PROFILE();
							return std::any(std::vector<Common::Byte>(data, data + size));
							});
						CreateComponent<Async::Manager::Task::EntityId>(entity1id, taskEntityId);
					}
					else {
						const ECS2::Entity::Id resourceEntityId = resourceIt->second;
						CreateComponent<Resource::EntityId>(entity1id, resourceEntityId);
						RemoveComponent<Request::State::Pending>(entity1id);
						CreateComponent<Request::State::Finished>(entity1id);
					}
					END_PROFILE();
				}
				else if (requestCF.IsSet<Request::State::InProgress>()) {

					BEGIN_PROFILE("Process in progress state")
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

					END_PROFILE();
				}

			}

		}

	} // namespace Resource

} // namespace OksEngine