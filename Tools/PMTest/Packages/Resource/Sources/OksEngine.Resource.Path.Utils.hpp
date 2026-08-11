#pragma once

#include <regex>
#include <filesystem>

namespace OksEngine::Resource {

	static const char* prefix_ = "resource:/";

	[[nodiscard]]
	static inline bool IsResourcePath(const std::string& str) {
		std::regex pattern(R"(^resource:\/([^\/]+\/)*[^\/]+\.[^\/]+$)");
		return std::regex_match(str, pattern);
	}
}