
#include <Render.Vulkan.Driver.DescriptorSet.hpp>

#include <Render.Vulkan.Driver.Buffer.hpp>
#include <Render.Vulkan.Driver.ImageView.hpp>
#include <Render.Vulkan.Driver.Sampler.hpp>
namespace Render::Vulkan {


	void DescriptorSet::Update(const std::vector<Binding::UpdateInfo>& infos) {

		for (const Binding::UpdateInfo& info : infos) {
			VkWriteDescriptorSet descriptorWrite;
			
			std::vector<VkDescriptorBufferInfo> descriptorBufferInfos;
			descriptorBufferInfos.reserve(info.bufferInfos_.size());
			std::vector<VkDescriptorImageInfo> descriptorImageInfos;
			descriptorImageInfos.reserve(info.imageInfos_.size());

			//Reuse buffers.
			descriptorBufferInfos.clear();
			descriptorImageInfos.clear();

			if (info.type_ == VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER ||
				info.type_ == VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER) {
				
				ASSERT(info.imageInfos_.empty());

				for (const auto& bufferInfo : info.bufferInfos_) {
					VkDescriptorBufferInfo vkBufferInfo;
					{
						vkBufferInfo.buffer = bufferInfo.buffer_->GetHandle();
						vkBufferInfo.offset = bufferInfo.offset_;
						vkBufferInfo.range = bufferInfo.range_;
					}
					descriptorBufferInfos.push_back(vkBufferInfo);
				}

				{
					descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrite.pNext = nullptr;
					descriptorWrite.dstSet = GetHandle();
					descriptorWrite.dstBinding = info.binding_;						// Descriptor binding that we want to update.
					descriptorWrite.dstArrayElement = info.arrayElement_;							// Descriptors can be arrays. We also need to specify the first index in the array that we want to update.
					descriptorWrite.descriptorType = info.type_;					//VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
					descriptorWrite.descriptorCount = descriptorBufferInfos.size();	//Specifies how many array elements you want to update.
					descriptorWrite.pBufferInfo = &descriptorBufferInfos.back();
					descriptorWrite.pImageInfo = nullptr;
					descriptorWrite.pTexelBufferView = nullptr;
				}  
			}
			else if(info.type_ == VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {

				ASSERT(info.bufferInfos_.empty());

				for (const auto& imageInfo : info.imageInfos_) {
					VkDescriptorImageInfo vkImageInfo{};
					{
						vkImageInfo.imageView = imageInfo.imageView_->GetHandle();
						vkImageInfo.imageLayout = imageInfo.imageLayout_;
						vkImageInfo.sampler = (imageInfo.sampler_ != nullptr) ? (imageInfo.sampler_->GetHandle()) : (VK_NULL_HANDLE);
					}
					descriptorImageInfos.push_back(vkImageInfo);
				}

				{
					descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrite.pNext = nullptr;
					descriptorWrite.dstSet = GetHandle();
					descriptorWrite.dstBinding = info.binding_;		// Descriptor binding that we want to update.
					descriptorWrite.dstArrayElement = info.arrayElement_;			// Descriptors can be arrays. We also need to specify the first index in the array that we want to update.
					descriptorWrite.descriptorType = info.type_;	//VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
					descriptorWrite.descriptorCount = descriptorImageInfos.size();			//Specifies how many array elements you want to update.
					descriptorWrite.pBufferInfo = nullptr;
					descriptorWrite.pImageInfo = descriptorImageInfos.data() + descriptorImageInfos.size() - 1;
					descriptorWrite.pTexelBufferView = nullptr;
				}
			}
			else {
				NOT_IMPLEMENTED();
			}

			//Update each binding separately.
			vkUpdateDescriptorSets(
				*createInfo_.LD_,
				1, &descriptorWrite,
				0, nullptr);
		}

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