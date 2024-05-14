#pragma once

#include <list>
#include <vector>
#include <functional>
#include <unordered_set>

#include <Common.VersionedMap>;
#include <Common.Types>
#include <OS.Assert>

namespace Common {

	namespace DataStructures {

		template<class Type>
		class Graph final {
		public:

			class Node;

			class [[nodiscard]] Node final {
			public:

				using Id = VersionedMap<Node>::Id;

				Node(Type&& data) noexcept : data_{ std::move(data) } { }
				Node(const Type& data) noexcept : data_{ data } { }

				[[nodiscard]]
				bool HasLinkTo(Id nodeId) noexcept {
					return linksTo_.contains(nodeId);
				}

				[[nodiscard]]
				bool HasLinkFrom(Id nodeId) noexcept {
					return linksFrom_.contains(nodeId);
				}

				[[nodiscard]]
				Common::Size GetLinksToNumber() const noexcept { 
					return linksTo_.size();
				}

				[[nodiscard]]
				Common::Size GetLinksFromNumber() const noexcept {
					return linksFrom_.size();
				}

				void ForEachLinksTo(std::function<void(Id linkTo)> processNode) noexcept {
					for (Id linkTo : linksTo_) {
						processNode(linkTo);
					}
				}

				void ForEachLinksFrom(std::function<void(Id linkFrom)> processNode) noexcept {
					for (Id linkFrom : linksFrom_) {
						processNode(linkFrom);
					}
				}

				Node& operator=(const Node& copyNode) noexcept = delete;

				Type& GetValue() noexcept { return data_; }

				const Type& GetValue() const noexcept { const_cast<Node*>(this)->GetValue(); }

			private:

				void DeleteLinkTo(Id nodeId) noexcept {
					auto it = std::find(linksTo_.begin(), linksTo_.end(), nodeId);
					linksTo_.erase(it);
				}

				void DeleteLinkFrom(Id nodeId) noexcept {
					auto it = std::find(linksFrom_.begin(), linksFrom_.end(), nodeId);
					linksFrom_.erase(it);
				}

				void AddLinkTo(Id nodeId) noexcept {
					linksTo_.push_back(nodeId);
				}

				void AddLinkFrom(Id nodeId) noexcept {
					linksFrom_.push_back(nodeId);
				}

				friend class Graph;

			private:
				Type data_;
				std::vector<Id> linksTo_;
				std::vector<Id> linksFrom_;
			};

			Node::Id AddNode(Type&& data) noexcept {
				return nodes_.AddElement(std::move(data));
			}

			Node::Id AddNode(const Type& data) noexcept {
				return nodes_.AddElement(data);
			}

			void AddLinkFromTo(Node::Id fromNodeId, Node::Id toNodeId) {

				Node& fromNode = nodes_.GetElement(fromNodeId);
				fromNode.AddLinkTo(toNodeId);

				Node& toNode = nodes_.GetElement(toNodeId);
				toNode.AddLinkFrom(fromNodeId);
				
			}

			void DeleteNode(Node::Id nodeId) noexcept {
				Node& node = nodes_.GetElement(nodeId);
				node.ForEachLinksTo([this, nodeId](Node::Id linkTo) {
					Node& node = nodes_.GetElement(linkTo);
					OS::AssertMessage(node.HasLinkFrom(nodeId), "Node links error.");
					node.DeleteLinkFrom(nodeId);
				});
			}

			using ProcessNode = std::function<void(typename Node::Id nodeId, Node& node)>;

			void ForEachNode(ProcessNode&& processor) noexcept {
				nodes_.ForEachElement([&](Node::Id nodeId, Node& node) {
					processor(nodeId, node);
				});
			}

			Node& GetNode(Node::Id nodeId) noexcept {
				return nodes_.GetElement(nodeId);
			}

			void Clear() noexcept {
				nodes_.Clear();
			}

		private:
			VersionedMap<Node> nodes_;
		};

	}

	namespace DS = DataStructures;


}