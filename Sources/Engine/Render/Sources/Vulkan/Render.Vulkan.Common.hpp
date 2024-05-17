#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <ranges>
#include <windows.h>
#include <vulkan/vulkan.hpp>

#include <Math.hpp>
#include <OS.hpp>
#include <Geometry.hpp>

namespace Render::Vulkan {
	
	inline void VkCall(VkResult nativeAPICallResult, Common::Format format) noexcept {
		OS::AssertMessage(nativeAPICallResult == VK_SUCCESS, format);
	}

	class Vertex3fnc : public Vertex3fnc {
	public:

		static VkVertexInputBindingDescription GetBindingDescription() {
			VkVertexInputBindingDescription bindingDescription{};

			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex3fnc);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;//VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex3fnc, position_);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;//VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex3fnc, normal_);

			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;//VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(Vertex3fnc, color_);

			return attributeDescriptions;
		}
	};

	class Vertex3fc : public Vertex3fc {
	public:

		static VkVertexInputBindingDescription GetBindingDescription() {
			VkVertexInputBindingDescription bindingDescription{};

			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex3fc);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;//VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex3fc, position_);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;//VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex3fc, color_);

			return attributeDescriptions;
		}
	};

	class Vertex3fnt : public Vertex3fnt {
	public:


		static VkVertexInputBindingDescription GetBindingDescription() {
			VkVertexInputBindingDescription bindingDescription{};

			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex3fnt);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;//VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex3fnt, position_);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;//VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex3fnt, normal_);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 2;
			attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex3fnt, texel_);

			return attributeDescriptions;
		}
	};

	class Vertex3fn�t : public Vertex3fn�t {
	public:


		static VkVertexInputBindingDescription GetBindingDescription() {
			VkVertexInputBindingDescription bindingDescription{};

			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex3fn�t);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 4> GetAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};

			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex3fn�t, position_);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex3fn�t, normal_);

			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(Vertex3fn�t, color_);

			attributeDescriptions[3].binding = 0;
			attributeDescriptions[3].location = 3;
			attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[3].offset = offsetof(Vertex3fn�t, texel_);

			return attributeDescriptions;
		}
	};

	class Extension {
	public:

		Extension(const char* name) : name_{ name } { };

		[[nodiscard]]
		const char* GetRawName() const { return name_.c_str(); }

		[[nodiscard]]
		bool operator==(const Extension& extension) const {
			return name_ == extension.name_;
		}

	private:
		std::string name_;
	};

	class Extensions {
	public:

		Extensions() noexcept = default;
		Extensions(const std::initializer_list<Extension>& extensions) noexcept :
			extensions_{ extensions } {

		}

		void AddExtension(const Extension& extension) {
			extensions_.push_back(extension);
		}

		[[nodiscard]]
		bool Contains(const Extension& extension) const {
			const auto it = std::ranges::find(extensions_, extension);
			return it != extensions_.cend();
		}

		[[nodiscard]]
		Common::Size GetSize() const { return extensions_.size(); }
		[[nodiscard]]
		bool IsEmpty() const noexcept { return (GetSize() == 0); }

		std::vector<const char*> GetRawNames() const {
			std::vector<const char*> rawNames;
			rawNames.reserve(GetSize());
			for (const Extension& extension : extensions_) {
				rawNames.push_back(extension.GetRawName());
			}
			return rawNames;
		}

		using iterator = std::vector<Extension>::iterator;
		using const_iterator = std::vector<Extension>::const_iterator;

		iterator begin() {
			return extensions_.begin();
		}

		const_iterator begin() const {
			return extensions_.cbegin();
		}
		iterator end() {
			return extensions_.end();
		}
		const_iterator end() const {
			return extensions_.cend();
		}

	private:
		std::vector<Extension> extensions_;
	};

	struct UIRequiredInfo {
		Extensions requiredExtensions_;
		HWND windowHandle_ = NULL;
		HINSTANCE instanceHandle_ = NULL;
		Math::Vector2i frameBufferSize_{ 1920, 1080 };
	};

	class ValidationLayer {
	public:

		ValidationLayer(const ValidationLayer& copyValidationLayer) noexcept :
			name_{ copyValidationLayer.name_ } {

		}
		ValidationLayer(const char* name) : name_{ name } { };

		[[nodiscard]]
		const char* GetRawName() const { return name_.c_str(); }

		ValidationLayer& operator=(const ValidationLayer& copyValidationLayer) noexcept = default;

		[[nodiscard]]
		bool operator==(const ValidationLayer& layer) const {
			return name_ == layer.name_;
		}
	private:
		std::string name_;
	};

	class ValidationLayers {
	public:

		void AddLayer(const ValidationLayer& layer) {
			validationLayers_.push_back(layer);
		}

		[[nodiscard]]
		bool IsContains(const ValidationLayer& layer) const {
			const auto it = std::ranges::find(validationLayers_, layer);
			return it != validationLayers_.cend();
		}

		[[nodiscard]]
		Common::Size GetSize() const { return validationLayers_.size(); }
		[[nodiscard]]
		bool IsEmpty() const noexcept { return (GetSize() == 0); }

		std::vector<const char*> GetRawNames() const {
			std::vector<const char*> rawNames;
			rawNames.reserve(GetSize());
			for (const ValidationLayer& extension : validationLayers_) {
				rawNames.push_back(extension.GetRawName());
			}
			return rawNames;
		}

		using iterator = std::vector<ValidationLayer>::iterator;
		using const_iterator = std::vector<ValidationLayer>::const_iterator;

		iterator begin() {
			return validationLayers_.begin();
		}

		const_iterator begin() const {
			return validationLayers_.cbegin();
		}
		iterator end() {
			return validationLayers_.end();
		}
		const_iterator end() const {
			return validationLayers_.cend();
		}

	private:
		std::vector<ValidationLayer> validationLayers_;
	};


	struct QueueFamily {
		using Index = uint32_t;

		Index index_;
		VkQueueFamilyProperties properties_;


		[[nodiscard]]
		bool operator==(const QueueFamily& queueFamily) const noexcept {
			return index_ == queueFamily.index_;
		}
		//bool IsGraphics() const {
		//	return properties_.queueFlags & VK_QUEUE_GRAPHICS_BIT;
		//}

		//bool IsPresent(std::shared_ptr<PhysicalDevice> physicalDevice, std::shared_ptr<WindowSurface> windowSurface) const {

		//	VkBool32 presentSupport = false;
		//	vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice->GetNative(), index_, windowSurface->GetNative(), &presentSupport);

		//	return presentSupport;
		//}

	};

	class QueueFamilies {
	public:

		using Indices = std::vector<QueueFamily::Index>;

		QueueFamilies() noexcept = default;

		QueueFamilies(const std::initializer_list<QueueFamily>& queueFamilies) :
			queueFamilies_{ queueFamilies } {

		}

		void AddQueueFamily(const QueueFamily& queueFamily) {
			queueFamilies_.push_back(queueFamily);
		}

		[[nodiscard]]
		std::vector<QueueFamily::Index> GetIndices() const noexcept {
			std::vector<QueueFamily::Index> indices;
			for (const QueueFamily& queueFamily : queueFamilies_) {
				indices.push_back(queueFamily.index_);
			}
			return indices;
		}

		[[nodiscard]]
		bool IsContains(const QueueFamily& queueFamily) const {
			const auto it = std::ranges::find(queueFamilies_, queueFamily);
			return it != queueFamilies_.cend();
		}

		[[nodiscard]]
		Common::Size GetSize() const { return queueFamilies_.size(); }

		[[nodiscard]]
		bool IsEmpty() const { return (GetSize() == 0); }

		using iterator = std::vector<QueueFamily>::iterator;
		using const_iterator = const std::vector<QueueFamily>::const_iterator;

		iterator begin() {
			return queueFamilies_.begin();
		}

		const_iterator begin() const {
			return queueFamilies_.cbegin();
		}

		iterator end() {
			return queueFamilies_.end();
		}

		const_iterator end() const {
			return queueFamilies_.cend();
		}

	private:
		std::vector<QueueFamily> queueFamilies_;
	};



}