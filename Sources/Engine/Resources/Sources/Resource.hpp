#pragma once

#include <memory>
#include <filesystem>
#include <functional>
#include <vector>
#include <unordered_map>
#include <string>

#include <Memory.AllocationCallbacks.hpp>
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

		~Resource() noexcept {
			OS::AssertMessage(file_.use_count() == 1 || file_.use_count() == 0,
				{ "Attempt to release resource, but there are links to it. "
				"Try to delete all links to the file %s to release the resource.",
				path_.filename().string() });

		}

	private:
		const std::filesystem::path path_;
		std::shared_ptr<OS::File> file_ = nullptr;
	};

	//class ResourceManager {
	//public:
	//	
	//	ResourceManager() noexcept {

	//	}

	//	void Load() {

	//		auto processFile = [this](const std::filesystem::path& path) {
	//			LoadResource(path);
	//		};

	//		ProcessFiles(GetResourcesPath(), processFile);
	//	}

	//	void LoadResource(const std::filesystem::path& path) {
	//		auto resource = std::make_shared<Resource>(path);
	//		resource->Load();
	//		const auto resourcePath = resource->GetPath();
	//		const auto resourceFileName = resourcePath.filename().string();
	//		resources_.insert({ resourceFileName, resource });
	//	}

	//	//std::shared_ptr<Resource> GetResource(const Resource::Identifier& resourceId) {
	//	//	
	//	//	return resources_[resourceId];

	//	//}

	//	const std::filesystem::path& GetResourcesPath() const { return resourcesPath_; };

	//private:
	//	void ProcessFiles(const std::filesystem::path& path, const std::function<void(const std::filesystem::path&)>& callback);
	//private:
	//	std::unordered_map<std::string, std::shared_ptr<Resource>> resources_;
	//	const std::filesystem::path resourcesPath_;
	//};

	class ResourceSystem {
	private:

		struct ResourceInfo {
			std::string name_;
			Resource resource_;
		};

		using Graph = DS::Graph<ResourceInfo>;

	public:
		ResourceSystem() {
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
				Resource{ resourceFilePath }
			};

			const Graph::Node::Id newResourceNodeId = graph_.AddNode(std::move(resourceInfo));
			const Graph::Node::Id nodeId = GetNodeId(resourcePath);
			graph_.AddLinkFromTo(nodeId, newResourceNodeId);

		}
		
		void LoadResource(std::filesystem::path resourcePath) {
			ResourceInfo& resourceInfo = GetResourceInfo(resourcePath);
			resourceInfo.resource_.Load();
		}

	private:
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
				currentNode.GetValue().name_ == rootName_,
				{ "Root note must have name \"%s\"", rootName_ }
			);
			for (const std::filesystem::path path : nodePath) {
				const std::string resourceName = path.string();
				if (GetResourceInfo(currentNodeId).name_ != resourceName) {
					[[maybe_unused]] 
					bool found = false;
					currentNode.ForEachLinksTo([&](Graph::Node::Id nodeId) {
						const ResourceInfo& resourceInfo = GetResourceInfo(nodeId);
						if (resourceInfo.name_ == resourceName) {
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
			std::string findResourceName = (*(--nodePath.end())).string();
			OS::AssertMessage(
				GetResourceInfo(currentNodeId).name_ == findResourceName,
				{	"Attempt to find node with path that doesn't exist.\n"
					"Incorrect path:\n%s", nodePath.string().c_str()});
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