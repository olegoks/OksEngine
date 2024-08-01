
#include <Render.Vulkan.Driver.Pipeline.hpp>

namespace Render::Vulkan {

	ImguiPipeline::ImguiPipeline(const CreateInfo& createInfo) :
		Pipeline{
		Pipeline::CreateInfo{
			createInfo.physicalDevice_,
			createInfo.logicDevice_,
			createInfo.renderPass_,
			{
				//{ VK_SHADER_STAGE_VERTEX_BIT, 0 , sizeof(float) * 4 }
			},
			std::vector<std::shared_ptr<DescriptorSetLayout>>{
				std::make_shared<DescriptorSetLayout>(
					DescriptorSetLayout::CreateInfo{
						"Transform",
						createInfo.logicDevice_,
						std::vector<VkDescriptorSetLayoutBinding>{{
							{
						0,
						VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
						1,
						VK_SHADER_STAGE_VERTEX_BIT,
						nullptr
					}
					}}}),
				std::make_shared<DescriptorSetLayout>(
					DescriptorSetLayout::CreateInfo{
						"DiffuseMap",
						createInfo.logicDevice_,
						std::vector<VkDescriptorSetLayoutBinding>{{
							{
						0,
						VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
						1,
						VK_SHADER_STAGE_FRAGMENT_BIT,
						nullptr
						}
						}}
					})
			},
			std::make_shared<ShaderModule>(ShaderModule::CreateInfo{
				createInfo.logicDevice_,
				createInfo.vertexShader_->GetSpirv()
				}),
			std::make_shared<ShaderModule>(ShaderModule::CreateInfo{
				createInfo.logicDevice_,
				createInfo.fragmentShader_->GetSpirv()
				}),
			nullptr,//createInfo.depthTestInfo_,
			createInfo.colorAttachmentSize_,
			createInfo.colorAttachmentFormat_,
			VertexInfo{
				Vertex2ftc::GetBindingDescription(),
				Vertex2ftc::GetAttributeDescriptions()
			},
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			VK_FRONT_FACE_COUNTER_CLOCKWISE,
			VK_CULL_MODE_NONE,
			{  /*VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR */ }
		}
	} { }



}