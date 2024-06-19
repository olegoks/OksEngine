#pragma once 

#include <Common.Types.hpp>
#include <Datastructures.Vector.hpp>
#include <RAL.Common.hpp>

namespace RAL {
	


	class Shape {
	public:
		
		template<class PolygonType>
		void AddPolygon(const PolygonType& polygon) {
			OS::NotImplemented();
		}

		template<>
		void AddPolygon<Polygon<Vertex3fnc>>(const Polygon<Vertex3fnc>& polygon) {
			for (const auto& vertex : polygon) {
				vertices_.PushBack(vertex.position_);
				normals_.PushBack(vertex.normal_);
				colors_.PushBack(vertex.color_);  
			}
			for (Common::Index vertexIndex = vertices_.GetSize() - 1 - 2;
				vertexIndex < vertices_.GetSize(); vertexIndex++) {
				OS::Assert(vertexIndex < Common::Limits<RAL::Index16>::Max());
				indices_.PushBack(static_cast<RAL::Index16>(vertexIndex));
			}
			OS::Assert(IsValid());
		}

		[[nodiscard]]
		RAL::VertexType GetVertexType() const noexcept {
			if (vertices_.GetSize() == normals_.GetSize() &&
				vertices_.GetSize() == colors_.GetSize()) {
				return RAL::VertexType::V3f_N3f_C3f;
			} else {
				OS::NotImplemented();
			}
		}

		template<class VertexType>
		void ForEachVertex(std::function<void(const VertexType& vertex)>&& process) const noexcept {
			OS::NotImplemented();
		}

		template<>
		void ForEachVertex<Vertex3fnc>(std::function<void(const Vertex3fnc& vertex)>&& process) const noexcept {
			for (Common::Index i = 0; i < vertices_.GetSize(); i++) {
				Vertex3fnc vertex{ vertices_[i].position_, normals_[i], colors_[i] };
				process(vertex);
			}
		}


	private:
		[[nodiscard]]
		bool IsValid() const noexcept {
			//OS::Assert(vertices_.GetSize() == normals_.GetSize());
		}
	private:
		DS::Vector<RAL::Vertex3f>		vertices_;
		DS::Vector<RAL::Index16>		indices_;
		DS::Vector<RAL::Normal3f>		normals_;
		DS::Vector<RAL::Color3f>		colors_;
		DS::Vector<RAL::UV2f>			uvs_;
		std::shared_ptr<RAL::Texture>	texture_ = nullptr;
	};

}