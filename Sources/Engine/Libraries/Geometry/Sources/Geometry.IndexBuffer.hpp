#pragma once

#include <vector>

#include <Common.Types.hpp>
#include <Geometry.Index.hpp>

namespace Geometry {

	template<class IndexType = Index16>
	class IndexBuffer {
	public:

		void Add(IndexType index) noexcept {
			indices_.push_back(index);
		}

		void Add(const IndexType* indices, Common::Size indicesNumber) noexcept {
			for (Common::Index i = 0; i < indicesNumber; i++) {
				Add(indices[i]);
			}
		}

		void Add(const IndexBuffer& indices) noexcept {
			const Common::Size beginIndicesNumber = GetIndicesNumber();
			indices_.resize(GetIndicesNumber() + indices.GetIndicesNumber());
			std::memcpy(indices_.data() + beginIndicesNumber, indices.indices_.data(), indices.GetSizeInBytes());
		}

		void AddNextMesh(Common::Size currentVerticesNumber, const IndexBuffer& indices) {
			const Common::Size beginIndicesNumber = GetIndicesNumber();
			indices_.reserve(GetIndicesNumber() + indices.GetIndicesNumber());
			for (Common::Index i = 0; i < indices.GetIndicesNumber(); i++) {
				indices_.push_back(currentVerticesNumber + indices[i]);
			}
		}

		[[nodiscard]]
		Common::Size GetIndicesNumber() const noexcept { 
			return indices_.size();
		}

		[[nodiscard]]
		Common::Size GetSizeInBytes() const noexcept { 
			return GetIndicesNumber() * sizeof(IndexType);
		}

		[[nodiscard]]
		IndexType& operator[](Common::Index index) noexcept {
			return indices_[index];
		}

		[[nodiscard]]
		const IndexType& operator[](Common::Index index) const noexcept {
			return indices_[index];
		}

		void Reserve(Common::Size capacity) {
			indices_.reserve(capacity);
		}

		[[nodiscard]]
		const IndexType* GetData() const noexcept { return indices_.data(); }

	private:
		std::vector<IndexType> indices_;
	};

}