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
#include <Render/OksEngine.GeometryStorage.hpp>

//#include "OksEngine.Render.Subsystem.hpp"

namespace OksEngine {

	struct ImmutableRenderGeometry : public ECSComponent<ImmutableRenderGeometry> {
	public:
		ImmutableRenderGeometry();
		ImmutableRenderGeometry(std::string meshTag) :
			ECSComponent{ nullptr }, meshTag_{ meshTag } {}

		std::string meshTag_;
		std::string shaderTag_;
	};


	template<>
	inline void Edit<ImmutableRenderGeometry>(ImmutableRenderGeometry* immutableRenderGeometry) {
		ImGui::TextDisabled("Mesh tag %s:", immutableRenderGeometry->meshTag_.c_str());		
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


	struct LoadGeometryDescriptionFileRequest : public ECSComponent<LoadGeometryDescriptionFileRequest> {
		ECS::Entity::Id resourceTaskEntityId_ = ECS::Entity::Id::invalid_;
		std::string geomFileName_ = "";
		LoadGeometryDescriptionFileRequest() :
			ECSComponent{ nullptr },
			resourceTaskEntityId_{ ECS::Entity::Id::invalid_ } {

		}

		LoadGeometryDescriptionFileRequest(const std::string& geometryFileName) :
			ECSComponent{ nullptr },
			resourceTaskEntityId_{ ECS::Entity::Id::invalid_ },
			geomFileName_{ geometryFileName } {

		}
	};


	template<>
	inline void Edit<LoadGeometryDescriptionFileRequest>(LoadGeometryDescriptionFileRequest* loadGeometryDescriptionFileRequest) {
		ImGui::TextDisabled("geom file: \"%s\"", loadGeometryDescriptionFileRequest->geomFileName_.c_str());
		ImGui::TextDisabled("Task entity ID: \"%d\"", loadGeometryDescriptionFileRequest->resourceTaskEntityId_);
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
		GeometryFile(const std::string geomName, const std::string yamlText) :
			ECSComponent{ nullptr },
			geomName_{ geomName },
			yamlText_{ yamlText },
			mesh_{  } {
 			mesh_ = YAML::Load(yamlText_);
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

	class CreateLoadGeometryDescriptionFileRequest : public ECSSystem {
	public:

		CreateLoadGeometryDescriptionFileRequest(Context& context) noexcept : ECSSystem{ context } { }

	public:

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override {
			auto* immutableRenderGeometry = world->GetComponent<ImmutableRenderGeometry>(entityId);
			world->CreateComponent<LoadGeometryDescriptionFileRequest>(entityId, immutableRenderGeometry->meshTag_);
		}

		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override {
			return { ECS::Entity::Filter{}
				.Include<ImmutableRenderGeometry>()
				.Exclude<GeometryFile>()
				.Exclude<LoadGeometryDescriptionFileRequest>(), ECS::Entity::Filter{}.ExcludeAll() };
		}

		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<CreateLoadGeometryDescriptionFileRequest>().GetId();
		}
	};

	class LoadGeometryDescriptionFile : public ECSSystem {
	public:

		LoadGeometryDescriptionFile(Context& context) noexcept : ECSSystem{ context } { }

	public:

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override;

		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override;

		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<LoadGeometryDescriptionFile>().GetId();
		}
	};

	struct LoadMeshRequest : public ECSComponent<LoadMeshRequest> {

		LoadMeshRequest() : ECSComponent{ nullptr } { }

		enum class Type {
			OBJ_MTL_TEXTURES,
			OBJ_MTL,
			OBJ,
			FBX,
			FBX_TEXTURES,
			Undefined
		};

		LoadMeshRequest(const std::string& name, std::map<std::string, ECS::Entity::Id> resourceEntityId, Type type) 
			: ECSComponent{ nullptr },
			resourceEntityId_{ resourceEntityId },
			type_{ type }, name_{ name } {}

		std::map<std::string, ECS::Entity::Id> resourceEntityId_;
		Type type_ = Type::Undefined;
		std::string name_;
	};

	class CreateLoadMeshRequest : public ECSSystem {
	public:

		CreateLoadMeshRequest(Context& context) noexcept : ECSSystem{ context } { }

	public:

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override;

		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override {
			return { ECS::Entity::Filter{}
				.Include<GeometryFile>()
				.Exclude<LoadMeshRequest>(), ECS::Entity::Filter{}.ExcludeAll() };
		}

		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<CreateLoadMeshRequest>().GetId();
		}
	};


	struct Mesh : public ECSComponent<Mesh> {
	public:
		Mesh() : ECSComponent{ nullptr } {

		}
		Mesh(const std::string& tag, const Geom::Model2::Id& modelId) :
			ECSComponent{ nullptr }, modelId_{ modelId }/*, tag_{ tag }*/ {}
		Geom::Model2::Id modelId_;
		//Geom::Mesh::Id meshId_;
		//std::string tag_;
		ModelStorage::Model::Id model_;
		Common::Index driverModelId_ = Common::Limits<Common::Index>::Max();
	};


	template<>
	inline void Edit<Mesh>(Mesh* mesh) {
		//ImGui::TextDisabled("Tag: \"%s\"", mesh->tag_.c_str());
		//ImGui::TextDisabled("Id: %d", mesh->meshId_);
	}


	class LoadMesh : public ECSSystem {
	public:

		LoadMesh(Context& context) noexcept : ECSSystem{ context } {

		}

	public:

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override;

		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override {
			return { ECS::Entity::Filter{}.Include<LoadMeshRequest>().Include<GeometryFile>().Exclude<Mesh>(), ECS::Entity::Filter{}.ExcludeAll()};
		}

	private:
		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<LoadMesh>().GetId();
		}
	};

	class RenderMesh : public ECSSystem {
	public:

		RenderMesh(Context& context) noexcept : ECSSystem{ context } { }

	public:

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId);

		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override;

	private:
		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<RenderMesh>().GetId();
		}
	};


}
