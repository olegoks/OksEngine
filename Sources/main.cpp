
#include <Engine.hpp>


int main(int argc, char** argv){

    using namespace OksEngine;

    OS::AssertMessage(argc > 0, "First parameter must be config file name.");

    CommandLineParameters commandLineParameters{ argc, argv };
    const Engine::CreateInfo engineCreateInfo{
        commandLineParameters
    };
    Engine engine{ engineCreateInfo };
    //Entity entity = engine.CreateEntity();
    //entity.AddPosition(0, 0, 0);
    //entity.AddBehaviour("TestObject.lua", "TestObject");

    Entity dragonLore = engine.CreateEntity();
    dragonLore.AddPosition(0, 0, 0);
    dragonLore.AddBehaviour("TestObject.lua", "TestObject");
    dragonLore.AddImmutableRenderGeometry(Math::Matrix4x4f::GetTranslate({ 0, 0, 0 }), "dragon_lore.obj", "dragon_lore.mtl", "dragon_lore.bmp");
    //debugCube.
    //debugCube.AddDebugRenderableGeometry();

    engine.Run();

    return 0;
}