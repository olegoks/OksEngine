
#include <OksEngine.Entity.hpp>
#include <OksEngine.Behaviour.hpp>
#include <OksEngine.RenderableGeometry.hpp>

namespace OksEngine {

	void Entity::AddImmutableRenderGeometry(const Math::Matrix4x4f& modelMatrix, std::string objName, std::string mtlName, std::string textureName) {
		world_->CreateComponent<ImmutableRenderGeometry>(
			GetId(),
			context_,
			modelMatrix,
			objName,
			mtlName,
			textureName);
	}

	void Entity::AddBehaviour(
		const char* scriptName,
		const char* objectName) {
		world_->CreateComponent<Behaviour>(
			GetId(),
			context_,
			GetId(),
			scriptName,
			objectName);
	}

	void Entity::AddPosition(int x, int y, int z) {
		world_->CreateComponent<Position>(GetId(), x, y, z);
	}

}