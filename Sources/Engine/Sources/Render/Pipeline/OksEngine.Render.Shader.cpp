#pragma once
#include <Render\Pipeline\auto_OksEngine.Render.Shader.hpp>
#include <Render\Pipeline\OksEngine.Render.Shader.Utils.hpp>

#include <Resources/OksEngine.ResourceSystem.Utils.hpp>
#include <Common/Async/OksEngine.Async.Utils.hpp>

#include <RAL.Driver.hpp>

#include <Render/Pipeline/SPIRV-Reflect/spirv_reflect.h>

namespace OksEngine
{

	namespace Render
	{
		namespace Shader
		{



			void EditBindings(std::shared_ptr<ECS2::World> ecsWorld, Bindings* bindings) {
				ImGui::PushID(Bindings::GetTypeId());
				for (const auto& binding : bindings->bindings_) {
					ImGui::Indent(20.f);
					ImGui::TextDisabled("Name %s", binding.name_.c_str());
					ImGui::TextDisabled("Resource type %s", magic_enum::enum_name(binding.resourceType_).data());
					ImGui::TextDisabled("Set %d", binding.set_);
					ImGui::TextDisabled("Binding %d", binding.binding_);
					ImGui::TextDisabled("Count %d", binding.count_);
					ImGui::Unindent(20.f);
					ImGui::Separator();
				}
				ImGui::PopID();
			}

			void EditPushConstants(std::shared_ptr<ECS2::World> ecsWorld, PushConstants* pushConstants) {
				ImGui::PushID(Bindings::GetTypeId());
				for (const auto& pushConstant : pushConstants->constants_) {
					ImGui::Indent(20.f);
					ImGui::TextDisabled("Name %s", pushConstant.name_.c_str());
					ImGui::TextDisabled("Offset %d", pushConstant.offset_);
					ImGui::TextDisabled("Size %d", pushConstant.size_);
					ImGui::Unindent(20.f);
					ImGui::Separator();
				}
				ImGui::PopID();
			}

			ResourcePath ParseResourcePath(luabridge::LuaRef& vertexShaderRef) {

				luabridge::LuaRef resourcePathRef = vertexShaderRef["resourcePath"];
				std::string resourcePath = resourcePathRef.cast<std::string>().value();
				ASSERT(Resource::IsResourcePath(resourcePath));
				return ResourcePath{ std::move(resourcePath) };

			};

			void CreateResourceRequest::Update(ECS2::Entity::Id entity0id, const OksEngine::Render::Shader::Tag* shader__Tag0,
				const OksEngine::Render::Shader::ResourcePath* shader__ResourcePath0) {


				const ECS2::Entity::Id requestEntityId = RESOURCE__MANAGER__CREATE_LOAD_RESOURCE_REQUEST(
					shader__ResourcePath0->resourcePath_);

				CreateComponent<Shader::ResourceRequestEntityId>(entity0id, requestEntityId);

			};


			void SaveResourceEntity::Update(
				ECS2::Entity::Id entity0id,
				const OksEngine::Render::Shader::Tag* shader__Tag0,
				const OksEngine::Render::Shader::ResourcePath* shader__ResourcePath0,
				const OksEngine::Render::Shader::ResourceRequestEntityId* shader__ResourceRequestEntityId0) {


				if (RESOURCE__MANAGER__IS_REQUEST_FINISHED(shader__ResourceRequestEntityId0->id_)) {

					const ECS2::Entity::Id resourceEntityId = GetComponent<Resource::EntityId>(
						shader__ResourceRequestEntityId0->id_)->id_;
					CreateComponent<Shader::ResourceEntityId>(entity0id, resourceEntityId);
					RemoveComponent<Shader::ResourceRequestEntityId>(entity0id);

				}

			};


			void CreateCompileShaderAsyncTask::Update(
				ECS2::Entity::Id entity0id,
				const OksEngine::Render::Shader::Tag* shader__Tag0,
				const OksEngine::Name* name0,
				const OksEngine::Render::Shader::ResourceEntityId* shader__ResourceEntityId0) {

				const auto* data = GetComponent<Resource::Data>(shader__ResourceEntityId0->id_);


				std::string shaderText{ data->data_.data(), data->data_.size() };
				const RAL::Driver::Shader::Type shaderType = RENDER__SHADER__GET_SHADER_TYPE(entity0id);
				const ECS2::Entity::Id compileTaskEntityId = COMMON__ASYNC__CREATE_TASK(
					([resourceData = data->data_, shaderType, name = name0->value_]() -> std::any {

						std::string shaderText{ resourceData.data(), resourceData.size() };

						auto compiler = RAL::Driver::Shader::Compiler::Create();
						
						RAL::Driver::Shader::Compiler::Parameters parameters{
							.type_ = shaderType,
							.name_ = name,
							.text_ = shaderText
						};

						auto byteCode = compiler->Compile(parameters);

						return std::any{ std::move(byteCode) };
						}));

				CreateComponent<Shader::CompileTaskEntityId>(entity0id, compileTaskEntityId);
			};


			void SaveBinaryShader::Update(
				ECS2::Entity::Id entity0id,
				const OksEngine::Render::Shader::Tag* shader__Tag0,
				const OksEngine::Render::Shader::CompileTaskEntityId* shader__CompileTaskEntityId0) {

				if (COMMON__ASYNC__IS_TASK_FINISHED(shader__CompileTaskEntityId0->id_)) {
					auto* info = GetComponent<Async::Manager::Task::Info>(shader__CompileTaskEntityId0->id_);
					auto binary = *info->future_._Get_value()._Cast<std::vector<Common::UInt32>>();
					CreateComponent<Shader::Binary>(entity0id, std::move(binary));

					//Destroy request entity 
					DestroyEntity(shader__CompileTaskEntityId0->id_);
				}

			};

			void ShaderReflection::Update(
				ECS2::Entity::Id entity0id,
				const OksEngine::Render::Shader::Tag* shader__Tag0,
				const OksEngine::Render::Shader::Binary* shader__Binary0) {


				std::vector<Common::UInt32> spirvCode = shader__Binary0->data_;

				SpvReflectShaderModule module;
				SpvReflectResult result = spvReflectCreateShaderModule(
					spirvCode.size() * sizeof(uint32_t),
					spirvCode.data(),
					&module
				);

				ASSERT(result == SPV_REFLECT_RESULT_SUCCESS);
				{
					uint32_t bindingCount = 0;
					result = spvReflectEnumerateDescriptorBindings(&module, &bindingCount, nullptr);
					if (result != SPV_REFLECT_RESULT_SUCCESS) {
						spvReflectDestroyShaderModule(&module);
						ASSERT_FAIL();
					}

					std::vector<SpvReflectDescriptorBinding*> bindings(bindingCount);
					result = spvReflectEnumerateDescriptorBindings(&module, &bindingCount, bindings.data());

					if (result != SPV_REFLECT_RESULT_SUCCESS) {
						spvReflectDestroyShaderModule(&module);
						ASSERT_FAIL();
					}

					auto toECSDescriptorType = [](SpvReflectDescriptorType descriptor_type) {
						//typedef enum SpvReflectDescriptorType {
						//	SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER = 0,        // = VK_DESCRIPTOR_TYPE_SAMPLER
						//	SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER = 1,        // = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
						//	SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE = 2,        // = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE
						//	SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE = 3,        // = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
						//	SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER = 4,        // = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER
						//	SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER = 5,        // = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER
						//	SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER = 6,        // = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
						//	SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER = 7,        // = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
						//	SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC = 8,        // = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
						//	SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC = 9,        // = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC
						//	SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT = 10,        // = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT
						//	SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR = 1000150000 // = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR
						//} SpvReflectDescriptorType;

						if (descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
							return ResourceType::Sampler;
						}
						else if (descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER) {
							return ResourceType::Storage;
						}
						else if (descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
							return ResourceType::Uniform;
						}
						else {
							NOT_IMPLEMENTED();
							return ResourceType::Undefined;
						}

						
						};


					std::vector<Binding> ecsBindings;
					for (const auto* binding : bindings) {
						printf("Binding: set=%u, binding=%u, name=%s, type=%d, count=%u\n",
							binding->set,
							binding->binding,
							binding->name,
							binding->descriptor_type,
							binding->count);

						Binding ecsBinding{
							binding->name,
							binding->set,
							binding->binding,
							binding->count,
							toECSDescriptorType(binding->descriptor_type)
						};
						ecsBindings.emplace_back(ecsBinding);
					}
					if (!ecsBindings.empty()) {
						CreateComponent<Bindings>(entity0id, ecsBindings);
					}
				}
				{
					uint32_t pushConstantCount = 0;
					SpvReflectResult result = spvReflectEnumeratePushConstantBlocks(&module, &pushConstantCount, nullptr);
					ASSERT(result == SPV_REFLECT_RESULT_SUCCESS);

					std::vector<SpvReflectBlockVariable*> pushConstants(pushConstantCount);
					result = spvReflectEnumeratePushConstantBlocks(&module, &pushConstantCount, pushConstants.data());
					ASSERT(result == SPV_REFLECT_RESULT_SUCCESS);

					std::vector<Shader::PushConstant> ecsPushConstants;
					for (const auto* pc : pushConstants) {
						//printf("Push constant block: name=%s, size=%u bytes, offset=%u\n",
						//	pc->name, pc->size, pc->offset);

						Render::Shader::PushConstant ecsPushConstant{
							pc->name,
							pc->offset,
							pc->size
						};
						ecsPushConstants.emplace_back(ecsPushConstant);
					}
					if (!ecsPushConstants.empty()) {
						CreateComponent<PushConstants>(entity0id, ecsPushConstants);
					}
				}

				spvReflectDestroyShaderModule(&module);
				//return true;

			}

		}

	} // namespace Render

} // namespace OksEngine