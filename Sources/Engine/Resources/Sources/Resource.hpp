#pragma once

#include <memory>
#include <filesystem>
#include <functional>
#include <vector>
#include <unordered_map>
#include <string>

#include <OS.Memory.AllocationCallbacks.hpp>
#include <OS.hpp>
#include <Datastructures.Graph.hpp>

namespace Resources {

	class Resource {
	public:
		Resource(
			const std::filesystem::path& path,
			Memory::AllocationCallbacks allocationCallbacks = Memory::AllocationCallbacks{}) noexcept :
			path_{ path } {}

		void Load();
		void Unload();

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

			[[nodiscard]]
			Resource& GetResource() noexcept
			{
				return resource_;
			}

		private:
			std::string name_;
			Resource resource_;
		};

		using Graph = DS::Graph<ResourceInfo>;

	public:

		ResourceSystem(const Memory::AllocationCallbacks& allocationCallbacks = Memory::AllocationCallbacks{}) noexcept :
			allocationCallbacks_{ allocationCallbacks } {
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

			const std::filesystem::path root = *resourcePath.begin();

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

		void LoadResource(std::filesystem::path resourcePath) {
			const Graph::Node::Id nodeId = GetNodeId(resourcePath);
			LoadResource(nodeId);
		}

		void UnloadResource(std::filesystem::path resourcePath) {
			const Graph::Node::Id nodeId = GetNodeId(resourcePath);
			UnloadResource(nodeId);
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
					resourceInfo.GetResource().Load();
				}
				return true;
			};

			ForEachDependenceNode(nodeId, processNode);
		}

		void UnloadResource(const Graph::Node::Id nodeId) {

			ProcessDependence processNode = [this](Graph::Node::Id nodeId)->bool {
				ResourceInfo& resourceInfo = GetResourceInfo(nodeId);
				if (resourceInfo.GetName() != rootName_) {
					resourceInfo.GetResource().Unload();
				}
				return true;
			};

			ForEachDependenceNode(nodeId, processNode);
		}

		[[nodiscard]]
		ResourceInfo& GetResourceInfo(std::filesystem::path resourcePath) {
			const Graph::Node::Id resourceNodeId = GetNodeId(resourcePath);
			Graph::Node& resourceNode = graph_.GetNode(resourceNodeId);
			return resourceNode.GetValue();
		}

		[[nodiscard]]
		Graph::Node::Id GetNodeId(std::filesystem::path nodePath) {
			OS::AssertMessage(
				(*nodePath.begin()).string() == rootName_,
				"Attempt to use incorrect resource path.");

			Graph::Node::Id currentNodeId = rootNodeId_;
			Graph::Node& currentNode = graph_.GetNode(currentNodeId);
			OS::AssertMessage(
				currentNode.GetValue().GetName() == rootName_,
				{ "Root note must have name \"%s\"", rootName_ }
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
					OS::AssertMessage(found,
						"Attempt to use incorrect resource path.");
				}
			}

			[[maybe_unsed]]
			std::string findResourceName = (*(--nodePath.end())).string();
			OS::AssertMessage(
				GetResourceInfo(currentNodeId).GetName() == findResourceName,
				{ "Attempt to find node with path that doesn't exist.\n"
					"Incorrect path:\n%s", nodePath.string().c_str() });
			return currentNodeId;
		}

		[[nodiscard]]
		ResourceInfo& GetResourceInfo(Graph::Node::Id nodeId) {
			Graph::Node& node = graph_.GetNode(nodeId);
			return node.GetValue();
		}

	private:
		static inline std::string rootName_ = "Root";
		Graph::Node::Id rootNodeId_;
		Graph graph_;
		Memory::AllocationCallbacks allocationCallbacks_;
	};


	//Job InitializeResourceSystemJob() {

	//	Job::CreateInfo createInfo;
	//	{
	//		createInfo.name_ = "InitializeResourceSystemJob";
	//		createInfo.function_ = [](Job::ExecutionContext& context) {
	//			auto resourceManager = std::make_shared<ResourceManager>("D:/Desktop/OksEngine/Resources/");
	//			resourceManager->Load();
	//			return resourceManager;
	//		};
	//	}

	//	Job initializeResourceManagerJob = Job{ createInfo };

	//	return initializeResourceManagerJob;
	//}

}