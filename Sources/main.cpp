
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
    {
        dragonLore.AddPosition(0, 0, 0);
        dragonLore.AddBehaviour("TestObject.lua", "TestObject");
        glm::mat4 renderGeomTransform{ 1.0f };
        renderGeomTransform = glm::translate(renderGeomTransform, { 0, 0, 0 });
        dragonLore.AddImmutableRenderGeometry(renderGeomTransform, "dragon_lore.obj", "dragon_lore.mtl", "dragon_lore.bmp");
    }
    Entity grassBlock = engine.CreateEntity();
    {
        grassBlock.AddPosition(0, 0, 0);
        grassBlock.AddBehaviour("GrassBlock.lua", "GrassBlock");
        glm::mat4 renderGeomTransform{ 1.0f };
        renderGeomTransform = glm::translate(renderGeomTransform, { 0, 0, 0 });
        grassBlock.AddImmutableRenderGeometry(renderGeomTransform,
            /* Math::Matrix4x4f::GetIdentity() **//*Math::Matrix4x4f::GetTranslate({ 20, 0, 0 }*/
            "GrassBlock.obj",
            "GrassBlock.mtl",
            "GrassBlock.png");
        glm::mat4 rigidBodyTransform = glm::mat4{ 1.0f };
        rigidBodyTransform = glm::translate(rigidBodyTransform, { 0, 10.0, 0.f });
        grassBlock.AddRigidBody(rigidBodyTransform, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f);
        grassBlock.AddMapRigidBodyToRenderGeometry();
    }
    //Entity grassBlock2 = engine.CreateEntity();
    //{
    //    grassBlock2.AddPosition(0, 0, 0);
    //    grassBlock2.AddBehaviour("GrassBlock.lua", "GrassBlock");
    //    glm::mat4 renderGeomTransform;
    //    renderGeomTransform = glm::translate(renderGeomTransform, { 0, 0, 0 });
    //    grassBlock2.AddImmutableRenderGeometry(renderGeomTransform,
    //        /* Math::Matrix4x4f::GetIdentity() **//*Math::Matrix4x4f::GetTranslate({ 20, 0, 0 }*/
    //        "GrassBlock.obj",
    //        "GrassBlock.mtl",
    //        "GrassBlock.png");
    //    glm::mat4 rigidBodyTransform = glm::mat4{ 1.0f };
    //    rigidBodyTransform = glm::translate(rigidBodyTransform, { 0, 7.0, 0.f });
    //    grassBlock2.AddRigidBody(rigidBodyTransform, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f);
    //    grassBlock2.AddMapRigidBodyToRenderGeometry();
    //}
    Entity forset = engine.CreateEntity();
    forset.AddPosition(0, 0, 0);
    glm::mat4 axisTransform{1.0f};
    forset.AddImmutableRenderGeometry(
        axisTransform,
        "axes.obj",
        "axes.mtl",
        ""); 


    Entity camera = engine.CreateEntity();
    camera.AddPosition(5.f, 0.f, 0.f);
    camera.AddBehaviour("Camera.lua", "Camera");
    camera.AddCamera({ 5.f, 0.f, 0.f }, { -5.f, 0.f, 0.f }, { 0.f, -1.f, 0.f });

    engine.Run();

    return 0;
}