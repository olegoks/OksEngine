
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
    
    Entity grassBlock = engine.CreateEntity();
    grassBlock.AddPosition(0, 0, 0);
    grassBlock.AddBehaviour("GrassBlock.lua", "GrassBlock");
    grassBlock.AddImmutableRenderGeometry(
        Math::Matrix4x4f::GetTranslate({ 0, 0, 0 }),
        "GrassBlock.obj",
        "GrassBlock.mtl",
        "GrassBlock.png");

    Entity camera = engine.CreateEntity();
    grassBlock.AddPosition(0, 0, 0);
    camera.AddBehaviour("Camera.lua", "Camera");
    camera.AddCamera({ 5.f, 0.f, 0.f }, { -5.f, 0.f, 0.f });

    engine.Run();

    return 0;
}