
#include <Engine.hpp>


int main(int argc, char** argv){

    using namespace OksEngine;

    OS::AssertMessage(argc > 0, "First parameter must be config file name.");

    CommandLineParameters commandLineParameters{ argc, argv };
    const Engine::CreateInfo engineCreateInfo{
        commandLineParameters
    };
    Engine engine{ engineCreateInfo };
    Entity entity = engine.CreateEntity();
    entity.AddPosition(0, 0, 0);
    entity.AddBehaviour("../../Sources/Scripts/", "TestObject.lua", "TestObject");

    engine.Run();

    return 0;
}