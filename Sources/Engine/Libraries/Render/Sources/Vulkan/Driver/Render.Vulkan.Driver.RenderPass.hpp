#pragma once

#include <vulkan/vulkan.hpp>

#include <OS.Assert.hpp>

#include <Render.Vulkan.Driver.SwapChain.hpp>
#include <Render.Vulkan.Driver.LogicDevice.hpp>

namespace Render::Vulkan {


	class RenderPass : public Abstraction<VkRenderPass> {
	public:

		struct DepthTestInfo {
			VkFormat depthStencilBufferFormat_ = VkFormat::VK_FORMAT_UNDEFINED;
		};

		struct CreateInfo {
			std::string name_ = "No name render pass";
			std::shared_ptr<LogicDevice> LD_ = nullptr;
			VkFormat colorAttachmentFormat_ = VkFormat::VK_FORMAT_UNDEFINED;
			VkSampleCountFlagBits samplesCount_ = VkSampleCountFlagBits::VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
			std::shared_ptr<DepthTestInfo> depthTestInfo_ = nullptr;
		};

		struct Subpass {

			struct DepthTestInfo {
				VkFormat depthStencilBufferFormat_ = VkFormat::VK_FORMAT_UNDEFINED;
			};

			struct CreateInfo {
				VkFormat colorAttachmentFormat_ = VkFormat::VK_FORMAT_UNDEFINED;
				VkSampleCountFlagBits samplesCount_ = VkSampleCountFlagBits::VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
				std::shared_ptr<DepthTestInfo> depthTestInfo_ = nullptr;
			};

			const VkAttachmentDescription& GetColorAttachment() const noexcept {
				return colorAttachment_;
			}

			const VkAttachmentDescription& GetColorAttachmentResolve() const noexcept {
				return colorAttachmentResolve_;
			}

			const VkAttachmentDescription& GetDepthStencilAttachment() const noexcept {
				return depthBufferAttachment_;
			}

			VkAttachmentDescription colorAttachment_{ 0 };
			VkAttachmentReference colorAttachmentRef_{ 0 };

			VkAttachmentDescription depthBufferAttachment_{ 0 };
			VkAttachmentReference depthBufferAttachmentRef_{ 0 };

			VkAttachmentDescription colorAttachmentResolve_{ 0 };
			VkAttachmentReference colorAttachmentRefResolve_{ 0 };

			VkSubpassDependency dependency_{ 0 };
			VkSubpassDescription descriptor_{ 0 };

		};

		RenderPass(const CreateInfo& createInfo) noexcept : LD_{ createInfo.LD_ } {

			{
				Subpass::CreateInfo subpassCreateInfo{ };
				{
					subpassCreateInfo.colorAttachmentFormat_ = createInfo.colorAttachmentFormat_;
					if (createInfo.depthTestInfo_ != nullptr) {
						auto depthTestInfo = std::make_shared<Subpass::DepthTestInfo>();
						depthTestInfo->depthStencilBufferFormat_ = createInfo.depthTestInfo_->depthStencilBufferFormat_;
						subpassCreateInfo.depthTestInfo_ = depthTestInfo;
					}
					subpassCreateInfo.samplesCount_ = createInfo.samplesCount_;
				}
				//Subpass subpass{ subpassCreateInfo };

				//Multisample attachment 
				VkAttachmentDescription multisampleAttachment{
					.flags = 0,
					.format = createInfo.colorAttachmentFormat_,
					.samples = createInfo.samplesCount_,
					.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
					.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
					.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
					.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
					.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
				};
				VkAttachmentReference multisampleAttachmentRef{
					.attachment = 0,
					.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
				};

				//Depth attachment
				VkAttachmentDescription depthAttachment{
					.flags = 0,
					.format = createInfo.depthTestInfo_->depthStencilBufferFormat_,
					.samples = VK_SAMPLE_COUNT_1_BIT,//
					.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
					.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
					.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
					.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
					.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
				};
				VkAttachmentReference depthAttachmentRef{
					.attachment = 1,
					.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
				};



				//swap chain
				VkAttachmentDescription swapChainAttachment{
					.flags = 0,
					.format = createInfo.colorAttachmentFormat_,
					.samples = VK_SAMPLE_COUNT_1_BIT, //createInfo.samplesCount_,
					.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
					.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
					.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
					.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
					.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
				};
				VkAttachmentReference swapChainAttachmentRef{
					.attachment = 2,
					.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
				};

				//Rendered buffer
				VkAttachmentDescription renderedAttachment{
					.flags = 0,
					.format = createInfo.colorAttachmentFormat_,
					.samples = VK_SAMPLE_COUNT_1_BIT,
					.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
					.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
					.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
					.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
					.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL//VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
				};

				VkAttachmentReference renderedAttachmentRef{
					.attachment = 3,
					.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
				};

				VkAttachmentReference renderedAttachmentSubpass2Ref{
					.attachment = 3,
					.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				};

				VkSubpassDependency inExternalDependency{
					.srcSubpass = VK_SUBPASS_EXTERNAL,
					.dstSubpass = 0,
					.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					.dstStageMask = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
					.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					.dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
				};

				VkSubpassDependency postProcessSubpassDependency_{
					.srcSubpass = 0,
					.dstSubpass = 1,
					.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					.dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT
				};

				VkSubpassDependency outExternalDependency{
					.srcSubpass = 1,
					.dstSubpass = VK_SUBPASS_EXTERNAL,
					.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					.dstStageMask = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
					.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					.dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
				};


				VkSubpassDescription renderSubpassDesc{
					.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
					//.inputAttachmentCount = 1,
					//.pInputAttachments = &renderedAttachmentRef,
					.colorAttachmentCount = 1,
					.pColorAttachments = &renderedAttachmentRef,
					.pResolveAttachments = nullptr,
					.pDepthStencilAttachment = &depthAttachmentRef
				};


				VkSubpassDescription postProcessSubpassDesc{
					.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
					.inputAttachmentCount = 1,
					.pInputAttachments = &renderedAttachmentSubpass2Ref,
					.colorAttachmentCount = 1,
					.pColorAttachments = &multisampleAttachmentRef,
					.pResolveAttachments = &swapChainAttachmentRef,
					.pDepthStencilAttachment = nullptr// &depthAttachmentRef
				};

				VkSubpassDescription subpasses[2]{ renderSubpassDesc, postProcessSubpassDesc };


				std::vector<VkSubpassDependency> subpassesDependencies;
				{
					subpassesDependencies.push_back(inExternalDependency);
					subpassesDependencies.push_back(postProcessSubpassDependency_);
					subpassesDependencies.push_back(outExternalDependency);

				}

				std::vector<VkAttachmentDescription> attachments;
				{
					attachments.push_back(multisampleAttachment);
					attachments.push_back(depthAttachment);
					attachments.push_back(swapChainAttachment);
					attachments.push_back(renderedAttachment);
				}


				VkRenderPassCreateInfo renderPassInfo{};
				{
					renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
					renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
					renderPassInfo.pAttachments = attachments.data();
					renderPassInfo.subpassCount = 2;
					renderPassInfo.pSubpasses = &subpasses[0];
					renderPassInfo.dependencyCount = static_cast<Common::UInt32>(subpassesDependencies.size());
					renderPassInfo.pDependencies = subpassesDependencies.data();
				}
				VkRenderPass renderPass = VK_NULL_HANDLE;
				VkCall(vkCreateRenderPass(createInfo.LD_->GetHandle(), &renderPassInfo, nullptr, &renderPass),
					"Error while creating render pass.");

				VkDebugUtilsObjectNameInfoEXT nameInfo = {};
				nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
				nameInfo.objectType = VK_OBJECT_TYPE_RENDER_PASS; // Тип объекта,, VK_OBJECT_TYPE_BUFFER
				nameInfo.objectHandle = (uint64_t)renderPass;   // Объект, которому назначается имя
				nameInfo.pObjectName = createInfo.name_.c_str();      // Имя объекта
				SetObjectName(*createInfo.LD_, &nameInfo);

				SetHandle(renderPass);


			}

		}

		~RenderPass() noexcept {
			Destroy();
		}

	private:

		void Destroy() noexcept {
			ASSERT_FMSG(LD_ != nullptr, "Logic device is not initialized.");
			ASSERT_FMSG(GetHandle() != VK_NULL_HANDLE, "Attempt to destroy VK_NULL_HANDLE VkRenderPass.");
			vkDestroyRenderPass(LD_->GetHandle(), GetHandle(), nullptr);
		}

	private:
		std::shared_ptr<LogicDevice> LD_ = nullptr;
	};


	class RenderPass2 : public Abstraction<VkRenderPass> {
	public:

		struct AttachmentData {
			std::shared_ptr<Image> image_ = nullptr;
			std::shared_ptr<ImageView> imageView_ = nullptr;
		};

		struct Attachment {
			VkFormat                        format;
			VkSampleCountFlagBits           samples;
			VkAttachmentLoadOp              loadOp;
			VkAttachmentStoreOp             storeOp;
			VkImageLayout                   initialLayout;
			VkImageLayout                   finalLayout;
		};


		struct AttachmentRef {
			enum class Type {
				Input,
				Color,
				Depth,
				Resolve,
				Undefined
			};
			Common::UInt32 index_ = Common::Limits<Common::UInt32>::Max();
			Type type_ = Type::Undefined;
			VkImageLayout layout_;
		};

		struct Subpass {
			struct Dependency {
				uint32_t                srcSubpass_;
				VkPipelineStageFlags    srcStageMask_;
				VkAccessFlags           srcAccessMask_;
				uint32_t                dstSubpass_;
				VkPipelineStageFlags    dstStageMask_;
				VkAccessFlags           dstAccessMask_;
			};

			
			std::vector<VkAttachmentReference> inputAttachments_;
			std::vector<VkAttachmentReference> colorAttachments_;
			std::shared_ptr<VkAttachmentReference> resolveAttachment_ = nullptr;
			std::shared_ptr<VkAttachmentReference> depthStencilAttachment_ = nullptr;
		};

		struct CreateInfo {
			std::string name_ = "No name";
			std::shared_ptr<LogicDevice> LD_ = nullptr;
			std::vector<Attachment> attachments_;
			std::vector<Subpass> subpasses_;
			std::vector<Subpass::Dependency> dependencies_;
		};

		RenderPass2(const CreateInfo& createInfo) noexcept : LD_{ createInfo.LD_ } {

			std::vector<VkSubpassDescription> subpassDescs_;
			subpassDescs_.reserve(createInfo.subpasses_.size());
			
			for (const Subpass& subpass : createInfo.subpasses_) {
				VkSubpassDescription desc{
					.flags = 0,
					.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
					.inputAttachmentCount = static_cast<Common::UInt32>(subpass.inputAttachments_.size()),
					.pInputAttachments = subpass.inputAttachments_.data(),
					.colorAttachmentCount = static_cast<Common::UInt32>(subpass.colorAttachments_.size()),
					.pColorAttachments = subpass.colorAttachments_.data(),
					.pResolveAttachments = subpass.resolveAttachment_.get(),
					.pDepthStencilAttachment = subpass.depthStencilAttachment_.get()
				};
				subpassDescs_.push_back(desc);
			}

			std::vector<VkAttachmentDescription> attachments;
			for (const Attachment& attachment : createInfo.attachments_) {
				attachments.push_back(VkAttachmentDescription{
						.flags = 0,
						.format = attachment.format,
						.samples = attachment.samples,
						.loadOp = attachment.loadOp,
						.storeOp = attachment.storeOp,
						.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
						.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
						.initialLayout = attachment.initialLayout,
						.finalLayout = attachment.finalLayout
					});
			}

			std::vector<VkSubpassDependency> dependencies;
			for (const Subpass::Dependency& dependency : createInfo.dependencies_) {
				dependencies.push_back(VkSubpassDependency{
					.srcSubpass = dependency.srcSubpass_,
					.dstSubpass = dependency.dstSubpass_,
					.srcStageMask = dependency.srcStageMask_,
					.dstStageMask = dependency.dstStageMask_,
					.srcAccessMask = dependency.srcAccessMask_,
					.dstAccessMask = dependency.dstAccessMask_,
					});
			}


			VkRenderPassCreateInfo renderPassInfo{};
			{
				renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
				renderPassInfo.attachmentCount = static_cast<uint32_t>(createInfo.attachments_.size());
				renderPassInfo.pAttachments = attachments.data();
				renderPassInfo.subpassCount = static_cast<uint32_t>(subpassDescs_.size());
				renderPassInfo.pSubpasses = subpassDescs_.data();
				renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
				renderPassInfo.pDependencies = dependencies.data();
			}
			VkRenderPass renderPass = VK_NULL_HANDLE;
			VkCall(vkCreateRenderPass(createInfo.LD_->GetHandle(), &renderPassInfo, nullptr, &renderPass),
				"Error while creating render pass.");

			VkDebugUtilsObjectNameInfoEXT nameInfo{
				.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
				.objectType = VK_OBJECT_TYPE_RENDER_PASS,
				.objectHandle = (uint64_t)renderPass,
				.pObjectName = createInfo.name_.c_str()
			};
			SetObjectName(*createInfo.LD_, &nameInfo);

			SetHandle(renderPass);


		}

		~RenderPass2() noexcept {
			Destroy();
		}

	private:

		void Destroy() noexcept {
			ASSERT_FMSG(LD_ != nullptr, "Logic device is not initialized.");
			ASSERT_FMSG(GetHandle() != VK_NULL_HANDLE, "Attempt to destroy VK_NULL_HANDLE VkRenderPass.");
			vkDestroyRenderPass(LD_->GetHandle(), GetHandle(), nullptr);
		}

	private:
		std::shared_ptr<LogicDevice> LD_ = nullptr;
	};


	using RP = RenderPass2;
}