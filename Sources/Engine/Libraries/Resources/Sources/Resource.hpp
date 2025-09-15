#pragma once

#include <tuple>
#include <memory>
#include <filesystem>
#include <functional>
#include <vector>
#include <unordered_map>
#include <string>
#include <set>

#include <OS.Memory.AllocationCallbacks.hpp>
#include <OS.hpp>
#include <Datastructures.Graph.hpp>

namespace Resources {


	class ResourceData {
	public:

		template<class Type>
		 Type* GetData() {
			return reinterpret_cast<Type*>(data_.GetData());
		}

		[[nodiscard]]
		Common::Size GetSize() const noexcept { return data_.GetSize(); }

		ResourceData() = default;

		ResourceData(const Common::Byte* data, Common::Size size) noexcept {
			data_.Resize(size);
			std::memcpy(data_.GetData(), data, size);
		}

		ResourceData(ResourceData&& moveResource) noexcept : data_{ /*std::move(moveResource.data_)*/ } {
			std::swap(data_, moveResource.data_);
		}

		ResourceData& operator=(ResourceData&& moveResource) noexcept {
			if (this == &moveResource) {
				return *this;
			}

			data_ = std::move(moveResource.data_);

			return *this;
		}

		~ResourceData() {

		}

		DS::Vector<Common::Byte> data_;
	};


	class Resource {
	public:
		Resource(
			const std::filesystem::path& path,
			Memory::AllocationCallbacks allocationCallbacks = Memory::AllocationCallbacks{}) noexcept :
			path_{ path } {

			file_ = std::make_unique<OS::BinaryFile>(GetPath(), allocationCallbacks_);

		}

		void Load();
		void Unload();
		[[nodiscard]]
		bool IsLoaded() noexcept {
			return file_->IsLoaded();
		}

		[[nodiscard]]
		const std::filesystem::path& GetPath() const { return path_; }

		[[nodiscard]]
		std::shared_ptr<OS::File> GetFile() const { return file_; }

		Resource& operator=(const Resource& copyResource) noexcept = default;

		~Resource() noexcept { }

	private:
		Memory::AllocationCallbacks allocationCallbacks_;
		const std::filesystem::path path_;
		std::shared_ptr<OS::File> file_ = nullptr;
	};

	class ResourceSystem {
	private:

		class ResourceInfo {
		public:

			ResourceInfo(std::string name, const Resource& resource)noexcept :
				name_{ name },
				resource_{ resource } { }

			[[nodiscard]]
			std::string GetName() const noexcept {
				return name_;
			}

			std::filesystem::path GetOSPath() {
				return resource_.GetPath();
			}

			[[nodiscard]]
			Resource& GetResource() noexcept
			{
				return resource_;
			}

			[[nodiscard]]
			const Resource& GetResource() const noexcept
			{
				return resource_;
			}

		private:
			std::string name_;
			Resource resource_;
		};

		using Graph = DS::Graph<ResourceInfo>;

	public:

		struct CreateInfo {
			std::set<std::string> fileExtensions_;
		};

		ResourceSystem(const CreateInfo& createInfo, const Memory::AllocationCallbacks& allocationCallbacks = Memory::AllocationCallbacks{}) noexcept :
			ci_{ createInfo },
			allocationCallbacks_{
			allocationCallbacks
			} {
			ResourceInfo resourceInfo{
				"Root",
				Resource{ "" }
			};

			rootNodeId_ = graph_.AddNode(std::move(resourceInfo));
		}

		void AddResource(
			std::string name,
			std::filesystem::path resourceFilePath,
			std::filesystem::path resourcePath) {

			ResourceInfo resourceInfo{
				name,
				Resource{
					resourceFilePath,
					allocationCallbacks_
				}
			};


			const Graph::Node::Id newResourceNodeId = graph_.AddNode(std::move(resourceInfo));
			const Graph::Node::Id nodeId = GetNodeId(resourcePath);
			graph_.AddLinkFromTo(nodeId, newResourceNodeId);

		}

		Resources::ResourceData ForceGetResourceData(std::filesystem::path OSResourcePath) {
			Resources::Resource forceResource = ForceGetResource(OSResourcePath);
			//ASSERT_FMSG(forceResource.IsLoaded(), "Resource must be loaded at the moment.");
			auto file = std::dynamic_pointer_cast<OS::BinaryFile>(forceResource.GetFile());
			return { file->GetData(), file->GetSize() };
		}

		Resources::ResourceData GetResourceData(std::filesystem::path resourcePath) {
			//LoadResource(resourcePath);
			Resources::Resource resource = GetResource(resourcePath);
			auto binaryFile = std::dynamic_pointer_cast<OS::BinaryFile>(resource.GetFile());
			Resources::ResourceData engineResource(binaryFile->GetData(), binaryFile->GetSize());
			return engineResource;
		}

		std::filesystem::path GetFilesystemPath(std::filesystem::path resourcePath) {
			const Graph::Node::Id nodeId = GetNodeId(resourcePath);
			auto resourceInfo = GetResourceInfo(nodeId);
			return resourceInfo.GetOSPath();
		}

		bool IsFileExist(std::filesystem::path osPath) {
			return std::filesystem::exists(osPath);
		}

		bool CreateResource(const std::filesystem::path& osPath, Resources::ResourceData&& resourceData) {
			
			const std::string filename = osPath.filename().string();

			if (IsResourceExist(filename)) {
				
				auto file = std::dynamic_pointer_cast<OS::BinaryFile>(GetResource(filename).GetFile());
				file->Clear();
				OS::BinaryFile::WriteInfo writeInfo{
				.data_ = resourceData.GetData<Common::Byte>(),
				.size_ = resourceData.GetSize()
				};
				*file << writeInfo;
				file->Load();
			} else {

				auto file = std::make_shared<OS::BinaryFile>(osPath);
				file->Create();
				OS::BinaryFile::WriteInfo writeInfo{
					.data_ = resourceData.GetData<Common::Byte>(),
					.size_ = resourceData.GetSize()
				};
				*file << writeInfo;
				file->Close();

				AddResource(osPath.filename().string(), osPath, "Root");

			}
			return true;
		}

		void LoadResource(std::filesystem::path resourcePath) {
			//ASSERT_FMSG(IsNodeExist(resourcePath), "Resource node that required doesn't exist.");
			const Graph::Node::Id nodeId = GetNodeId(resourcePath);
			LoadResource(nodeId);
		}

		void UnloadResource(std::filesystem::path resourcePath) {
			const Graph::Node::Id nodeId = GetNodeId(resourcePath);
			UnloadResource(nodeId);
		}

		bool IsResourceExist(std::filesystem::path resourcePath) {
			ASSERT_FMSG(
				(*resourcePath.begin()).string() == rootName_,
				"Attempt to use incorrect resource path.");

			Graph::Node::Id currentNodeId = rootNodeId_;
			Graph::Node& currentNode = graph_.GetNode(currentNodeId);
			ASSERT_FMSG(
				currentNode.GetValue().GetName() == rootName_,
				"Root note must have name \"%s\"", rootName_
			);
			for (const std::filesystem::path path : resourcePath) {
				const std::string resourceName = path.string();
				if (GetResourceInfo(currentNodeId).GetName() != resourceName) {
					[[maybe_unused]]
					bool found = false;
					currentNode.ForEachLinksTo([&](Graph::Node::Id nodeId) {
						const ResourceInfo& resourceInfo = GetResourceInfo(nodeId);
						if (resourceInfo.GetName() == resourceName) {
							currentNodeId = nodeId;
							found = true;
							return false;
						}
						return true;
						});
					if (found) {
						return true;
					}
				}
			}
			return false;
		}

		bool IsResourceExist(const std::string& resourceName) {
			Graph::Node::Id currentNodeId = rootNodeId_;
			Graph::Node& currentNode = graph_.GetNode(currentNodeId);
			ASSERT_FMSG(
				currentNode.GetValue().GetName() == rootName_,
				"Root note must have name \"%s\"", rootName_
			);

			bool isExist = false;

			graph_.ForEachNode([&](Graph::Node::Id nodeId, Graph::Node& node) {

				if (node.GetValue().GetName() == resourceName) {
					isExist = true;
					return false;
				}

				return true;
				});
			return isExist;
		}

		[[nodiscard]]
		const Resource& GetResource(std::filesystem::path resourcePath) {
			const ResourceInfo& resourceInfo = GetResourceInfo(resourcePath);
			return resourceInfo.GetResource();
		}

		[[nodiscard]]
		const Resource& GetResource(std::string resourceName) {
			const ResourceInfo& resourceInfo = GetResourceInfo(resourceName);
			return resourceInfo.GetResource();
		}

		[[nodiscard]]
		Resource ForceGetResource(std::filesystem::path OSResourcePath) {
			Resource forceResource{ OSResourcePath };
			forceResource.Load();
			return forceResource;
		}

		void LoadAllResources() {
			LoadResource(rootName_);
		}

		void UnloadAllResources() {
			UnloadResource(rootName_);
		}

		~ResourceSystem() {
			UnloadAllResources();
		}

		void SetRoots(const std::vector<std::filesystem::path>& rootPaths) {
			rootFilesystemPaths_ = rootPaths;

			for (const auto& rootPath : rootPaths) {
				try {
					for (const auto& entry : std::filesystem::recursive_directory_iterator(rootPath)) {
						if (entry.path().extension().string() == ".gltf") {
							Common::BreakPointLine();
						}
						if (std::filesystem::is_regular_file(entry) && ci_.fileExtensions_.contains(entry.path().extension().string())) {
							AddResource(entry.path().filename().string(), entry.path(), "Root");
						}
					}
				}
				catch (const std::filesystem::filesystem_error& error) {
					OS::LogInfo("ResourceSystem", error.what());
				}
			}
		}

		using ProcessAddedResource = std::function<bool(const std::filesystem::path path)>;

		void ForEachAddedResource(ProcessAddedResource&& processAddedResource) {

			ProcessDependence processNode = [&](Graph::Node::Id nodeId)->bool {
				const std::filesystem::path nodePath = GetNodePath(nodeId);
				if (nodePath != rootName_) {
					return processAddedResource(nodePath);
				}
				return true;
				};

			ForEachDependenceNode(rootNodeId_, processNode);
		}

		std::filesystem::path GetOSResourcePath(const std::filesystem::path& resourcePath) {
			return GetResourceInfo(resourcePath).GetOSPath();
		}

	private:
		using ProcessDependence = std::function<bool(Graph::Node::Id)>;

		bool ForEachDependenceNode(Graph::Node::Id nodeId, ProcessDependence& processDependence) {
			Graph::Node& node = graph_.GetNode(nodeId);
			const bool stop = !processDependence(nodeId);
			if (stop) return false;
			node.ForEachLinksTo(
				[&](Graph::Node::Id nodeId)->bool
				{
					return ForEachDependenceNode(nodeId, processDependence);;
				});
			return true;
		}

		void LoadResource(const Graph::Node::Id nodeId) {

			ProcessDependence processNode = [this](Graph::Node::Id nodeId)->bool {
				ResourceInfo& resourceInfo = GetResourceInfo(nodeId);
				if (resourceInfo.GetName() != rootName_) {

					if (!resourceInfo.GetResource().IsLoaded()) {
						resourceInfo.GetResource().Load();
					}
					OS::LogInfo("Resources", { "Loaded resource {}", resourceInfo.GetName().c_str() });
				}
				return true;
				};

			ForEachDependenceNode(nodeId, processNode);
		}

		void UnloadResource(const Graph::Node::Id nodeId) {

			ProcessDependence processNode = [this](Graph::Node::Id nodeId)->bool {
				ResourceInfo& resourceInfo = GetResourceInfo(nodeId);
				if (resourceInfo.GetName() != rootName_) {
					if (resourceInfo.GetResource().IsLoaded()) {
						resourceInfo.GetResource().Unload();
					}
				}
				return true;
				};

			ForEachDependenceNode(nodeId, processNode);
		}

		[[nodiscard]]
		ResourceInfo& GetResourceInfo(const std::filesystem::path& resourcePath) {
			const Graph::Node::Id resourceNodeId = GetNodeId(resourcePath);
			Graph::Node& resourceNode = graph_.GetNode(resourceNodeId);
			return resourceNode.GetValue();
		}

		[[nodiscard]]
		ResourceInfo& GetResourceInfo(const std::string& resourceName) {
			Graph::Node::Id currentNodeId = rootNodeId_;
			Graph::Node& currentNode = graph_.GetNode(currentNodeId);
			ASSERT_FMSG(
				currentNode.GetValue().GetName() == rootName_,
				"Root note must have name \"%s\"", rootName_
			);

			Graph::NodeId foundNodeId = Graph::Node::invalidId_;

			graph_.ForEachNode([&](Graph::Node::Id nodeId, Graph::Node& node) {

				if (node.GetValue().GetName() == resourceName) {
					foundNodeId = nodeId;
					return false;
				}

				return true;
				});

#pragma region Assert
			ASSERT_FMSG(
				foundNodeId != Graph::Node::invalidId_,
				"There is not resource with such resource name:{}.",
					resourceName);
#pragma endregion
			
			Graph::Node& node = graph_.GetNode(foundNodeId);
			return node.GetValue();
		}

	public:
		[[nodiscard]]
		bool IsNodeExist(std::filesystem::path nodePath) {
			ASSERT_FMSG(
				(*nodePath.begin()).string() == rootName_,
				"Attempt to use incorrect resource path.");

			Graph::Node::Id currentNodeId = rootNodeId_;
			Graph::Node& currentNode = graph_.GetNode(currentNodeId);
			ASSERT_FMSG(
				currentNode.GetValue().GetName() == rootName_,
				"Root note must have name \"%s\"", rootName_
			);
			for (const std::filesystem::path path : nodePath) {
				const std::string resourceName = path.string();
				if (GetResourceInfo(currentNodeId).GetName() != resourceName) {
					[[maybe_unused]]
					bool found = false;
					currentNode.ForEachLinksTo([&](Graph::Node::Id nodeId) {
						const ResourceInfo& resourceInfo = GetResourceInfo(nodeId);
						if (resourceInfo.GetName() == resourceName) {
							currentNodeId = nodeId;
							found = true;
							return false;
						}
						return true;
						});
					return false;
				}
			}

			[[maybe_unused]]
			std::string findResourceName = (*(--nodePath.end())).string();
			ASSERT_FMSG(
				GetResourceInfo(currentNodeId).GetName() == findResourceName,
				"Attempt to find node with path that doesn't exist.\n"
					"Incorrect path:\n{}", nodePath.string().c_str());
			return true;
		}

	private:

		[[nodiscard]]
		Graph::Node::Id GetNodeId(std::filesystem::path nodePath) {
			ASSERT_FMSG(
				(*nodePath.begin()).string() == rootName_,
				"Attempt to use incorrect resource path.");

			Graph::Node::Id currentNodeId = rootNodeId_;
			Graph::Node& currentNode = graph_.GetNode(currentNodeId);
			ASSERT_FMSG(
				currentNode.GetValue().GetName() == rootName_,
				"Root note must have name \"%s\"", rootName_
			);
			for (const std::filesystem::path path : nodePath) {
				const std::string resourceName = path.string();
				if (GetResourceInfo(currentNodeId).GetName() != resourceName) {
					[[maybe_unused]]
					bool found = false;
					currentNode.ForEachLinksTo([&](Graph::Node::Id nodeId) {
						const ResourceInfo& resourceInfo = GetResourceInfo(nodeId);
						if (resourceInfo.GetName() == resourceName) {
							currentNodeId = nodeId;
							found = true;
							return false;
						}
						return true;
						});
					ASSERT_FMSG(found,
						"Attempt to use incorrect resource path.");
				}
			}

			[[maybe_unused]]
			std::string findResourceName = (*(--nodePath.end())).string();
			ASSERT_FMSG(
				GetResourceInfo(currentNodeId).GetName() == findResourceName,
				"Attempt to find node with path that doesn't exist.\n"
					"Incorrect path:\n%s", nodePath.string().c_str());
			return currentNodeId;
		}


		[[nodiscard]]
		std::filesystem::path GetNodePath(Graph::Node::Id nodeId) {

			Graph::Node::Id currentNodeId = nodeId;

			std::list<std::string> nodePathElements;

			Graph::Node& currentNode = graph_.GetNode(currentNodeId);
			nodePathElements.push_front(currentNode.GetValue().GetName());

			currentNode.ForEachLinksFrom([&](Graph::Node::Id nodeIdFrom) {
				Graph::Node& nodeFrom = graph_.GetNode(nodeIdFrom);
				nodePathElements.push_front(nodeFrom.GetValue().GetName());
				currentNodeId = nodeIdFrom;
				return false;
				});

			while (currentNodeId != rootNodeId_) {
				Graph::Node& currentNode = graph_.GetNode(currentNodeId);
				currentNode.ForEachLinksFrom([&](Graph::Node::Id nodeIdFrom) {
					Graph::Node& nodeFrom = graph_.GetNode(nodeIdFrom);
					nodePathElements.push_front(nodeFrom.GetValue().GetName());
					currentNodeId = nodeIdFrom;
					return false;
					});
			}

			std::filesystem::path nodePath;

			for (std::string element : nodePathElements) {
				nodePath /= element;
			}

			return nodePath;
		}


		[[nodiscard]]
		ResourceInfo& GetResourceInfo(Graph::Node::Id nodeId) {
			Graph::Node& node = graph_.GetNode(nodeId);
			return node.GetValue();
		}

	private:
		std::vector<std::filesystem::path> rootFilesystemPaths_;
		static inline std::string rootName_ = "Root";
		Graph::Node::Id rootNodeId_;
		Graph graph_;
		Memory::AllocationCallbacks allocationCallbacks_;
		CreateInfo ci_;
	};

}