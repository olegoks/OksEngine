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
		const IndexType* GetData() const noexcept { return indices_.data(); }

	private:
		std::vector<IndexType> indices_;
	};

}