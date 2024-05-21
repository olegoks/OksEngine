#pragma once

#include <memory>
#include <filesystem>
#include <functional>
#include <vector>
#include <unordered_map>
#include <string>

#include <OS.hpp>
#include <Datastructures.Graph.hpp>

namespace Resource {

	class Resource {
	public:
		Resource(const std::filesystem::path& path) :
			path_ { path } {}

		void Load();

		const std::filesystem::path& GetPath() const { return path_; }

		std::shared_ptr<OS::File> GetFile() const { return file_; }

	private:
		const std::filesystem::path path_;
		std::shared_ptr<OS::File> file_ = nullptr;
	};

	class ResourceManager {
	public:
		
		ResourceManager() noexcept {

		}

		void Load() {

			auto processFile = [this](const std::filesystem::path& path) {
				LoadResource(path);
			};

			ProcessFiles(GetResourcesPath(), processFile);
		}

		void LoadResource(const std::filesystem::path& path) {
			auto resource = std::make_shared<Resource>(path);
			resource->Load();
			const auto resourcePath = resource->GetPath();
			const auto resourceFileName = resourcePath.filename().string();
			resources_.insert({ resourceFileName, resource });
		}

		//std::shared_ptr<Resource> GetResource(const Resource::Identifier& resourceId) {
		//	
		//	return resources_[resourceId];

		//}

		const std::filesystem::path& GetResourcesPath() const { return resourcesPath_; };

	private:
		void ProcessFiles(const std::filesystem::path& path, const std::function<void(const std::filesystem::path&)>& callback);
	private:
		std::unordered_map<std::string, std::shared_ptr<Resource>> resources_;
		const std::filesystem::path resourcesPath_;
	};

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
			
			auto rootNodeId = graph_.AddNode(std::move(resourceInfo));
			rootNodeId_ = rootNodeId;
		}

		void AddResource(
			std::string name,
			std::filesystem::path resourceFilePath,
			std::filesystem::path resourceDependencePath) {

			auto root = *resourceDependencePath.begin();

			ResourceInfo resourceInfo {
				name,
				Resource{ resourceFilePath }
			};

			Graph::Node::Id newResourceNodeId = graph_.AddNode(std::move(resourceInfo));

			if (root.string() == rootName_) {
				graph_.AddLinkFromTo(rootNodeId_, newResourceNodeId);
			} else {
				Graph::Node::Id nodeId = GetNodeId(resourceDependencePath);
				graph_.AddLinkFromTo(nodeId, newResourceNodeId);

			}
		}
	
	private:
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
				std::string resourceName = path.string();
				if (resourceName == rootName_) { continue; }

				currentNode.ForEachLinksTo([this, &resourceName, &currentNodeId](Graph::Node::Id nodeId) {
					Graph::Node& node = graph_.GetNode(nodeId);
						if (node.GetValue().name_ == resourceName) {
							currentNodeId = nodeId;
							return false;
						}
						return true;
					});
			}
			return currentNodeId;
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