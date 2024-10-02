
#include <Render/OksEngine.LoadGeometryDescriptionFile.hpp>

#include <Render/OksEngine.LoadGeometryDescriptionFileRequest.hpp>
#include <Resources/OksEngine.LoadResourceRequest.hpp>
#include <Render/OksEngine.GeometryFile.hpp>
#include <Render/OksEngine.ImmutableRenderGeometry.hpp>

namespace OksEngine {

	void LoadGeometryDescriptionFile::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
		auto* request = world->GetComponent<LoadGeometryDescriptionFileRequest>(entityId);
		using namespace std::string_literals;
		const std::string& geomFilePath = "Root\\"s + request->geomFileName_;
		if (request->resourceTaskEntityId_.IsInvalid()) {
			request->resourceTaskEntityId_ = world->CreateEntity();
			world->CreateComponent<LoadResourceRequest>(request->resourceTaskEntityId_, request->geomFileName_);
		}
		else {
			if (world->IsComponentExist<Resource>(request->resourceTaskEntityId_)) {
				const auto* resource = world->GetComponent<Resource>(request->resourceTaskEntityId_);
				world->CreateComponent<GeometryFile>(entityId, request->geomFileName_, resource->resourceData_);
			}
		}
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> LoadGeometryDescriptionFile::GetFilter() const noexcept {
		return { ECS::Entity::Filter{}
			.Include<ImmutableRenderGeometry>()
			.Exclude<GeometryFile>()
			.Include<LoadGeometryDescriptionFileRequest>(), ECS::Entity::Filter{}.ExcludeAll() };
	}

}