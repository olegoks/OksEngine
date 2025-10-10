#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <ranges>
#include <vulkan/vulkan.hpp>
#include <vulkan/vk_enum_string_helper.h>
#include <Math.hpp>
#include <Geometry.hpp>
#include <RAL.Common.hpp>
#include <OS.Assert.hpp>
#include <OS.Logger.hpp>
#include <pix3.h>
#include <Common.Identifier.hpp>

namespace Render::Vulkan {

#if !defined(NDEBUG) || defined(USE_ASSERTS)
#define SHADER_DEBUG_PRINTF
#endif

inline void VkCall(VkResult nativeAPICallResult, const char* message) noexcept {
	const bool success = (nativeAPICallResult == VK_SUCCESS);
	if (!success) {
		std::string errorMessage = string_VkResult(nativeAPICallResult);
		errorMessage += ". ";
		errorMessage += message;
		ASSERT_FAIL_MSG(errorMessage.c_str());
	}
}


#if !defined(NDEBUG) || defined(USE_ASSERTS)

#define VK_CALL(vulkanAPICallResult, message)\
	VkCall(vulkanAPICallResult, message)

#else 

#define VK_CALL(vulkanAPICallResult, message) (vulkanAPICallResult)

#endif

	using Vector3f = Geom::Vector3f;
	using Index16 = Geom::Index16;
	using Color4b = Geom::Color4b;
	using Color3f = Geom::Color3f;
	using Normal3f = Geom::Normal3f;

	class Vertex2ftc : public Geometry::Vertex2ftc {
	public:


		static VkVertexInputBindingDescription GetBindingDescription() {
			VkVertexInputBindingDescription bindingDescription{};

			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex2ftc);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions() {
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

			{
				VkVertexInputAttributeDescription attributeDescription;
				{
					attributeDescription.binding = 0;
					attributeDescription.location = 0;
					attributeDescription.format = VK_FORMAT_R32G32_SFLOAT;
					attributeDescription.offset = offsetof(Vertex2ftc, position_);
				}
				attributeDescriptions.push_back(attributeDescription);
			}

			{
				VkVertexInputAttributeDescription attributeDescription;
				{
					attributeDescription.binding = 0;
					attributeDescription.location = 1;
					attributeDescription.format = VK_FORMAT_R32G32_SFLOAT;
					attributeDescription.offset = offsetof(Vertex2ftc, uv_);
				}
				attributeDescriptions.push_back(attributeDescription);
			}

			{
				VkVertexInputAttributeDescription attributeDescription;
				{
					attributeDescription.binding = 0;
					attributeDescription.location = 2;
					attributeDescription.format = VK_FORMAT_R8G8B8A8_UNORM;////VK_FORMAT_R32G32B32A32_SFLOAT;
					attributeDescription.offset = offsetof(Vertex2ftc, color_);
				}
				attributeDescriptions.push_back(attributeDescription);
			}

			return attributeDescriptions;
		}

	private:
	};

	class Vertex3fnc : public Geometry::Vertex3fnc {
	public:

		Vertex3fnc(const Vector3f& position, const Normal3f normal, const Color3f& color) :
			Geom::Vertex3fnc{ position, normal, color }{}

		static VkVertexInputBindingDescription GetBindingDescription() {
			VkVertexInputBindingDescription bindingDescription{};

			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex3fnc);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions() {
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

			{
				VkVertexInputAttributeDescription attributeDescription;
				{
					attributeDescription.binding = 0;
					attributeDescription.location = 0;
					attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;//VK_FORMAT_R32G32_SFLOAT;
					attributeDescription.offset = offsetof(Vertex3fnc, position_);
				}
				attributeDescriptions.push_back(attributeDescription);
			}

			{
				VkVertexInputAttributeDescription attributeDescription;
				{
					attributeDescription.binding = 0;
					attributeDescription.location = 1;
					attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;//VK_FORMAT_R32G32B32_SFLOAT;
					attributeDescription.offset = offsetof(Vertex3fnc, normal_);
				}
				attributeDescriptions.push_back(attributeDescription);
			}

			{
				VkVertexInputAttributeDescription attributeDescription;
				{
					attributeDescription.binding = 0;
					attributeDescription.location = 2;
					attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;//VK_FORMAT_R32G32B32_SFLOAT;
					attributeDescription.offset = offsetof(Vertex3fnc, color_);
				}
				attributeDescriptions.push_back(attributeDescription);
			}

			return attributeDescriptions;
		}

	};

	class Vertex3fc : public Geometry::Vertex3fc {
	public:

		static VkVertexInputBindingDescription GetBindingDescription() {
			VkVertexInputBindingDescription bindingDescription{};

			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex3fc);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions() {
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

			{
				VkVertexInputAttributeDescription attributeDescription;
				{
					attributeDescription.binding = 0;
					attributeDescription.location = 0;
					attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
					attributeDescription.offset = offsetof(Vertex3fc, position_);
				}
				attributeDescriptions.push_back(attributeDescription);
			}

			{
				VkVertexInputAttributeDescription attributeDescription;
				{
					attributeDescription.binding = 0;
					attributeDescription.location = 1;
					attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
					attributeDescription.offset = offsetof(Vertex3fc, color_);
				}
				attributeDescriptions.push_back(attributeDescription);
			}

			return attributeDescriptions;
		}
	};

	class Vertex3fnt : public Geometry::Vertex3fnt {
	public:


		static VkVertexInputBindingDescription GetBindingDescription() {
			VkVertexInputBindingDescription bindingDescription{};

			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex3fnt);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions() {
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

			{
				VkVertexInputAttributeDescription attributeDescription;
				{
					attributeDescription.binding = 0;
					attributeDescription.location = 0;
					attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;//VK_FORMAT_R32G32_SFLOAT;
					attributeDescription.offset = offsetof(Vertex3fnt, position_);
				}
				attributeDescriptions.push_back(attributeDescription);
			}

			{
				VkVertexInputAttributeDescription attributeDescription;
				{
					attributeDescription.binding = 0;
					attributeDescription.location = 1;
					attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;//VK_FORMAT_R32G32B32_SFLOAT;
					attributeDescription.offset = offsetof(Vertex3fnt, normal_);
				}
				attributeDescriptions.push_back(attributeDescription);
			}

			{
				VkVertexInputAttributeDescription attributeDescription;
				{
					attributeDescription.binding = 0;
					attributeDescription.location = 2;
					attributeDescription.format = VK_FORMAT_R32G32_SFLOAT;
					attributeDescription.offset = offsetof(Vertex3fnt, uv_);
				}
				attributeDescriptions.push_back(attributeDescription);
			}

			return attributeDescriptions;
		}
	};

	class Vertex3fntbw : public Geometry::Vertex3fntbw {
	public:


		static VkVertexInputBindingDescription GetBindingDescription() {
			VkVertexInputBindingDescription bindingDescription{};

			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex3fntbw);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions() {
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

			{
				VkVertexInputAttributeDescription attributeDescription;
				{
					attributeDescription.binding = 0;
					attributeDescription.location = 0;
					attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;//VK_FORMAT_R32G32_SFLOAT;
					attributeDescription.offset = offsetof(Vertex3fntbw, position_);
				}
				attributeDescriptions.push_back(attributeDescription);
			}

			{
				VkVertexInputAttributeDescription attributeDescription;
				{
					attributeDescription.binding = 0;
					attributeDescription.location = 1;
					attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;//VK_FORMAT_R32G32B32_SFLOAT;
					attributeDescription.offset = offsetof(Vertex3fntbw, normal_);
				}
				attributeDescriptions.push_back(attributeDescription);
			}

			{
				VkVertexInputAttributeDescription attributeDescription;
				{
					attributeDescription.binding = 0;
					attributeDescription.location = 2;
					attributeDescription.format = VK_FORMAT_R32G32_SFLOAT;
					attributeDescription.offset = offsetof(Vertex3fntbw, uv_);
				}
				attributeDescriptions.push_back(attributeDescription);
			}

			{
				VkVertexInputAttributeDescription attributeDescription;
				{
					attributeDescription.binding = 0;
					attributeDescription.location = 3;
					attributeDescription.format = VK_FORMAT_R8G8B8A8_UINT;
					attributeDescription.offset = offsetof(Vertex3fntbw, boneIds_);
				}
				attributeDescriptions.push_back(attributeDescription);
			}

			{
				VkVertexInputAttributeDescription attributeDescription;
				{
					attributeDescription.binding = 0;
					attributeDescription.location = 4;
					attributeDescription.format = VK_FORMAT_R8G8B8A8_UINT;
					attributeDescription.offset = offsetof(Vertex3fntbw, weights_);
				}
				attributeDescriptions.push_back(attributeDescription);
			}

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


	inline PFN_vkSetDebugUtilsObjectNameEXT SetObjectName = nullptr;
	

	[[nodiscard]]
	static inline VkIndexType IndexSizeToVulkanType(Common::Size indexSize) {
		switch (indexSize) {
		case  2: {
			return VkIndexType::VK_INDEX_TYPE_UINT16;
			break;
		}
		case  4: {
			return VkIndexType::VK_INDEX_TYPE_UINT32;
			break;
		}
		default: {
			ASSERT_FAIL_MSG("Unsupported index type.");
			return VkIndexType::VK_INDEX_TYPE_MAX_ENUM;
		}
		};
	}


}
