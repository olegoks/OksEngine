#pragma once

#include <DataStructures.Graph.hpp>
#include <Geometry.Shape.hpp>
#include <Geometry.AABB.hpp>

namespace Geometry {



	class SceneGraph {
	public:

		class Mesh {
		public:

			using Id = DS::Graph<Mesh>::Node::Id;
			glm::vec3 translate_{ 0.f, 0.f, 0.f };
			glm::quat rotate_;
			std::shared_ptr<Geom::Shape> shape_;
			AABB aabb_;
			OBB obb_;
		};

		SceneGraph() {
			//root node
			meshs_.AddNode(Mesh{});
		}

		

		void AddShape(std::shared_ptr<Geom::Shape> shape) {
			Mesh mesh{

			};
		}

	private:
		Mesh::Id rootMeshId_;
		DS::Graph<Mesh> meshs_;
	};

}