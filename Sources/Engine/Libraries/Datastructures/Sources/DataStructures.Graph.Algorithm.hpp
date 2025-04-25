#include <DataStructures.hpp>

#include <unordered_set>

namespace DataStructures {

	template<class Type>
	std::vector<Graph<Type>> FindIsolatedSubraphs(const Graph<Type>& graph) {
		std::vector<Graph<Type>> isolatedSubgraphs;
		std::unordered_set<Graph<Type>::Node::Id> processedNodes;

		graph.ForEachNode([&](Graph<Type>::NodeId nodeId, Graph<Type>::Node& node) {
			
			Graph<Type> subgraph;

			if (processedNodes.contains(nodeId)) {
				return true;
			}



			return true;
			});

	}


}