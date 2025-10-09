#include <Render.Vulkan.Driver.Buffer.hpp>

#include <Render.Vulkan.Driver.CommandBuffer.hpp>
#include <Render.Vulkan.Driver.DeviceMemory.hpp>

namespace Render::Vulkan
{


	Buffer::Buffer(const CreateInfo& createInfo) :
		createInfo_{ createInfo } {

		VkBufferCreateInfo bufferInfo{};
		{
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = createInfo.size_;
			bufferInfo.usage = createInfo.usage_;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}
		VkBuffer buffer = VK_NULL_HANDLE;
		VK_CALL(vkCreateBuffer(createInfo_.LD_->GetHandle(), &bufferInfo, nullptr, &buffer), 
			"Error while creating buffer.");

		SetHandle(buffer);
	}

	void Buffer::Write(Common::Size offset, const void* data, Common::Size sizeInBytes) noexcept {
		ASSERT(memory_ != nullptr);
		ASSERT(offset + sizeInBytes <= memory_->GetSize());
		memory_->Write(offset, data, sizeInBytes);
	}

	void Buffer::Read(Common::Size offsetInBytes, void* memory, Common::Size bytesNumber) noexcept {
		ASSERT(memory_ != nullptr);
		memory_->Read(offsetInBytes, memory, bytesNumber);
	}

	void Buffer::CopyDataTo(std::shared_ptr<Buffer> bufferTo, std::shared_ptr<CommandPool> commandPool) {
		CopyFullDataTo(*bufferTo, commandPool);

	}

	void Buffer::CopyFullDataTo(Buffer& bufferTo, std::shared_ptr<CommandPool> commandPool) {
		ASSERT(GetSizeInBytes() == bufferTo.GetSizeInBytes());
		DataCopy(*this, bufferTo, 0, 0, bufferTo.GetSizeInBytes(), createInfo_.LD_, commandPool);
	}

	void Buffer::DataCopy(const std::shared_ptr<Buffer> bufferFrom, std::shared_ptr<Buffer> bufferTo, Common::Size offsetFrom, Common::Size offsetTo, Common::Size bytesNumber, std::shared_ptr<LogicDevice> ld, std::shared_ptr<CommandPool> commandPool) {

		DataCopy(*bufferFrom, *bufferTo, offsetFrom, offsetTo, bytesNumber, ld, commandPool);

	}

	void Buffer::DataCopy(const Buffer& bufferFrom, Buffer& bufferTo, Common::Size offsetFrom, Common::Size offsetTo, Common::Size bytesNumber, std::shared_ptr<LogicDevice> ld, std::shared_ptr<CommandPool> commandPool) {


		CommandBuffer::CreateInfo commandBufferCreateInfo;
		{
			commandBufferCreateInfo.LD_ = ld;
			commandBufferCreateInfo.commandPool_ = commandPool;
			commandBufferCreateInfo.level_ = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		}
		auto commandBuffer = std::make_shared<CommandBuffer>(commandBufferCreateInfo);

		commandBuffer->Begin();
		commandBuffer->Copy(bufferFrom, bufferTo, offsetFrom, offsetTo, bytesNumber);
		commandBuffer->End();

		//Fence::CreateInfo fenceCI{
		//	.LD_ = ld
		//};

		//auto fence = std::make_shared<Fence>(fenceCI);
		//fence->Reset();
		commandBuffer->Submit(ld->GetGraphicsQueue()/*, fence*/);

		//fence->Wait();
		vkQueueWaitIdle(ld->GetGraphicsQueue());

	}


	[[nodiscard]]
	Common::Size Buffer::GetSizeInBytes() const noexcept { return createInfo_.size_; }

	void Buffer::BindMemory(std::shared_ptr<DeviceMemory> memory) {

		OS::Assert(memory_ == nullptr);

		VK_CALL(vkBindBufferMemory(
			createInfo_.LD_->GetHandle(),
			GetHandle(),
			memory->GetHandle(),
			0),
			"Error while binding device memory to buffer.");

		memory_ = memory;
	}

	void Buffer::Allocate(){

		OS::Assert(memory_ == nullptr);

		const VkMemoryRequirements memoryRequirements = GetMemoryRequirements();
		const uint32_t memoryType = FindSuitableMemoryType(createInfo_.physicalDevice_, memoryRequirements.memoryTypeBits, createInfo_.properties_);

		DeviceMemory::CreateInfo memoryCreateInfo{
			.LD_ = createInfo_.LD_,
			.requirements_ = memoryRequirements,
			.memoryTypeIndex_ = memoryType
		};

		auto memory = std::make_shared<DeviceMemory>(memoryCreateInfo);

		BindMemory(memory);

		memory_ = memory;

	}

	void Buffer::Deallocate() {

		OS::Assert(memory_ != nullptr);

		memory_ = nullptr;

	}

	[[nodiscard]]
	bool Buffer::IsAllocated() const noexcept {
		return (memory_ != nullptr);
	}

	Buffer::~Buffer() {
		OS::Assert(!IsNullHandle());
		vkDestroyBuffer(
			createInfo_.LD_->GetHandle(),
			GetHandle(), nullptr);
		SetNullHandle();

	}

}