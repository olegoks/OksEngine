#pragma once 

#include <Datastructures.Graph.hpp>
#include <Geometry.Model.hpp>
#include <RAL.Texture.hpp>
#include <OksEngine.Storage.hpp>

namespace OksEngine {


	class ModelStorage {
	public:

		struct Model {

			using Id = Common::Id;

			struct Mesh {

				

				Geom::Mesh::Id meshId_;
				Geom::Texture::Id textureStorageId_;
				RAL::Texture::Id driverTextureId_;
				Common::Index driverMeshId_;
			};

			using MeshId = DS::Graph<Mesh>::NodeId;

			MeshId AddMesh() {
				DS::Graph<Mesh>::Node::Id meshNodeId = meshs_.AddNode(Mesh{});
				meshs_.AddLinkFromTo(rootNode_, meshNodeId);
				return meshNodeId;
			}

			[[nodiscard]]
			Mesh& GetMesh(MeshId meshId) {
				return meshs_.GetNode(meshId).GetValue();
			}

			using ProccessMesh = std::function<bool(Mesh&)>;

			void ForEachMesh(ProccessMesh&& processMesh);


			Model() {
				rootNode_ = meshs_.AddNode(Mesh{});
			}

			DS::Graph<Mesh>::Node::Id rootNode_;
			DS::Graph<Mesh> meshs_;
		};

		[[nodiscard]]
		Model::Id CreateModel(const std::string& tag) {
			const Model::Id modelId = models_.Add(tag, Model{});
			auto& modelNode = models_.Get(modelId);
			modelNode.meshs_.AddNode(Model::Mesh{});
			return modelId;
		}

		Model::Mesh& AddModelMesh(Common::Id modelId) {
			auto& modelNode = models_.Get(modelId);
			modelNode.meshs_.AddNode(Model::Mesh{});
		}

		[[nodiscard]]
		Model& GetModel(Model::Id modelId) {
			return models_.Get(modelId);
		}

		TaggedStorage<Model> models_;
	};
}