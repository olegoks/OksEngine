#pragma once
#include <Render\Pipeline\auto_OksEngine.Render.Pipeline.hpp>

#include <Render/Pipeline/OksEngine.Render.Shader.Utils.hpp>

#include <ECS/Scene/auto_OksEngine.Scene.hpp>
#include <ECS/Scene/OksEngine.Scene.Utils.hpp>

#include <Resources/auto_OksEngine.ResourceSystem.hpp>
#include <Resources/OksEngine.ResourceSystem.Utils.hpp>

#include <magic_enum/magic_enum.hpp>


namespace OksEngine
{

	namespace Render
	{
		namespace PipelineDescription
		{

			namespace Manager {
				void Create::Update() {
					const ECS2::Entity::Id managerEntityId = CreateEntity();
					CreateComponent<Manager::Tag>(managerEntityId);
					CreateComponent<Manager::Pipelines>(managerEntityId);
				}

				void EditPipelines(std::shared_ptr<ECS2::World> ecsWorld, Pipelines* pipelines) {

					ImGui::Indent(20.f);

					for (auto& [name, entityId] : pipelines->nameToId_) {
						if (ImGui::CollapsingHeader(name.c_str())) {
							ImGui::Indent(20.f);
							EditEntity(ecsWorld, entityId);
							ImGui::Unindent(20.f);
						}
					}

					if (ImGui::BeginTable("Pipelines", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
						ImGui::TableSetupColumn("Name");
						ImGui::TableSetupColumn("Entity id");
						ImGui::TableHeadersRow();
						for (const auto& [key, value] : pipelines->nameToId_) {
							ImGui::TableNextRow();

							ImGui::TableSetColumnIndex(0);
							ImGui::Text("%s", key.c_str());

							ImGui::TableSetColumnIndex(1);
							ImGui::Text("%d", value.GetRawValue());
						}

						ImGui::EndTable();
					}

					ImGui::Unindent(20.f);


				}
			}
			//namespace VertexShader {
			//	ResourcePath ParseResourcePath(luabridge::LuaRef& vertexShaderRef) {

			//		luabridge::LuaRef resourcePathRef = vertexShaderRef["resourcePath"];
			//		std::string resourcePath = resourcePathRef.cast<std::string>().value();
			//		ASSERT(Resource::IsResourcePath(resourcePath));
			//		return ResourcePath{ std::move(resourcePath) };

			//	};
			//}
			//namespace FragmentShader {
			//	ResourcePath ParseResourcePath(luabridge::LuaRef& fragmentShaderRef) {

			//		luabridge::LuaRef resourcePathRef = fragmentShaderRef["resourcePath"];
			//		std::string resourcePath = resourcePathRef.cast<std::string>().value();
			//		ASSERT(Resource::IsResourcePath(resourcePath));
			//		return ResourcePath{ std::move(resourcePath) };

			//	};
			//}
			//namespace GeometryShader {
			//	ResourcePath ParseResourcePath(luabridge::LuaRef& geometryShaderRef) {

			//		luabridge::LuaRef resourcePathRef = geometryShaderRef["resourcePath"];
			//		std::string resourcePath = resourcePathRef.cast<std::string>().value();
			//		ASSERT(Resource::IsResourcePath(resourcePath));
			//		return ResourcePath{ std::move(resourcePath) };

			//	};
			//}

			//PushConstants ParsePushConstants(luabridge::LuaRef& pushConstantsRef) {

			//	const Stage stage = magic_enum::enum_cast<Stage>(pushConstantsRef["stage"].cast<std::string>().value()).value();
			//	const Common::UInt32 offset = pushConstantsRef["offset"].cast<uint32_t>().value();
			//	const Common::UInt32 size = pushConstantsRef["size"].cast<uint32_t>().value();

			//	return PushConstants{ stage, offset, size };

			//}

			//Bindings ParseBindings(luabridge::LuaRef& bindingsRef) {

			//	luabridge::LuaRef bindingsArrayRef = bindingsRef["bindings"];
			//	std::vector<Binding> bindings;
			//	if (bindingsRef.isTable()) {
			//		for (int i = 0; i <= bindingsArrayRef.length(); ++i) {
			//			luabridge::LuaRef bindingTable = bindingsArrayRef[i];
			//			if (bindingTable.isTable()) {
			//				Binding binding;
			//				{
			//					binding.binding_ = bindingTable["binding"].cast<uint32_t>().value();
			//					binding.resourceType_ = magic_enum::enum_cast<ResourceType>(bindingTable["resourceType"].cast<std::string>().value()).value();
			//					binding.stage_ = magic_enum::enum_cast<Stage>(bindingTable["stage"].cast<std::string>().value()).value();
			//				}
			//				bindings.emplace_back(binding);
			//			}
			//		}
			//	}
			//	return Bindings{ std::move(bindings) };
			//};

			InputAssembler ParseInputAssembler(luabridge::LuaRef& inputAssemblerRef) {

				const VertexType vertexType = magic_enum::enum_cast<VertexType>(inputAssemblerRef["vertexType"].cast<std::string>().value()).value();
				const IndexType indexType = magic_enum::enum_cast<IndexType>(inputAssemblerRef["indexType"].cast<std::string>().value()).value();
				const FrontFace frontFace = magic_enum::enum_cast<FrontFace>(inputAssemblerRef["frontFace"].cast<std::string>().value()).value();
				const CullMode cullMode = magic_enum::enum_cast<CullMode>(inputAssemblerRef["cullMode"].cast<std::string>().value()).value();

				return InputAssembler{ vertexType, indexType, frontFace, cullMode };
			};

			Multisampling ParseMultisampling(luabridge::LuaRef& multisamplingRef) {

				const SamplesCount samplesCount = magic_enum::enum_cast<SamplesCount>(multisamplingRef["samplesCount"].cast<std::string>().value()).value();

				return Multisampling{ samplesCount };
			};

			DepthTest ParseDepthTest(luabridge::LuaRef& depthTestRef) {


				const bool enable = depthTestRef["enable"].cast<bool>().value();
				const DepthBufferCompareOperation compareOperation = magic_enum::enum_cast<DepthBufferCompareOperation>(depthTestRef["compareOperation"].cast<std::string>().value()).value();

				return DepthTest{
					depthTestRef["enable"].cast<bool>().value(),
					compareOperation
				};

			};

			namespace Manager {
				void LoadPipelineDescriptions::Update(
					ECS2::Entity::Id entity0id,
					const OksEngine::Render::PipelineDescription::Manager::Tag* manager__Tag0) {

					std::string pipelines{ "resource:/Scenes/Pipelines/OksEngine.Pipelines.rpl" };
					SCENE__MANAGER__CREATE_LOAD_SCENE_REQUEST(pipelines);

					std::string spipelines{ "resource:/Scenes/Pipelines/OksEngine.SkeletonPipeline.rpl" };
					SCENE__MANAGER__CREATE_LOAD_SCENE_REQUEST(spipelines);

				}
				/*void CreateResourcesLoaded::Update(
					ECS2::Entity::Id entity0id,
					const OksEngine::Render::PipelineDescription::Manager::Tag* manager__Tag0,
					OksEngine::Render::PipelineDescription::Manager::Pipelines* manager__Pipelines0,

					ECS2::Entity::Id entity1id,
					const OksEngine::Render::PipelineDescription::Tag* pipelineDescription__Tag1) {


					const bool allResourcesLoaded = [this](ECS2::Entity::Id entityId) {
						const ECS2::ComponentsFilter cf = GetComponentsFilter(entityId);
						if (cf.IsSet<GeometryShader::ResourcePath>()) {
							if (!cf.IsSet<GeometryShader::ResourceEntityId>()) {
								return false;
							}
						}
						if (cf.IsSet<VertexShader::ResourcePath>()) {
							if (!cf.IsSet<VertexShader::ResourceEntityId>()) {
								return false;
							}
						}
						if (cf.IsSet<FragmentShader::ResourcePath>()) {
							if (!cf.IsSet<FragmentShader::ResourceEntityId>()) {
								return false;
							}
						}
						return true;
						}(entity1id);
					if (allResourcesLoaded) {
						CreateComponent<ResourcesLoaded>(entity1id);
					}

				}*/


			}

			void CreateReadyComponent::Update(
				ECS2::Entity::Id entity0id,
				const OksEngine::Render::PipelineDescription::Manager::Tag* manager__Tag0,
				OksEngine::Render::PipelineDescription::Manager::Pipelines* manager__Pipelines0,
				ECS2::Entity::Id entity1id,
				const OksEngine::Render::PipelineDescription::Tag* pipelineDescription__Tag1,
				const OksEngine::Name* name1) {

				const auto allShadersReady = [this](ECS2::Entity::Id pipelineDescriptionEntityId) {
					const ECS2::ComponentsFilter cf = GetComponentsFilter(pipelineDescriptionEntityId);
					if (cf.IsSet<GeometryShader::EntityId>()) {
						const ECS2::Entity::Id shaderEntityId = GetComponent<GeometryShader::EntityId>(pipelineDescriptionEntityId)->id_;
						if (!RENDER__SHADER__IS_SHADER_READY(shaderEntityId)) {
							return false;
						}
					}
					if (cf.IsSet<VertexShader::EntityId>()) {
						const ECS2::Entity::Id shaderEntityId = GetComponent<VertexShader::EntityId>(pipelineDescriptionEntityId)->id_;
						if (!RENDER__SHADER__IS_SHADER_READY(shaderEntityId)) {
							return false;
						}
					}
					if (cf.IsSet<FragmentShader::EntityId>()) {
						const ECS2::Entity::Id shaderEntityId = GetComponent<FragmentShader::EntityId>(pipelineDescriptionEntityId)->id_;
						if (!RENDER__SHADER__IS_SHADER_READY(shaderEntityId)) {
							return false;
						}
					}
					return true;
					};

				if (allShadersReady(entity1id)) {
					manager__Pipelines0->nameToId_[name1->value_] = entity1id;
					CreateComponent<PipelineDescription::Ready>(entity1id);
				}

			}

			//void CreateReadyComponent::Update(
			//	ECS2::Entity::Id entity0id, const OksEngine::Render::PipelineDescription::Manager::Tag* manager__Tag0,
			//	OksEngine::Render::PipelineDescription::Manager::Pipelines* manager__Pipelines0,
			//	ECS2::Entity::Id entity1id,
			//	const OksEngine::Render::PipelineDescription::Tag* pipelineDescription__Tag1,
			//	const OksEngine::Name* name1) {

			//	if (IsComponentExist<ResourcesLoaded>(entity1id)) {
			//		manager__Pipelines0->nameToId_[name1->value_] = entity1id;
			//		CreateComponent<Ready>(entity1id);
			//	}

			//}

			//namespace GeometryShader
			//{
			//	void CreateResourceRequest::Update(
			//		ECS2::Entity::Id entity0id,
			//		const OksEngine::Render::PipelineDescription::Manager::Tag* manager__Tag0,
			//		const OksEngine::Render::PipelineDescription::Manager::Pipelines* manager__Pipelines0,
			//		ECS2::Entity::Id entity1id,
			//		const OksEngine::Render::PipelineDescription::Tag* render__PipelineDescription__Tag1,
			//		const OksEngine::Render::PipelineDescription::GeometryShader::ResourcePath
			//		* pipelineDescription__GeometryShader__ResourcePath1) {


			//		const ECS2::Entity::Id requestEntityId = RESOURCE__MANAGER__CREATE_LOAD_RESOURCE_REQUEST(
			//			pipelineDescription__GeometryShader__ResourcePath1->resourcePath_);

			//		CreateComponent<GeometryShader::ResourceRequestEntityId>(entity1id, requestEntityId);
			//	}


			//	void SaveResourceEntity::Update(
			//		ECS2::Entity::Id entity0id,
			//		const OksEngine::Render::PipelineDescription::Manager::Tag* manager__Tag0,
			//		OksEngine::Render::PipelineDescription::Manager::Pipelines* manager__Pipelines0,

			//		ECS2::Entity::Id entity1id,
			//		const OksEngine::Render::PipelineDescription::Tag* render__PipelineDescription__Tag1,
			//		const OksEngine::Render::PipelineDescription::GeometryShader::ResourcePath* geometryShader__ResourcePath1,
			//		const OksEngine::Render::PipelineDescription::GeometryShader::ResourceRequestEntityId
			//		* geometryShader__ResourceRequestEntityId1) {

			//		if (RESOURCE__MANAGER__IS_REQUEST_FINISHED(geometryShader__ResourceRequestEntityId1->id_)) {

			//			const ECS2::Entity::Id resourceEntityId = GetComponent<Resource::EntityId>(
			//				geometryShader__ResourceRequestEntityId1->id_)->id_;
			//			CreateComponent<GeometryShader::ResourceEntityId>(entity1id, resourceEntityId);
			//			RemoveComponent<GeometryShader::ResourceRequestEntityId>(entity1id);

			//		}

			//	}

			//}

			//namespace VertexShader
			//{
			//	void CreateResourceRequest::Update(
			//		ECS2::Entity::Id entity0id,
			//		const OksEngine::Render::PipelineDescription::Manager::Tag* manager__Tag0,
			//		const OksEngine::Render::PipelineDescription::Manager::Pipelines* manager__Pipelines0,
			//		ECS2::Entity::Id entity1id,
			//		const OksEngine::Render::PipelineDescription::Tag* render__PipelineDescription__Tag1,
			//		const OksEngine::Render::PipelineDescription::VertexShader::ResourcePath
			//		* pipelineDescription__VertexShader__ResourcePath1) {


			//		const ECS2::Entity::Id requestEntityId = RESOURCE__MANAGER__CREATE_LOAD_RESOURCE_REQUEST(
			//			pipelineDescription__VertexShader__ResourcePath1->resourcePath_);

			//		CreateComponent<VertexShader::ResourceRequestEntityId>(entity1id, requestEntityId);
			//	}

			//	void SaveResourceEntity::Update(
			//		ECS2::Entity::Id entity0id,
			//		const OksEngine::Render::PipelineDescription::Manager::Tag* manager__Tag0,
			//		OksEngine::Render::PipelineDescription::Manager::Pipelines* manager__Pipelines0,

			//		ECS2::Entity::Id entity1id,
			//		const OksEngine::Render::PipelineDescription::Tag* render__PipelineDescription__Tag1,
			//		const OksEngine::Render::PipelineDescription::VertexShader::ResourcePath* vertexShader__ResourcePath1,
			//		const OksEngine::Render::PipelineDescription::VertexShader::ResourceRequestEntityId* vertexShader__ResourceRequestEntityId1) {

			//		if (RESOURCE__MANAGER__IS_REQUEST_FINISHED(vertexShader__ResourceRequestEntityId1->id_)) {

			//			const ECS2::Entity::Id resourceEntityId = GetComponent<Resource::EntityId>(
			//				vertexShader__ResourceRequestEntityId1->id_)->id_;
			//			CreateComponent<VertexShader::ResourceEntityId>(entity1id, resourceEntityId);
			//			RemoveComponent<VertexShader::ResourceRequestEntityId>(entity1id);

			//		}

			//	}

			//}


			//namespace FragmentShader
			//{
			//	void CreateResourceRequest::Update(
			//		ECS2::Entity::Id entity0id,
			//		const OksEngine::Render::PipelineDescription::Manager::Tag* manager__Tag0,
			//		const OksEngine::Render::PipelineDescription::Manager::Pipelines* manager__Pipelines0,
			//		ECS2::Entity::Id entity1id,
			//		const OksEngine::Render::PipelineDescription::Tag* render__PipelineDescription__Tag1,
			//		const OksEngine::Render::PipelineDescription::FragmentShader::ResourcePath
			//		* pipelineDescription__FragmentShader__ResourcePath1) {


			//		const ECS2::Entity::Id requestEntityId = RESOURCE__MANAGER__CREATE_LOAD_RESOURCE_REQUEST(
			//			pipelineDescription__FragmentShader__ResourcePath1->resourcePath_);

			//		CreateComponent<FragmentShader::ResourceRequestEntityId>(entity1id, requestEntityId);
			//	}

			//	void SaveResourceEntity::Update(
			//		ECS2::Entity::Id entity0id,
			//		const OksEngine::Render::PipelineDescription::Manager::Tag* manager__Tag0,
			//		OksEngine::Render::PipelineDescription::Manager::Pipelines* manager__Pipelines0,

			//		ECS2::Entity::Id entity1id,
			//		const OksEngine::Render::PipelineDescription::Tag* render__PipelineDescription__Tag1,
			//		const OksEngine::Render::PipelineDescription::FragmentShader::ResourcePath* fragmentShader__ResourcePath1,
			//		const OksEngine::Render::PipelineDescription::FragmentShader::ResourceRequestEntityId
			//		* fragmentShader__ResourceRequestEntityId1) {

			//		if (RESOURCE__MANAGER__IS_REQUEST_FINISHED(fragmentShader__ResourceRequestEntityId1->id_)) {

			//			const ECS2::Entity::Id resourceEntityId = GetComponent<Resource::EntityId>(
			//				fragmentShader__ResourceRequestEntityId1->id_)->id_;
			//			CreateComponent<FragmentShader::ResourceEntityId>(entity1id, resourceEntityId);
			//			RemoveComponent<FragmentShader::ResourceRequestEntityId>(entity1id);

			//		}



			//	}

			//}

		}



	} // namespace Render

} // namespace OksEngine