#pragma once

#include <string>
#include <OksEngine.ECS.Component.hpp>
#include <OksEngine.ECS.System.hpp>
#include <Math.Matrix.hpp>
#include <RAL.Texture.hpp>


#include <Geometry.Model.hpp>
#define YAML_CPP_STATIC_DEFINE
#define YAML_CPP_API
#include <yaml-cpp/yaml.h>
#include <Geometry.Storage.hpp>
#include <Render/OksEngine.GeometryStorage.hpp>
#include <RAL.Driver.hpp>
#include <Resources/OksEngine.Resource.hpp>

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
				.Exclude<LoadMeshRequest>(),
				ECS::Entity::Filter{}.ExcludeAll() };
		}

		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<CreateLoadMeshRequest>().GetId();
		}
	};


	struct Mesh : public ECSComponent<Mesh> {
	public:
		Mesh() : ECSComponent{ nullptr } {

		}
		Mesh(/*const std::string& tag, */const Geom::Model2::Id& modelId) :
			ECSComponent{ nullptr }, modelId_{ modelId } {}
		Geom::Model2::Id modelId_;
		//ModelStorage::Model::Id model_;
		//Common::Id driverModelId_;
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

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override {
			auto* loadMeshRequest = world->GetComponent<LoadMeshRequest>(entityId);
			bool allResourcesLoaded = true;
			for (auto [resourceName, resourceEntityId] : loadMeshRequest->resourceEntityId_) {
				if (!world->IsComponentExist<Resource>(resourceEntityId)) {
					allResourcesLoaded = false;
				}
			}

			if (!allResourcesLoaded) {
				return;
			}

			Geom::Model2 model;

			LoadMeshRequest::Type meshType = loadMeshRequest->type_;
			if (meshType == LoadMeshRequest::Type::OBJ_MTL_TEXTURES) {

				ECS::Entity::Id objEntityId;
				std::string objName;
				ECS::Entity::Id mtlEntityId;
				std::string mtlName;
				for (auto& [name, entityId] : loadMeshRequest->resourceEntityId_) {
					if (name.find(".obj") != name.npos) {
						objName = name;
						objEntityId = entityId;
					}
					if (name.find(".mtl") != name.npos) {
						mtlName = name;
						mtlEntityId = entityId;
					}
				}
				OS::Assert(!objEntityId.IsInvalid() && !mtlEntityId.IsInvalid());
				auto* objResource = world->GetComponent<Resource>(objEntityId);
				auto* mtlResource = world->GetComponent<Resource>(mtlEntityId);
				OS::LogInfo("render", { "Processing model file {}...", objName.c_str() });
				model = Geom::ParseObjMtlModelBaked(
					objName, { objResource->resourceData_.c_str(), objResource->resourceData_.size() },
					mtlName, { mtlResource->resourceData_.c_str(), mtlResource->resourceData_.size() }
				);
				OS::LogInfo("render", { "File {} processed.", objName.c_str() });
				//auto model = Geom::ParseFbxModelBaked(
				//	objName, { objResource->resourceData_.c_str(), objResource->resourceData_.size() }
				////);
				//auto modelStorage = GetContext().GetModelStorage();
				//ModelStorage::Model::Id modelId = modelStorage->CreateModel(loadMeshRequest->name_);
				//ModelStorage::Model& storageModel = modelStorage->GetModel(modelId);
				//for (auto& mesh : model.meshes_) {
				//	ModelStorage::Model::MeshId storageMeshId = storageModel.AddMesh();
				//	ModelStorage::Model::Mesh& storageMesh = storageModel.GetMesh(storageMeshId);
				//	if (mesh.textureName_ != "") {
				//		if (!GetContext().GetTextureStorage()->IsExist(mesh.textureName_)) {
				//			const ECS::Entity::Id textureResourceEntityId = loadMeshRequest->resourceEntityId_[mesh.textureName_];
				//			auto* textureResource = world->GetComponent<Resource>(textureResourceEntityId);
				//			Geom::Texture texture = Geom::CreateTexture(textureResource->resourceData_.c_str(), textureResource->resourceData_.size());
				//			const Geom::Texture::Id textureId = GetContext().GetTextureStorage()->Add(mesh.textureName_, std::move(texture));
				//			storageMesh.textureStorageId_ = textureId;
				//		}
				//		storageMesh.textureStorageId_ = GetContext().GetTextureStorage()->GetIdByTag(mesh.textureName_);
				//	}
				//	Geom::Mesh::Id meshId = GetContext().GetMeshStorage()->Add(mesh.name_, std::move(mesh));
				//	storageMesh.meshId_ = meshId;
				//}

				//auto* geomFile = world->GetComponent<GeometryFile>(entityId);
				//world->CreateComponent<Mesh>(entityId, geomFile->mesh_["Name"].as<std::string>(), modelId);
			}
			else if (meshType == LoadMeshRequest::Type::FBX) {

				ECS::Entity::Id fbxEntityId;
				std::string fbxName;
				for (auto& [name, entityId] : loadMeshRequest->resourceEntityId_) {
					if (name.find(".fbx") != name.npos) {
						fbxName = name;
						fbxEntityId = entityId;
					}
				}
				OS::Assert(!fbxEntityId.IsInvalid());
				auto* objResource = world->GetComponent<Resource>(fbxEntityId);
				OS::LogInfo("render", { "Processing model file {}...", fbxName.c_str() });
				model = Geom::ParseFbxModelBaked(
					fbxName,
					{
						objResource->resourceData_.c_str(),
						objResource->resourceData_.size()
					});
				OS::LogInfo("render", { "File {} processed.", fbxName.c_str() });
			}

			auto modelStorage = GetContext().GetModelStorage();
			ModelStorage::Model::Id modelId = modelStorage->CreateModel(loadMeshRequest->name_);
			ModelStorage::Model& storageModel = modelStorage->GetModel(modelId);
			for (auto& mesh : model.meshes_) {
				ModelStorage::Model::MeshId storageMeshId = storageModel.AddMesh();
				ModelStorage::Model::Mesh& storageMesh = storageModel.GetMesh(storageMeshId);
				if (mesh.textureName_ != "") {
					if (!GetContext().GetTextureStorage()->IsExist(mesh.textureName_)) {
						const ECS::Entity::Id textureResourceEntityId = loadMeshRequest->resourceEntityId_[mesh.textureName_];
						auto* textureResource = world->GetComponent<Resource>(textureResourceEntityId);
						Geom::Texture texture = Geom::CreateTexture(textureResource->resourceData_.c_str(), textureResource->resourceData_.size());
						const Geom::Texture::Id textureId = GetContext().GetTextureStorage()->Add(mesh.textureName_, std::move(texture));
						storageMesh.textureStorageId_ = textureId;
					}
					storageMesh.textureStorageId_ = GetContext().GetTextureStorage()->GetIdByTag(mesh.textureName_);
				}
				Geom::Mesh::Id meshId = GetContext().GetMeshStorage()->Add(mesh.name_, std::move(mesh));
				storageMesh.meshId_ = meshId;
			}

			auto* geomFile = world->GetComponent<GeometryFile>(entityId);
			world->CreateComponent<Mesh>(entityId, modelId);
		}

		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override {
			return { ECS::Entity::Filter{}.Include<LoadMeshRequest>().Include<GeometryFile>().Exclude<Mesh>(), ECS::Entity::Filter{}.ExcludeAll() };
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


	struct DriverModel : public ECSComponent<DriverModel> {
	public:

		struct Mesh {
			Common::Id id_;
			std::vector<RAL::Driver::ShaderBinding::Data> shaderBindings_;
		};

		DriverModel() : ECSComponent{ nullptr } { }
		DriverModel(std::vector<Mesh>& driverMeshs) :
			ECSComponent{ nullptr },
			driverMeshs_{ driverMeshs } {}

		std::vector<Mesh> driverMeshs_;
	};


	class CreateDriverModel : public ECSSystem {
	public:

		CreateDriverModel(Context& context) noexcept : ECSSystem{ context } { }

	public:

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override;

		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override;

		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<CreateDriverModel>().GetId();
		}
	};





	class MapMeshTransform : public ECSSystem {
	public:

		MapMeshTransform(Context& context) noexcept : ECSSystem{ context } { }

	public:

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override;

		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override;

	private:
		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<MapMeshTransform>().GetId();
		}
	};

}
