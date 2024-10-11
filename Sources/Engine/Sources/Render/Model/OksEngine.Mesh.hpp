#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	//struct Mesh : public ECSComponent<Mesh> {
	//public:
	//	Mesh() : ECSComponent{ nullptr } {

	//	}
	//	Mesh(const Geom::Model2::Id& modelId) :
	//		ECSComponent{ nullptr }, modelId_{ modelId } {}
	//	Geom::Model2::Id modelId_;

	//};

	//template<>
	//inline void Edit<Mesh>(Mesh* mesh) {
	//	ImGui::TextDisabled("Model id: \"%d\"", mesh->modelId_);
	//}

	struct Mesh2 : public ECSComponent<Mesh2> {
	public:
		Mesh2() : ECSComponent{ nullptr } {

		}
	};

	template<>
	inline void Edit<Mesh2>(Mesh2* renderMesh) {
	}


	struct Vertices : public ECSComponent<Vertices> {
	public:
		Vertices(const Geom::VertexCloud<Geom::Vertex3f>& vertices) 
			: ECSComponent{ nullptr },
			vertices_{ vertices } {

		}

		Geom::VertexCloud<Geom::Vertex3f>	vertices_;
	};


	template<>
	inline void Edit<Vertices>(Vertices* renderMesh) {
	}

	struct Normals : public ECSComponent<Normals> {
	public:
		Normals(const DS::Vector<Geom::Normal3f>& normals) 
			: ECSComponent{ nullptr },
			normals_{ normals } {

		}

		DS::Vector<Geom::Normal3f>	normals_;
	};


	template<>
	inline void Edit<Normals>(Normals* renderMesh) {
	}

	struct Colors : public ECSComponent<Colors> {
	public:
		Colors(const DS::Vector<Geom::Color3f>& colors) 
			: ECSComponent{ nullptr },
			colors_{ colors } {

		}

		DS::Vector<Geom::Color3f>	colors_;
	};


	template<>
	inline void Edit<Colors>(Colors* renderMesh) {
	}

	struct UVs : public ECSComponent<UVs> {
	public:
		UVs(const DS::Vector<Geom::UV2f>& uvs) 
			: ECSComponent{ nullptr },
			uvs_{ uvs } {

		}

		DS::Vector<Geom::UV2f>	uvs_;
	};


	template<>
	inline void Edit<UVs>(UVs* renderMesh) {
	}

	struct Indices : public ECSComponent<Indices> {
	public:
		Indices(const Geom::IndexBuffer<Geom::Index16>& indices) 
			: ECSComponent{ nullptr },
			indices_{ indices } {

		}

		Geom::IndexBuffer<Geom::Index16>	indices_;
	};


	template<>
	inline void Edit<Indices>(Indices* renderMesh) {
	}


}