#pragma once

#include <string>
#include <OksEngine.ECS.Component.hpp>
#include <OksEngine.ECS.System.hpp>
#include <Math.Matrix.hpp>
#include <RAL.Texture.hpp>


#include <Geometry.Model.hpp>
#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>
#include <Geometry.Storage.hpp>

//#include "OksEngine.Render.Subsystem.hpp"

namespace OksEngine {

	struct ImmutableRenderGeometry : public ECSComponent<ImmutableRenderGeometry> {
	public:
		ImmutableRenderGeometry();
		ImmutableRenderGeometry(const std::string& meshTag);

		std::string meshTag_;
		std::string shaderTag_;
	};


	template<>
	inline void Edit<ImmutableRenderGeometry>(ImmutableRenderGeometry* immutableRenderGeometry) {
		ImGui::TextDisabled("Mesh tag: \"%s\"", immutableRenderGeometry->meshTag_.c_str());
	}

	template<>
	inline void Add<ImmutableRenderGeometry>(ECS::World* world, ECS::Entity::Id id) {
		static char geom[100] = { "dragon_lore.geom" };
		if (ImGui::CollapsingHeader("Create info")) {
			ImGui::InputText("Geometry description file:", geom, sizeof(geom));
		}
		if (ImGui::Button("Add component")) {
			if (!world->IsComponentExist<ImmutableRenderGeometry>(id)) {
				world->CreateComponent<ImmutableRenderGeometry>(id, geom);
			}
		}

	}


	template<>
	inline void Bind<ImmutableRenderGeometry>(Lua::Context& context) {
		context.GetGlobalNamespace()
			.beginClass<ImmutableRenderGeometry>("ImmutableRenderGeometry")
			.addConstructor<void(*)(std::string)>()
			.endClass();
	}

	struct GeometryFile : public ECSComponent<GeometryFile> {
	public:
		enum class Type {
			OBJ,
			FBX,
			Undefined
		};

		GeometryFile() : ECSComponent{ nullptr } {

		}
		GeometryFile(const std::string& geomName, const std::string& yamlText/*, YAML::Node geomYaml*/) :
			ECSComponent{ nullptr },
			geomName_{ geomName },
			yamlText_{ yamlText },
			mesh_{ YAML::Load(yamlText_) } {
		}

		std::string geomName_;
		std::string yamlText_;
		YAML::Node mesh_;

	};


	template<>
	inline void Edit<GeometryFile>(GeometryFile* geometryFile) {
		ImGui::TextDisabled(".geom file name: \"%s\"", geometryFile->geomName_.c_str());
		ImGui::TextDisabled("%s", geometryFile->yamlText_.c_str());
	}

	template<>
	inline void Add<GeometryFile>(ECS::World* world, ECS::Entity::Id id) {

	}

	class LoadGeometryDescriptionFile : public ECSSystem {
	public:

		LoadGeometryDescriptionFile(Context& context) noexcept : ECSSystem{ context } {

		}

	public:

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override;

		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override {
			return { ECS::Entity::Filter{}.Include<ImmutableRenderGeometry>().Exclude<GeometryFile>(), ECS::Entity::Filter{}.ExcludeAll() };
		}

	private:
		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<LoadGeometryDescriptionFile>().GetId();
		}
	};


	struct Mesh : public ECSComponent<Mesh> {
	public:
		Mesh() : ECSComponent{ nullptr } {

		}
		Mesh(const std::string& tag, Geom::Mesh::Id meshId) :
			ECSComponent{ nullptr }, meshId_{ meshId }, tag_{ tag } {}

		Geom::Mesh::Id meshId_;
		std::string tag_;
		Common::Index driverModelId_ = Common::Limits<Common::Index>::Max();
	};


	template<>
	inline void Edit<Mesh>(Mesh* mesh) {
		ImGui::TextDisabled("Tag: \"%s\"", mesh->tag_.c_str());
		ImGui::TextDisabled("Id: %d", mesh->meshId_);

	}

	template<>
	inline void Add<Geom::Mesh>(ECS::World* world, ECS::Entity::Id id) {

	}


	class LoadMesh : public ECSSystem {
	public:

		LoadMesh(Context& context) noexcept : ECSSystem{ context } {

		}

	public:

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override;

		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override {
			return { ECS::Entity::Filter{}.Include<GeometryFile>().Exclude<Mesh>(), ECS::Entity::Filter{}.ExcludeAll() };
		}

	private:
		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<LoadMesh>().GetId();
		}
	};

	class RenderMesh : public ECSSystem {
	public:

		RenderMesh(Context& context) noexcept : ECSSystem{ context } {

		}

	public:

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId);

		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override;

	private:
		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<RenderMesh>().GetId();
		}
	};


}
