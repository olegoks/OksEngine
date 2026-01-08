#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include <Common.Types.hpp>

#include <Render.Vulkan.Common.hpp>
#include <Render.Vulkan.Driver.Buffer.hpp>
#include <Render.Vulkan.Driver.DescriptorSet.hpp>
#include <Render.Vulkan.Driver.DescriptorSetLayout.hpp>


namespace Render::Vulkan {


	class StorageBuffer : public Buffer {
	public:

		struct CreateInfo {
			std::shared_ptr<PhysicalDevice> PD_ = nullptr;
			std::shared_ptr<LogicDevice> LD_ = nullptr;
			Common::Size sizeInBytes_ = 0;
		};

		StorageBuffer(const CreateInfo& ci) :
			Buffer{ Buffer::CreateInfo{ ci.PD_,
			ci.LD_,
			Common::CalculateAlignedSize(ci.sizeInBytes_, ci.PD_->GetProperties().limits.nonCoherentAtomSize),
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
			VK_BUFFER_USAGE_TRANSFER_DST_BIT |
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			//VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT |
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT }}, ci_{ci} {

			stagingBuffer_ = std::make_shared<StagingBuffer>(ci.PD_, ci.LD_, ci.sizeInBytes_);
			stagingBuffer_->Allocate();

		}

		void Write(Common::Size offset, const void* data, Common::Size sizeInBytes, std::shared_ptr<CommandPool> commandPool) {

			//PIXBeginEvent(PIX_COLOR(255, 255, 0), "Storage buffer: write to staging buffer.");
			//stagingBuffer_->Write(offset, data, sizeInBytes);
			//PIXEndEvent();
			
			
			
			using namespace Common;
			PIXBeginEvent(PIX_COLOR(255, 255, 0), "Storage buffer: writting data. Size %f MB.", (sizeInBytes / (float)(1_MB)));
			Buffer::WriteFlush(offset, data, sizeInBytes);
			//Buffer::
			PIXEndEvent();



			//PIXBeginEvent(PIX_COLOR(255, 255, 0), "Storage buffer: copy from staging buffer to storage");
			//Buffer::DataCopy(
			//	*stagingBuffer_, *this,
			//	offset, offset,
			//	sizeInBytes,
			//	ci_.LD_,
			//	commandPool);
			//PIXEndEvent();
		}

		void Read(Common::Size offset, void* data, Common::Size sizeInBytes, std::shared_ptr<CommandPool> commandPool) {
			using namespace Common;
			PIXBeginEvent(PIX_COLOR(255, 255, 0), "Storage buffer: reading data. Size %f MB.", (sizeInBytes / (float)(1_MB)));
			Buffer::Read(offset, data, sizeInBytes);
			PIXEndEvent();

			//using namespace Common;

			//PIXBeginEvent(PIX_COLOR(255, 255, 0), "Storage buffer: reading data. Size %f MB.", (sizeInBytes / (float)(1_MB)));
			//Buffer::DataCopy(
			//	*this, *stagingBuffer_,
			//	offset, offset,
			//	sizeInBytes,
			//	ci_.LD_,
			//	commandPool);


			//stagingBuffer_->Read(offset, data, sizeInBytes);
			//PIXEndEvent();

		}

	private:
		CreateInfo ci_;
		std::shared_ptr<StagingBuffer> stagingBuffer_ = nullptr;
	};

	using SB = StorageBuffer;

	class StorageBufferDescriptorSet : public SB {
	public:

		struct CreateInfo {
			SB::CreateInfo ubci_;
			VkShaderStageFlagBits shaderStage_ = VkShaderStageFlagBits::VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
			std::shared_ptr<CommandPool> CP_ = nullptr;
			std::shared_ptr<DescriptorPool> DP_ = nullptr;

		};

		StorageBufferDescriptorSet(const CreateInfo& ci)
			: SB{ ci.ubci_ } {


			VkDescriptorSetLayoutBinding inputAttachmentBinding{
				0,
				VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
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
				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				0, 0, SB::GetSizeInBytes());

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

	using SBDS = StorageBufferDescriptorSet;


}