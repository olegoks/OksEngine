#include <filesystem>

#include <Lua.Context.hpp>

#include <OS.CommandLineParameters.hpp>
#include <Resource.hpp>

int main(int argc, char** argv) {
	OS::CommandLineParameters parameters{ argc, argv };

	const std::string_view workDir = parameters.GetValue("-workDir");
	std::vector<std::filesystem::path> workDirs{ workDir };
	Resources::ResourceSystem resourceSystem;
	resourceSystem.SetRoots(workDirs);

	return 0;
}