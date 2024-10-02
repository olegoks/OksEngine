
#include <Render/OksEngine.ModelStorage.hpp>

namespace OksEngine {

	void OksEngine::ModelStorage::Model::ForEachMesh(ModelStorage::Model::ProccessMesh&& processMesh) {
		meshs_.ForEachData([&](Mesh& mesh) { return processMesh(mesh); });
	}

}
