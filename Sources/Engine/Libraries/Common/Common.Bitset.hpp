#pragma once

#include <bitset>

#include <Common.Types.hpp>

namespace Common {

	template<Common::UInt16 bitsNumber, Common::UInt8 childBitsetSize = sizeof(void*) * CHAR_BIT>
	class Bitset {
	public:

		using BitIndex = decltype(bitsNumber);
		constexpr static inline BitIndex invalidIndex_ = -1;

		void SetBit(BitIndex bitIndex) {
			childBitsets_[bitIndex / childBitsetSize].set(bitIndex % childBitsetSize);
		}

		void ResetBit(BitIndex bitIndex) {
			childBitsets_[bitIndex / childBitsetSize].reset(bitIndex % childBitsetSize);
		}

		[[nodiscard]]
		bool IsSet(BitIndex bitIndex) const noexcept {
			return childBitsets_[bitIndex / childBitsetSize].test(bitIndex % childBitsetSize);
		}

		[[nodiscard]]
		BitIndex FindFirstResettedBit() const noexcept {
			for (Common::Bitset<4096>::BitIndex i = 0; i < bitsNumber; i++) {
				if (!IsSet(i)) {
					return i;
				}
			}
			return invalidIndex_;
		}

		[[nodiscard]]
		constexpr decltype(bitsNumber) GetSize() const noexcept { return bitsNumber; }

	private:
		std::array<std::bitset<childBitsetSize>, bitsNumber / childBitsetSize> childBitsets_;
	};

}