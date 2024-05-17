#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

export module OksEngine.Render.Vulkan.Driver.DescriptorSet;

import OksEngine.Common.Types;

import OksEngine.OS.Assert;
import OksEngine.OS.Logger;

import OksEngine.Render.Vulkan.Common;

import OksEngine.Render.Vulkan.Driver.DescriptorPool;
import OksEngine.Render.Vulkan.Driver.LogicDevice;
import OksEngine.Render.Vulkan.Driver.DescriptorSetLayout;
import OksEngine.Render.Vulkan.Driver.UniformBuffer;

namespace Render::Vulkan {

	class DescriptorSet {
	public:

		struct CreateInfo {
			std::shared_ptr<LogicDevice> logicDevice_;
			std::shared_ptr<DescriptorPool> descriptorPool_;
			std::shared_ptr<DescriptorSetLayout> descriptorSetLayout_;
			std::shared_ptr<UniformBuffer> buffer_;
			VkDescriptorType type_;
		};

		DescriptorSet(const CreateInfo& createInfo) {

			Allocate(
				createInfo.logicDevice_->GetHandle(),
				createInfo.descriptorPool_->GetNative(),
				createInfo.descriptorSetLayout_->GetNative());

			Update(
				createInfo.logicDevice_->GetHandle(),
				createInfo.buffer_->GetNative(),
				createInfo.type_,
				createInfo.buffer_->GetSizeInBytes(),
				0);

		}

		[[nodiscard]]
		const VkDescriptorSet& GetNative() const noexcept { return descriptorSet_; }

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
				descriptorWrite.dstSet = GetNative();
				descriptorWrite.dstBinding = 0;
				descriptorWrite.dstArrayElement = 0;
				descriptorWrite.descriptorType = type; //VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorWrite.descriptorCount = 1;
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
			SetNative(descriptorSet);
		}

		void SetNative(VkDescriptorSet descriptorSet) noexcept {
			OS::Assert((descriptorSet != VK_NULL_HANDLE) && (GetNative() == VK_NULL_HANDLE) ||
				((descriptorSet == VK_NULL_HANDLE) && (GetNative() != VK_NULL_HANDLE)));
			descriptorSet_ = descriptorSet;
		}

	private:
		VkDescriptorSet descriptorSet_ = VK_NULL_HANDLE;
	};


}