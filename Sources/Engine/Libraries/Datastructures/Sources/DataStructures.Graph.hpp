#pragma once

#include <vector>
#include <functional>

#include <Datastructures.VersionedMap.hpp>

namespace DataStructures {

	template<class Type>
	class Graph final {
	public:

		class [[nodiscard]] Node final {
		public:

			using Id = VersionedMap<int>::Id;

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
			void ForEachLinksTo(std::function<bool(Id linkTo)> processNode) const noexcept {
				for (Id linkTo : linksTo_) {
					const bool stop = !processNode(linkTo);
					if (stop) { break; }
				}
			}
			void ForEachLinksFrom(std::function<bool(Id linkFrom)> processNode) const noexcept {
				for (Id linkFrom : linksFrom_) {
					const bool stop = processNode(linkFrom);
					if (stop) { break; }
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

		using NodeId = Node::Id;

		Node::Id AddNode(Type&& data) noexcept;
		Node::Id AddNode(const Type& data) noexcept;
		void AddLinkFromTo(Node::Id fromNodeId, Node::Id toNodeId);
		void DeleteNode(Node::Id nodeId) noexcept;
		
		using ProcessNode = std::function<bool(NodeId nodeId, Node& node)>;
		void ForEachNode(ProcessNode&& processor) noexcept;
		
		using ProcessData = std::function<bool(Type&)>;
		void ForEachData(ProcessData&& processor) noexcept;

		Node& GetNode(Node::Id nodeId) noexcept;
		void Clear() noexcept;
	private:

		DS::VersionedMap<Node> nodes_;
	};


	//template<class Type>
	//struct Graph<Type>::Node::Id {
	//	VersionedMap<Node>::Id id;
	//};

	template<class Type>
	Graph<Type>::Node::Id Graph<Type>::AddNode(Type&& data) noexcept {
		return nodes_.AddElement(std::move(data));
	}

	template<class Type>
	Graph<Type>::Node::Id Graph<Type>::AddNode(const Type& data) noexcept {
		return nodes_.AddElement(data);
	}

	template<class Type>
	void Graph<Type>::AddLinkFromTo(Node::Id fromNodeId, Node::Id toNodeId) {

		Node& fromNode = nodes_.GetElement(fromNodeId);
		fromNode.AddLinkTo(toNodeId);

		Node& toNode = nodes_.GetElement(toNodeId);
		toNode.AddLinkFrom(fromNodeId);

	}

	template<class Type>
	void Graph<Type>::DeleteNode(Node::Id nodeId) noexcept {
		Node& node = nodes_.GetElement(nodeId);
		node.ForEachLinksTo([this, nodeId](Node::Id linkTo) {
			Node& node = nodes_.GetElement(linkTo);
			OS::AssertMessage(node.HasLinkFrom(nodeId), "Node links error.");
			node.DeleteLinkFrom(nodeId);
			});
	}

	template<class Type>
	void Graph<Type>::ForEachNode(ProcessNode&& processor) noexcept {
		nodes_.ForEachElement([&](Node::Id nodeId, Node& node) {
			return processor(nodeId, node);
			});
	}

	template<class Type>
	void Graph<Type>::ForEachData(ProcessData&& processor) noexcept {
		nodes_.ForEachElement([&](Node::Id nodeId, Node& node) {
			return processor(node.GetValue());
			});
	}

	template<class Type>
	Graph<Type>::Node& Graph<Type>::GetNode(Node::Id nodeId) noexcept {
		return nodes_.GetElement(nodeId);
	}

	template<class Type>
	void Graph<Type>::Clear() noexcept {
		nodes_.Clear();
	}

}

namespace DS = DataStructures;
