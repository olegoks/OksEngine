#pragma once

#include <chrono>
#include <source_location>
#include <functional>

#include <Common.hpp>
#include <OS.Logger.hpp>
#include <OS.Clock.hpp>

namespace OS {

	using namespace std::chrono;

	class Profiler {
	public:
		
		struct Info {
			Clock::Duration executionTime_;
			std::string functionName_;
		};

		using FinishCallback = std::function<void(const Info&)>;

		Profiler(FinishCallback&& finishCallback, std::source_location sourceLocation = std::source_location::current()) noexcept :
			sourceLocation_{ sourceLocation },
			finishCallback_{ finishCallback } {
			start_ = Clock::Now();
		}

		~Profiler() noexcept {
			end_ = Clock::Now();
			const Clock::Duration delta = end_ - start_;
			Info info;
			info.executionTime_ = delta;
			info.functionName_ = sourceLocation_.function_name();
			finishCallback_(info);
		}

	private:
		std::source_location sourceLocation_;
		Clock::TimePoint start_;
		Clock::TimePoint end_;
		FinishCallback finishCallback_;
	};

}