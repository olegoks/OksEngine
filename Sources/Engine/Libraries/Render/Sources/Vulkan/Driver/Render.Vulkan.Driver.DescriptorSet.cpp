
#include <Render.Vulkan.Driver.DescriptorSet.hpp>

#include <Render.Vulkan.Driver.Buffer.hpp>
#include <Render.Vulkan.Driver.ImageView.hpp>
#include <Render.Vulkan.Driver.Sampler.hpp>
namespace Render::Vulkan {


	void DescriptorSet::UpdateBufferWriteConfiguration(
		std::shared_ptr<Buffer> buffer,
		VkDescriptorType type,
		Common::UInt32 binding,
		Common::Size offset,
		Common::Size range) {


		VkDescriptorBufferInfo bufferInfo{};
		{
			bufferInfo.buffer = *buffer;
			bufferInfo.offset = offset;
			bufferInfo.range = range;
		}

		VkWriteDescriptorSet descriptorWrite{};
		{
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = GetHandle();
			descriptorWrite.dstBinding = binding; // Descriptor binding that we want to update.
			descriptorWrite.dstArrayElement = 0; // Descriptors can be arrrays. We also need to specify the first index in the array that we want to update.
			descriptorWrite.descriptorType = type; //VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1; //Specifies how many array elements you want to update.
			descriptorWrite.pBufferInfo = &bufferInfo;
			descriptorWrite.pImageInfo = nullptr;
			descriptorWrite.pTexelBufferView = nullptr;
		}
		vkUpdateDescriptorSets(*createInfo_.LD_, 1, &descriptorWrite, 0, nullptr);

	}


	void DescriptorSet::UpdateImageWriteConfiguration(
		std::shared_ptr<ImageView> imageView,
		VkImageLayout imageLayout,
		std::shared_ptr<Sampler> sampler,
		VkDescriptorType type,
		Common::UInt32 binding) {

		VkDescriptorImageInfo imageInfo{};
		{
			imageInfo.imageView = *imageView;
			imageInfo.imageLayout = imageLayout;
			imageInfo.sampler = *sampler;
		}

		VkWriteDescriptorSet descriptorWrite{};
		{
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = GetHandle();
			descriptorWrite.dstBinding = binding; // Descriptor binding that we want to update.
			descriptorWrite.dstArrayElement = 0; // Descriptors can be arrrays. We also need to specify the first index in the array that we want to update.
			descriptorWrite.descriptorType = type; //VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1; //Specifies how many array elements you want to update.
			descriptorWrite.pBufferInfo = nullptr;
			descriptorWrite.pImageInfo = &imageInfo;
			descriptorWrite.pTexelBufferView = nullptr;
		}
		vkUpdateDescriptorSets(*createInfo_.LD_, 1, &descriptorWrite, 0, nullptr);

	}

}