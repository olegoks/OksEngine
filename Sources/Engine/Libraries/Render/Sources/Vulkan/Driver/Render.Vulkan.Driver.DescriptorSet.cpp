
#include <Render.Vulkan.Driver.DescriptorSet.hpp>

#include <Render.Vulkan.Driver.Buffer.hpp>
#include <Render.Vulkan.Driver.ImageView.hpp>
#include <Render.Vulkan.Driver.Sampler.hpp>
namespace Render::Vulkan {


	void DescriptorSet::Update(const std::vector<UpdateDescriptorInfo>& infos) {
		std::vector<VkWriteDescriptorSet> writes;
		std::vector<VkDescriptorBufferInfo> descriptorBufferInfos;
		std::vector<VkDescriptorImageInfo> descriptorImageInfos;
		writes.reserve(infos.size());

		for (const UpdateDescriptorInfo& info : infos) {
			VkWriteDescriptorSet descriptorWrite;
			if (info.type_ == VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
				VkDescriptorBufferInfo bufferInfo{};
				{
					bufferInfo.buffer = *info.bufferInfo_.buffer_;
					bufferInfo.offset = info.bufferInfo_.offset_;
					bufferInfo.range = info.bufferInfo_.range_;
				}
				descriptorBufferInfos.push_back(bufferInfo);
				{
					descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrite.pNext = nullptr;
					descriptorWrite.dstSet = GetHandle();
					descriptorWrite.dstBinding = info.binding_; // Descriptor binding that we want to update.
					descriptorWrite.dstArrayElement = 0; // Descriptors can be arrrays. We also need to specify the first index in the array that we want to update.
					descriptorWrite.descriptorType = info.type_; //VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
					descriptorWrite.descriptorCount = 1; //Specifies how many array elements you want to update.
					descriptorWrite.pBufferInfo = &descriptorBufferInfos.back();
					descriptorWrite.pImageInfo = nullptr;
					descriptorWrite.pTexelBufferView = nullptr;
				}
			}
			else if(info.type_ == VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {

				VkDescriptorImageInfo imageInfo{};
				{
					imageInfo.imageView = *info.imageInfo_.imageView_;
					imageInfo.imageLayout = info.imageInfo_.imageLayout_;
					imageInfo.sampler = (info.imageInfo_.sampler_ != nullptr) ? (*info.imageInfo_.sampler_) : (nullptr);
				}
				descriptorImageInfos.push_back(imageInfo);
				{
					descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrite.pNext = nullptr;
					descriptorWrite.dstSet = GetHandle();
					descriptorWrite.dstBinding = info.binding_; // Descriptor binding that we want to update.
					descriptorWrite.dstArrayElement = 0; // Descriptors can be arrrays. We also need to specify the first index in the array that we want to update.
					descriptorWrite.descriptorType = info.type_; //VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
					descriptorWrite.descriptorCount = 1; //Specifies how many array elements you want to update.
					descriptorWrite.pBufferInfo = nullptr;
					descriptorWrite.pImageInfo = &descriptorImageInfos.back();
					descriptorWrite.pTexelBufferView = nullptr;
				}
			}
			else if (info.type_ == VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER) {
				VkDescriptorBufferInfo bufferInfo{};
				{
					bufferInfo.buffer = *info.bufferInfo_.buffer_;
					bufferInfo.offset = info.bufferInfo_.offset_;
					bufferInfo.range = info.bufferInfo_.range_;
				}
				descriptorBufferInfos.push_back(bufferInfo);
				{
					descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrite.pNext = nullptr;
					descriptorWrite.dstSet = GetHandle();
					descriptorWrite.dstBinding = info.binding_; // Descriptor binding that we want to update.
					descriptorWrite.dstArrayElement = 0; // Descriptors can be arrrays. We also need to specify the first index in the array that we want to update.
					descriptorWrite.descriptorType = info.type_; //VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
					descriptorWrite.descriptorCount = 1; //Specifies how many array elements you want to update.
					descriptorWrite.pBufferInfo = &descriptorBufferInfos.back();
					descriptorWrite.pImageInfo = nullptr;
					descriptorWrite.pTexelBufferView = nullptr;
				}
			}
			else {
				OS::NotImplemented();
			}

			writes.push_back(descriptorWrite);
		}

		vkUpdateDescriptorSets(
			*createInfo_.LD_,
			static_cast<uint32_t>(writes.size()), writes.data(),
			0, nullptr);


	}

	void DescriptorSet::UpdateBufferWriteConfiguration(
		Buffer& buffer,
		VkDescriptorType type,
		Common::UInt32 binding,
		Common::Size offset,
		Common::Size range) {


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

	void DescriptorSet::UpdateBufferWriteConfiguration(
		std::shared_ptr<Buffer> buffer,
		VkDescriptorType type,
		Common::UInt32 binding,
		Common::Size offset,
		Common::Size range) {

		UpdateBufferWriteConfiguration(
			*buffer,
			type,
			binding,
			offset,
			range);

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
			imageInfo.sampler = (sampler != nullptr) ? (*sampler) : (nullptr);
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