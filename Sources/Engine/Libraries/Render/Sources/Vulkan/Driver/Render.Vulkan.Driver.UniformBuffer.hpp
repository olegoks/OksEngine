#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include <Common.Types.hpp>

#include <Render.Vulkan.Common.hpp>
#include <Render.Vulkan.Driver.Buffer.hpp>

namespace Render::Vulkan {
	class UniformBuffer : public Buffer {
	public:
		UniformBuffer(std::shared_ptr<PhysicalDevice> physicalDevice, std::shared_ptr<LogicDevice> logicDevice, Common::Size sizeInBytes) :
			Buffer{ Buffer::CreateInfo{ physicalDevice,
			logicDevice,
			sizeInBytes,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT } } {

		}
	};

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