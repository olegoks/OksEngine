
#include <Engine.hpp>

#include <string>
int main(int argc, char** argv) {

	using namespace OksEngine;

	ASSERT_FMSG(argc > 0, "First parameter must be config file name.");

	const Engine::CreateInfo engineCreateInfo{
		argc, argv
	};

	Engine engine{ engineCreateInfo };

	engine.Run();

	return 0;
}