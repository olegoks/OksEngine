#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include <Common.Types.hpp>
#include <OS.Assert.hpp>
#include <OS.Logger.hpp>

#include <Render.Vulkan.Common.hpp>
#include <Render.Vulkan.Abstraction.hpp>

#include <Render.Vulkan.Driver.DescriptorPool.hpp>
#include <Render.Vulkan.Driver.LogicDevice.hpp>
#include <Render.Vulkan.Driver.DescriptorSetLayout.hpp>
#include <Render.Vulkan.Driver.UniformBuffer.hpp>


namespace Render::Vulkan {

	class DescriptorSet : public Abstraction<VkDescriptorSet>{
	public:

		struct CreateInfo {
			std::shared_ptr<LogicDevice> logicDevice_;
			std::shared_ptr<DescriptorPool> descriptorPool_;
			std::shared_ptr<DescriptorSetLayout> DSL_;
			//std::shared_ptr<UniformBuffer> buffer_;
			//VkDescriptorType type_;
		};

		DescriptorSet(const CreateInfo& createInfo) : 
			createInfo_{ createInfo } {

			Allocate(
				createInfo.logicDevice_->GetHandle(),
				createInfo.descriptorPool_->GetNative(),
				*createInfo.DSL_);

			/*Update(
				createInfo.logicDevice_->GetHandle(),
				createInfo.buffer_->GetNative(),
				createInfo.type_,
				createInfo.buffer_->GetSizeInBytes(),
				0);*/

		}

		void UpdateBufferWriteConfiguration(
			std::shared_ptr<class Buffer> buffer,
			VkDescriptorType type,
			Common::UInt32 binding,
			Common::Size offset,
			Common::Size range);


		void UpdateImageWriteConfiguration(
			std::shared_ptr<class ImageView> imageView,
			VkImageLayout imageLayout,
			std::shared_ptr<class Sampler> sampler,
			VkDescriptorType type,
			Common::UInt32 binding);

		~DescriptorSet() {
			OS::Assert(GetHandle() != VK_NULL_HANDLE);
			vkFreeDescriptorSets(
				createInfo_.logicDevice_->GetHandle(),
				createInfo_.descriptorPool_->GetNative(),
				1,
				&GetHandle());
		}

	private:

		void Update(VkDevice logicDevice, VkBuffer buffer, VkDescriptorType type, Common::Size range, Common::Size offset = 0)  noexcept {

			VkDescriptorBufferInfo bufferInfo{};
			{
				bufferInfo.buffer = buffer;
				bufferInfo.offset = offset;
				bufferInfo.range = range;
			}

			VkWriteDescriptorSet descriptorWrite{};
			{
				descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrite.dstSet = GetHandle();
				descriptorWrite.dstBinding = 0; // Descriptor binding that we want to update.
				descriptorWrite.dstArrayElement = 0; // Descriptors can be arrrays. We also need to specify the first index in the array that we want to update.
				descriptorWrite.descriptorType = type; //VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorWrite.descriptorCount = 1; //Specifies how many array elements you want to update.
				descriptorWrite.pBufferInfo = &bufferInfo;
				descriptorWrite.pImageInfo = nullptr;
				descriptorWrite.pTexelBufferView = nullptr;
			}
			vkUpdateDescriptorSets(logicDevice, 1, &descriptorWrite, 0, nullptr);
		}

		void Allocate(VkDevice logicDevice, VkDescriptorPool descriptorPool, VkDescriptorSetLayout descriptorSetLayout) noexcept {
			VkDescriptorSetAllocateInfo allocInfo{};
			{
				allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
				allocInfo.descriptorPool = descriptorPool;
				allocInfo.descriptorSetCount = 1;
				allocInfo.pSetLayouts = &descriptorSetLayout;
			}
			VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
			VkCall(vkAllocateDescriptorSets(logicDevice, &allocInfo, &descriptorSet), 
				"Error while allocating memory for Descriptor Set");
			SetHandle(descriptorSet);
		}

	private:
		CreateInfo createInfo_;
	};




}