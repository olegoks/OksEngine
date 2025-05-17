#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include <Common.Types.hpp>

#include <Render.Vulkan.Common.hpp>
#include <Render.Vulkan.Driver.Buffer.hpp>
#include <Render.Vulkan.Driver.DescriptorSet.hpp>
#include <Render.Vulkan.Driver.DescriptorSetLayout.hpp>


namespace Render::Vulkan {


	class UniformBuffer : public Buffer {
	public:

		struct CreateInfo {
			std::shared_ptr<PhysicalDevice> PD_ = nullptr;
			std::shared_ptr<LogicDevice> LD_ = nullptr;
			Common::Size sizeInBytes_ = 0;
		};

		UniformBuffer(const CreateInfo& ci) :
			Buffer{ Buffer::CreateInfo{ ci.PD_,
			ci.LD_,
			ci.sizeInBytes_,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT } } {



		}

	};

	using UB = UniformBuffer;


	class UniformBufferDescriptorSet : public UB {
	public:

		struct CreateInfo {
			UB::CreateInfo ubci_;
			VkShaderStageFlagBits shaderStage_ = VkShaderStageFlagBits::VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
			std::shared_ptr<CommandPool> CP_ = nullptr;
			std::shared_ptr<DescriptorPool> DP_ = nullptr;

		};

		UniformBufferDescriptorSet(const CreateInfo& ci) 
			: UB{ ci.ubci_ } {


			VkDescriptorSetLayoutBinding inputAttachmentBinding{
				0,
				VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				1,
				static_cast<VkFlags>(ci.shaderStage_),
				nullptr
			};

			auto dsl = std::make_shared<DescriptorSetLayout>(
				DescriptorSetLayout::CreateInfo{
					"Rendered buffer",
					ci.ubci_.LD_,
					std::vector{ inputAttachmentBinding }
				});

			DescriptorSet::CreateInfo DSCreateInfo;
			{
				DSCreateInfo.DP_ = ci.DP_;
				DSCreateInfo.DSL_ = dsl;
				DSCreateInfo.LD_ = ci.ubci_.LD_;
			}

			auto ds = std::make_shared<DescriptorSet>(DSCreateInfo);

			ds->UpdateBufferWriteConfiguration(
				*this,
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				0, 0, UB::GetSizeInBytes());

			DSL_ = dsl;
			DS_ = ds;
		}

		auto GetDSL() noexcept {
			return DSL_;
		}

		auto GetDS() noexcept {
			return DS_;
		}

		std::shared_ptr<DescriptorSetLayout> DSL_ = nullptr;
		std::shared_ptr<DescriptorSet> DS_ = nullptr;
	};

	using UBDS = UniformBufferDescriptorSet;

	template<class StructureType>
	class UniformStructure : public UniformBuffer {
	public:
		struct CreateInfo {
			std::shared_ptr<PhysicalDevice> physicalDevice_;
			std::shared_ptr<LogicDevice> LD_;
		};
		UniformStructure(const CreateInfo& createInfo) :
			Buffer{ Buffer::CreateInfo{
					createInfo.physicalDevice_,
				createInfo.LD_,
				sizeof(StructureType),
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT }
		} {

		}

		void Fill(const StructureType* structure) {
			UniformBuffer::Fill(structure);
		}

	};

	template<class ElementType>
	class UniformArray : public UniformBuffer {
	public:
		struct CreateInfo {
			std::shared_ptr<PhysicalDevice> physicalDevice_;
			std::shared_ptr<LogicDevice> LD_;
			Common::Size size_ = 0;
		};
		UniformArray(const CreateInfo& createInfo) : Buffer{
			Buffer::CreateInfo{
				createInfo.physicalDevice_,
				createInfo.LD_,
				sizeof(ElementType) * createInfo.size_,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT }
		} { }

		void Fill(const ElementType* structure) {
			UniformBuffer::Fill(structure);
		}

	};

}