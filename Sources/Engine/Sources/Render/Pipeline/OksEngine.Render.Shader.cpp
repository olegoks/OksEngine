#pragma once
#include <Render\Pipeline\auto_OksEngine.Render.Shader.hpp>
#include <Render\Pipeline\OksEngine.Render.Shader.Utils.hpp>

#include <Resources/OksEngine.ResourceSystem.Utils.hpp>
#include <Common/Async/OksEngine.Async.Utils.hpp>

#include <RAL.Driver.hpp>

#include <Render/Pipeline/SPIRV-Reflect/spirv_reflect.h>

#include <Render/Pipeline/auto_OksEngine.Render.Attachment.hpp>

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

			void SaveText::Update(
				ECS2::Entity::Id entity0id, 
				const OksEngine::Render::Shader::Tag* shader__Tag0,
				const OksEngine::Render::Shader::ResourceEntityId* shader__ResourceEntityId0) {

				const auto* resourceData = GetComponent<Resource::Data>(shader__ResourceEntityId0->id_);
				CreateComponent<Shader::Text>(entity0id, std::string{ resourceData->data_.data(), resourceData->data_.size() });

			}


			void CreateCompileShaderAsyncTask::Update(
				ECS2::Entity::Id entity0id, const OksEngine::Render::Shader::Tag* shader__Tag0,
				const OksEngine::Name* name0, const OksEngine::Render::Shader::Text* shader__Text0) {



				const RAL::Driver::Shader::Type shaderType = RENDER__SHADER__GET_SHADER_TYPE(entity0id);
				const ECS2::Entity::Id compileTaskEntityId = COMMON__ASYNC__CREATE_TASK(
					([text = shader__Text0->text_, shaderType, name = name0->value_]() -> std::any {

						BEGIN_PROFILE("Compiling shader %s", name.c_str());
						auto compiler = RAL::Driver::Shader::Compiler::Create();

						RAL::Driver::Shader::Compiler::Parameters parameters{
							.type_ = shaderType,
							.name_ = name,
							.text_ = text
						};

						auto byteCode = compiler->Compile(parameters);
						END_PROFILE();
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

			//not in use now
			static Attachment_::Format ConvertSpvReflectFormatToEnum(SpvReflectFormat spvFormat) {
				switch (spvFormat) {
				case SPV_REFLECT_FORMAT_R16_UINT:           return Attachment_::Format::R16_UINT;
				case SPV_REFLECT_FORMAT_R16_SINT:           return Attachment_::Format::R16_SINT;
				case SPV_REFLECT_FORMAT_R16_SFLOAT:         return Attachment_::Format::R16_SFLOAT;
				case SPV_REFLECT_FORMAT_R16G16_UINT:        return Attachment_::Format::R16G16_UINT;
				case SPV_REFLECT_FORMAT_R16G16_SINT:        return Attachment_::Format::R16G16_SINT;
				case SPV_REFLECT_FORMAT_R16G16_SFLOAT:      return Attachment_::Format::R16G16_SFLOAT;
				case SPV_REFLECT_FORMAT_R16G16B16A16_UINT:  return Attachment_::Format::R16G16B16A16_UINT;
				case SPV_REFLECT_FORMAT_R16G16B16A16_SINT:  return Attachment_::Format::R16G16B16A16_SINT;
				case SPV_REFLECT_FORMAT_R16G16B16A16_SFLOAT:return Attachment_::Format::R16G16B16A16_SFLOAT;
				case SPV_REFLECT_FORMAT_R32_UINT:           return Attachment_::Format::R32_UINT;
				case SPV_REFLECT_FORMAT_R32_SINT:           return Attachment_::Format::R32_SINT;
				case SPV_REFLECT_FORMAT_R32_SFLOAT:         return Attachment_::Format::R32_SFLOAT;
				case SPV_REFLECT_FORMAT_R32G32_UINT:        return Attachment_::Format::R32G32_UINT;
				case SPV_REFLECT_FORMAT_R32G32_SINT:        return Attachment_::Format::R32G32_SINT;
				case SPV_REFLECT_FORMAT_R32G32_SFLOAT:      return Attachment_::Format::R32G32_SFLOAT;
				case SPV_REFLECT_FORMAT_R32G32B32A32_UINT:  return Attachment_::Format::R32G32B32A32_UINT;
				case SPV_REFLECT_FORMAT_R32G32B32A32_SINT:  return Attachment_::Format::R32G32B32A32_SINT;
				case SPV_REFLECT_FORMAT_R32G32B32A32_SFLOAT:return Attachment_::Format::R32G32B32A32_SFLOAT;
					// Depth/stencil форматы в SPIRV-Reflect обычно не представлены,
					// но если есть соответствующие константы, добавить. Иначе - Undefined.
				default:                                     
					ASSERT_FAIL();
					return Attachment_::Format::Undefined;
				}
			}

			void ShaderReflection::Update(
				ECS2::Entity::Id entity0id,
				const OksEngine::RenderDriver* renderDriver0,
				
				ECS2::Entity::Id entity1id,
				const OksEngine::Render::Shader::Tag* shader__Tag1,
				const OksEngine::Render::Shader::Binary* shader__Binary1) {


				std::vector<Common::UInt32> spirvCode = shader__Binary1->data_;

				SpvReflectShaderModule module;
				SpvReflectResult result = spvReflectCreateShaderModule(
					spirvCode.size() * sizeof(uint32_t),
					spirvCode.data(), 
					&module
				);

				ASSERT(result == SPV_REFLECT_RESULT_SUCCESS);

				//not in use now
				{
					// После того как вы загрузили модуль (spvReflectCreateShaderModule)
					uint32_t output_count = 0;
					spvReflectEnumerateOutputVariables(&module, &output_count, nullptr);
					std::vector<SpvReflectInterfaceVariable*> outputs(output_count);
					spvReflectEnumerateOutputVariables(&module, &output_count, outputs.data());

					for (size_t i = 0; i < outputs.size(); ++i) {
						SpvReflectInterfaceVariable* output = outputs[i];
						// location — это индекс color attachment, с которым ассоциирована переменная
						uint32_t location = output->location;
						// Имя переменной (например, "fColor")
						const char* name = output->name;
						// Информация о типе (через output->type_description)
						SpvReflectTypeDescription* type_desc = output->type_description;
						// Например, type_desc->op указывает на тип (OpTypeFloat, OpTypeVector и т.д.)
						// Для векторов можно узнать количество компонентов:
						uint32_t component_count = 0;
						if (type_desc->type_flags & SPV_REFLECT_TYPE_FLAG_VECTOR) {
							component_count = type_desc->traits.numeric.vector.component_count;
						}
						// Также можно получить базовый скалярный тип (float, int, uint)
						// и использовать эту информацию для выбора формата Vulkan.
					}
				}
				//Descriptor sets
				{
					uint32_t count = 0;
					result = spvReflectEnumerateDescriptorSets(&module, &count, NULL);
					assert(result == SPV_REFLECT_RESULT_SUCCESS);

					std::vector<SpvReflectDescriptorSet*> sets(count);
					result = spvReflectEnumerateDescriptorSets(&module, &count, sets.data());
					assert(result == SPV_REFLECT_RESULT_SUCCESS);




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
						Binding ecsBinding{
							binding->name,
							binding->set,
							binding->binding,
							(binding->count != 0) ?(binding->count):(4096/*renderDriver0->driver_->GetLimits().maxResourceSetSampledTextures_*/),
							toECSDescriptorType(binding->descriptor_type)
						};

						if (binding->type_description->op == SpvOp::SpvOpTypeRuntimeArray) {
							ecsBinding.count_ = 4096;
						}

						if (binding->type_description->op == SpvOp::SpvOpTypeArray && binding->count == 1) {
							//hiddent bindless array.
							ecsBinding.count_ = 4096;
						}

						ecsBindings.emplace_back(ecsBinding);
					}
					if (!ecsBindings.empty()) {
						CreateComponent<Bindings>(entity1id, ecsBindings);
					}
				}
				//Push constants
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
						CreateComponent<PushConstants>(entity1id, ecsPushConstants);
					}
				}

				spvReflectDestroyShaderModule(&module);
				CreateComponent<Shader::Ready>(entity1id);
				//return true;

			}

		}

	} // namespace Render

} // namespace OksEngine