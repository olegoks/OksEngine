#include <string_view>

#include <Common.StringView.hpp>

namespace Common {

	StringView::StringView(std::string_view view) { }

	StringView::StringView() { }

	StringView& StringView::operator+=(std::size_t leftOffset) {
		if (leftOffset_ + leftOffset >= GetFullLength()) {
			leftOffset_ = GetFullLength();
			return *this;
		}

		leftOffset_ += leftOffset;

		return *this;
	}

	StringView& StringView::operator-=(std::size_t rightOffset) {
		if (rightOffset_ + rightOffset >= GetFullLength()) {
			rightOffset_ = GetFullLength();
		}

		rightOffset_ += rightOffset;

		return *this;
	}

	std::string StringView::GetView() const {
		std::string view{ substr() };
		view = view.substr(leftOffset_, length() - rightOffset_ - leftOffset_);
		return view;
	}

}