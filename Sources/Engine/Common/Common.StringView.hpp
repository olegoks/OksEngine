#pragma once

#include <string_view>

#include <Common.Types.hpp>

namespace Common {

	class StringView : public std::string_view {
	public:

		StringView(const std::string& string) : std::string_view{ string.c_str()} {

		}

		StringView(std::string_view view);
		StringView();
		~StringView() = default;

		void SetLeftOffset(Common::Index leftOffset) {
			if (leftOffset_ + leftOffset >= GetFullLength()) {
				leftOffset_ = GetFullLength();
				return;
			}

			leftOffset_ += leftOffset;
		}

		void SetRightOffset(Common::Index rightOffset) {
			if (rightOffset_ + rightOffset >= GetFullLength()) {
				rightOffset_ = GetFullLength();
			}

			rightOffset_ += rightOffset;
		}

		StringView& operator+=(std::size_t leftOffset);
		StringView& operator-=(std::size_t rightOffset);

		std::string GetView() const;

		void SetView(Common::Index begin, Common::Size size) {
			SetLeftOffset(begin);
			if (size + GetLeftOffset() >= GetFullLength()) {
				SetRightOffset(0);
				return;
			}
			SetRightOffset(GetFullLength() - GetLeftOffset() - size);
		}

		[[nodiscard]]
		std::size_t GetLeftOffset() const {
			return leftOffset_;
		}

		[[nodiscard]]
		std::size_t GetRightOffset() const {
			return rightOffset_;
		}

		[[nodiscard]]
		std::size_t GetViewLength() const noexcept {
			return length() - leftOffset_ - rightOffset_;
		}

		[[nodiscard]]
		std::size_t GetFullLength() const noexcept {
			return length();
		}

		void OffsetViewRight(std::size_t viewOffset) {

			const std::size_t leftOffsetToEndLength = GetFullLength() - leftOffset_;

			if (viewOffset >= leftOffsetToEndLength) {
				leftOffset_ = GetFullLength();
				rightOffset_ = 0;
				return;
			}

			leftOffset_ += viewOffset;

			if (viewOffset >= rightOffset_) {
				rightOffset_ = 0;
				return;
			}

			rightOffset_ -= viewOffset;

		}


	private:
		std::size_t leftOffset_ = 0;
		std::size_t rightOffset_ = 0;
	};
}