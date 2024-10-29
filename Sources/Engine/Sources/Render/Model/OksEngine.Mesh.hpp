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


	struct Vertices3D : public ECSComponent<Vertices3D> {
	public:
		Vertices3D(const Geom::VertexCloud<Geom::Vertex3f>& vertices) 
			: ECSComponent{ nullptr },
			vertices_{ vertices } {

		}

		Vertices3D()
			: ECSComponent{ nullptr },
			vertices_{ } {

		}

		Geom::VertexCloud<Geom::Vertex3f>	vertices_;
	};


	template<>
	inline void Edit<Vertices3D>(Vertices3D* renderMesh) {
	}

	struct Vertices2D : public ECSComponent<Vertices2D> {
	public:
		Vertices2D(const Geom::VertexCloud<Geom::Vertex2f>& vertices)
			: ECSComponent{ nullptr },
			vertices_{ vertices } {

		}

		Vertices2D()
			: ECSComponent{ nullptr },
			vertices_{ } {

		}

		Geom::VertexCloud<Geom::Vertex2f>	vertices_;
	};


	template<>
	inline void Edit<Vertices2D>(Vertices2D* renderMesh) {
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
		Colors(const DS::Vector<Geom::Color4f>& colors) 
			: ECSComponent{ nullptr },
			colors_{ colors } {

		}

		Colors()
			: ECSComponent{ nullptr },
			colors_{ } {

		}

		DS::Vector<Geom::Color4f>	colors_;
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
		
		UVs()
			: ECSComponent{ nullptr },
			uvs_{ } {

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

		Indices()
			: ECSComponent{ nullptr },
			indices_{ } {

		}

		Geom::IndexBuffer<Geom::Index16>	indices_;
	};


	template<>
	inline void Edit<Indices>(Indices* renderMesh) {
	}


}