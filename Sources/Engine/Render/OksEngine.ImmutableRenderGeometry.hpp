#pragma once

#include <string>
#include <OksEngine.ECS.Component.hpp>
#include <OksEngine.ECS.System.hpp>
#include <Math.Matrix.hpp>
#include <RAL.Texture.hpp>
#include <Resources\OksEngine.Resource.Subsystem.hpp>
#include <Geometry.Model.hpp>
#include <yaml-cpp/yaml.h>
#include <Geometry.Storage.hpp>

namespace OksEngine {

	struct ImmutableRenderGeometry : public ECSComponent<ImmutableRenderGeometry> {
	public:
		ImmutableRenderGeometry() : ECSComponent{ nullptr }{

		}
		ImmutableRenderGeometry(const std::string& meshTag) :
			ECSComponent{ nullptr },
			meshTag_{ meshTag } {}

		std::string meshTag_;
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

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override {
			const auto* request = world->GetComponent<ImmutableRenderGeometry>(entityId);
			using namespace std::string_literals;
			const std::string& geomFilePath = "Root\\"s + request->meshTag_;
			auto taskId = GetContext().GetResourceSubsystem()->GetResource(Subsystem::Type::Engine, geomFilePath);
			ResourceSubsystem::Resource resource = GetContext().GetResourceSubsystem()->GetResource(Subsystem::Type::Engine, taskId);
			const std::string yamlText = resource.GetData<char>();
			world->CreateComponent<GeometryFile>(entityId, request->meshTag_, yamlText);
		}

		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override {
			return { ECS::Entity::Filter{}.Include<ImmutableRenderGeometry>().Exclude<GeometryFile>(), ECS::Entity::Filter{}.ExcludeAll()};
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

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override {
			const auto* geomFile = world->GetComponent<GeometryFile>(entityId);
			if (geomFile->mesh_["Obj"]) {
				const std::string objName = geomFile->mesh_["Obj"].as<std::string>();
				using namespace std::string_literals;
				const std::string& objFilePath = "Root\\"s + objName;
				auto taskId = GetContext().GetResourceSubsystem()->GetResource(Subsystem::Type::Engine, objFilePath);
				ResourceSubsystem::Resource resource = GetContext().GetResourceSubsystem()->GetResource(Subsystem::Type::Engine, taskId);
				auto mesh = Geom::ParseModelObj(resource.GetData<char>(), resource.GetSize());
				const Geom::Mesh::Id meshId = GetContext().GetGeomStorage()->Add(geomFile->mesh_["Name"].as<std::string>(), std::move(mesh));
				world->CreateComponent<Mesh>(entityId, geomFile->mesh_["Name"].as<std::string>(), meshId);
			}
		}

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

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override {
			auto* renderGeom = world->GetComponent<ImmutableRenderGeometry>(entityId);
			auto* mesh = world->GetComponent<Mesh>(entityId);
			//auto driver = GetContext().GetRenderSubsystem()->GetDriver();
			
			//driver->DrawIndexed()
		}

		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override {
			return { ECS::Entity::Filter{}.Include<Mesh>().Include<ImmutableRenderGeometry>(), ECS::Entity::Filter{}.ExcludeAll()};
		}

	private:
		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<RenderMesh>().GetId();
		}
	};

}
