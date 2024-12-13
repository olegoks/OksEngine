#pragma once

#include <vector>

#include <Common.Types.hpp>
#include <OS.hpp>
#include <Math.Vector.hpp>
#include <Math.Matrix.hpp>
#include <Geometry.Vertex.hpp>

namespace Geometry {

	template<class VertexType = Vertex3f>
	class VertexCloud {
	public:

		VertexCloud() noexcept = default;

		VertexCloud(const VertexType* vertices, Common::Size verticesNumber) noexcept :
			vertices_{ vertices, vertices + verticesNumber } {

		}

		VertexCloud(const VertexCloud& copyVertexCloud) noexcept :
			vertices_{ copyVertexCloud.vertices_ } {

		}

		void Add(const VertexType* vertices, Common::Size verticesNumber) noexcept {

			OS::Assert(vertices != nullptr);

			const Common::Size offset = GetVerticesNumber();
			const Common::Size newSize = GetVerticesNumber() + verticesNumber;
			vertices_.resize(newSize);
			std::memcpy(vertices_.data() + offset, vertices, verticesNumber * sizeof(VertexType));

		}

		[[nodiscard]]
		const VertexType* GetData() const noexcept {
			return vertices_.data();
		}

		void Add(const VertexType& vertexType) noexcept {
			vertices_.push_back(vertexType);
		}

		void operator*(const Math::Matrix4x4f& transform) noexcept {
			for (VertexType& vertex: vertices_) {
				vertex *= transform;
			}
		}


		VertexCloud& operator=(const VertexCloud& moveVertexCloud) {

			if (this == &moveVertexCloud) {
				return *this;
			}

			vertices_ = moveVertexCloud.vertices_;

			return *this;

		}

		VertexCloud& operator=(VertexCloud&& moveVertexCloud) {
			
			if (this == &moveVertexCloud) {
				return *this;
			}

			vertices_ = std::move(moveVertexCloud.vertices_);

			return *this;

		}


		[[nodiscard]]
		const VertexType& operator[](Common::Index index) const noexcept {
			return vertices_[index];
		}


		void Reserve(Common::Size capacity) {
			vertices_.reserve(capacity);
		}

		using Iterator = std::vector<VertexType>::iterator;
		using ConstIterator = std::vector<VertexType>::const_iterator;

		Iterator begin() noexcept { return vertices_.begin(); }
		Iterator end() noexcept { return vertices_.end(); }

		ConstIterator begin() const noexcept{ return vertices_.cbegin(); }
		ConstIterator end() const noexcept { return vertices_.cend(); }

		[[nodiscard]]
		Common::Size GetVerticesNumber() const noexcept { return vertices_.size(); }

		[[nodiscard]]
		Common::Size GetSizeInBytes() const noexcept { return GetVerticesNumber() * sizeof(VertexType); }

	private:
		std::vector<VertexType> vertices_;
	};

}
