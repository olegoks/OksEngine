#include <Render.Vulkan.Driver.Buffer.hpp>

#include <Render.Vulkan.Driver.CommandBuffer.hpp>
#include <Render.Vulkan.Driver.DeviceMemory.hpp>

namespace Render::Vulkan
{

	//Buffer::Buffer(Buffer&& moveBuffer) : LD_{ nullptr }, buffer_{ VK_NULL_HANDLE } {

	//	std::swap(createInfo_, moveBuffer.createInfo_);
	//	std::swap(memory_, moveBuffer.memory_);

	//}

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
		VkCall(vkCreateBuffer(createInfo_.LD_->GetHandle(), &bufferInfo, nullptr, &buffer), 
			"Error while creating buffer.");

		SetHandle(buffer);
	}

	void Buffer::Fill(const void* data, Common::Size sizeInBytes) noexcept {
		OS::Assert(memory_ != nullptr);
		memory_->Fill(data, sizeInBytes);
	}

	void Buffer::CopyDataTo(std::shared_ptr<Buffer> bufferTo, std::shared_ptr<CommandPool> commandPool) {
		CopyDataTo(*bufferTo, commandPool);

	}

	void Buffer::CopyDataTo(Buffer& bufferTo, std::shared_ptr<CommandPool> commandPool) {
		DataCopy(*this, bufferTo, createInfo_.LD_, commandPool);
	}

	void Buffer::DataCopy(const std::shared_ptr<Buffer> bufferFrom, std::shared_ptr<Buffer> bufferTo, std::shared_ptr<LogicDevice> ld, std::shared_ptr<CommandPool> commandPool) {

		DataCopy(*bufferFrom, *bufferTo, ld, commandPool);

	}

	void Buffer::DataCopy(const Buffer& bufferFrom, Buffer& bufferTo, std::shared_ptr<LogicDevice> ld, std::shared_ptr<CommandPool> commandPool) {


		CommandBuffer::CreateInfo commandBufferCreateInfo;
		{
			commandBufferCreateInfo.LD_ = ld;
			commandBufferCreateInfo.commandPool_ = commandPool;
		}
		auto commandBuffer = std::make_shared<CommandBuffer>(commandBufferCreateInfo);

		commandBuffer->Begin();
		commandBuffer->Copy(bufferFrom, bufferTo);
		commandBuffer->End();

		commandBuffer->Submit(ld->GetGraphicsQueue());

		vkQueueWaitIdle(ld->GetGraphicsQueue());

	}


	[[nodiscard]]
	Common::Size Buffer::GetSizeInBytes() const noexcept { return createInfo_.size_; }

	void Buffer::BindMemory(std::shared_ptr<DeviceMemory> memory) {

		OS::Assert(memory_ == nullptr);

		VkCall(vkBindBufferMemory(
			createInfo_.LD_->GetHandle(),
			GetHandle(),
			memory->GetHandle(),
			0),
			"Error while binding device memory to buffer.");

		memory_ = memory;
	}

	void Buffer::Allocate(){

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

	Buffer::~Buffer() {
		OS::Assert(!IsNullHandle());
		vkDestroyBuffer(
			createInfo_.LD_->GetHandle(),
			GetHandle(), nullptr);
		SetNullHandle();

	}

}