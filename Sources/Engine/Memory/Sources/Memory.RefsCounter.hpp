#pragma once

#include <functional>

#include <Common.hpp>
#include <OS.hpp>

namespace Memory {

	class RefsCounter {
	public:

		using Callback = std::function<void()>;

		RefsCounter(const RefsCounter& otherCopy) noexcept = delete;

		RefsCounter(Callback&& refAddedCallback, Callback&& noRefsCallback = []() noexcept {}) noexcept :
			refAddedCallback_{ refAddedCallback },
			noRefsCallback_{ noRefsCallback } {

		}

		void Add() noexcept { 
			++counter_;
			refAddedCallback_();
		}

		void Remove() noexcept { 
			OS::AssertMessage(GetValue() != 0, "Attempt to remove reference when value of counter is zero.");
			--counter_;
			if (GetValue() == 0) {
				noRefsCallback_();
			}
		}

		[[nodiscard]]
		Common::Size GetValue() const noexcept { return counter_; }

		RefsCounter& operator=(const RefsCounter& otherCopy) noexcept = delete;

	private:
		Common::Size counter_ = 0;
		Callback refAddedCallback_ = []() noexcept {};
		Callback noRefsCallback_ = []() noexcept {};
	};

}