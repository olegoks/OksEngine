
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

    //Entity dragonLore = engine.CreateEntity();
    //{
    //    dragonLore.AddPosition(0, 0, 0);
    //    dragonLore.AddBehaviour("TestObject.lua", "TestObject");
    //    glm::mat4 renderGeomTransform{ 1.0f };
    //    renderGeomTransform = glm::translate(renderGeomTransform, { 0, 0, 0 });
    //    dragonLore.AddImmutableRenderGeometry(renderGeomTransform, "dragon_lore.obj", "dragon_lore.mtl", "dragon_lore.bmp");
    //    glm::mat4 rigidBodyTransform = glm::mat4{ 1.0f };
    //    rigidBodyTransform = glm::translate(rigidBodyTransform, { 0, 10.0, 0.f });
    //    dragonLore.AddRigidBody(rigidBodyTransform, 100.f, 0.5f, 0.5f, 0.5f, 2.f, 0.5f, 0.5f);
    //    dragonLore.AddMapRigidBodyToRenderGeometry();
    //}

    /*Entity flashLight = engine.CreateEntity();
    {
        flashLight.AddPosition(0, 0, 0);
        glm::mat4 renderGeomTransform{ 1.0f };
        renderGeomTransform = glm::translate(renderGeomTransform, { 0, 0, 0 });
        flashLight.AddImmutableRenderGeometry();
    }*/


    Entity character = engine.CreateEntity();
    {
        glm::mat4 capsuleTransform = glm::mat4{ 1.0f };
        capsuleTransform = glm::translate(capsuleTransform, { 5.f, 1.0, 0.f });

        character.AddPosition(5.f, 0.f, 0.f);
        character.AddBehaviour("Character.lua", "Character");
        character.AddRigidBodyCapsule(
            capsuleTransform,
            80.f,
            1.f,
            0.0f,
            0.5f,
            0.5f,
            0.f,
            0.5f,
            1.f);
        //character.AddBehaviour("Camera.lua", "Camera");
        character.AddCamera({ 0.f, 0.f, -5.f }, { 0.f, 0.f, 5.f }, { 0.f, 1.f, 0.f });
        character.AddAttachedCamera(glm::mat4{ 1.f });
    }

    //Entity grassBlock = engine.CreateEntity();
    //{
    //    grassBlock.AddPosition(0, 0, 0);
    //    grassBlock.AddBehaviour("GrassBlock.lua", "GrassBlock");
    //    glm::mat4 renderGeomTransform{ 1.0f };
    //    renderGeomTransform = glm::translate(renderGeomTransform, { 0, 0, 0 });
    //    grassBlock.AddImmutableRenderGeometry(renderGeomTransform,
    //        "yellowCube.obj",
    //        "yellowCube.mtl",
    //        "");
    //    glm::mat4 rigidBodyTransform = glm::mat4{ 1.0f };
    //    rigidBodyTransform = glm::translate(rigidBodyTransform, { 0, 100.0, 0.f });
    //    grassBlock.AddRigidBodyBox(
    //        rigidBodyTransform,
    //        10.f,
    //        1.f,
    //        1.f,
    //        0.5f,
    //        0.5f,
    //        0.5f,
    //        0.5f,
    //        0.5f, 
    //        0.5f);
    //    grassBlock.AddMapRigidBodyToRenderGeometry();
    //}

    //Entity grassPlane = engine.CreateEntity();
    //{
    //	grassPlane.AddPosition(0, 0, 0);
    //	glm::mat4 renderGeomTransform{ 1.0f };
    //	renderGeomTransform = glm::translate(renderGeomTransform, { 0.f, 0.f, 0.f });
    //	grassPlane.AddImmutableRenderGeometry(
    //        renderGeomTransform,
    //		"GrassPlane.obj",
    //		"GrassPlane.mtl", 
    //		"Grass.jpg");
    //}

    //Common::UInt64 size = 3;
    //for (Common::UInt64 i = 0; i < size; i++) {
    //	for (Common::UInt64 j = 0; j < size - i; j++) {
    //		glm::vec3 localTm(glm::vec3(float(j * 2) - float(size - i) + 0.1, float(i * 2 + 1) + 1.0, 0));
    //		localTm *= 0.5;
    //		Entity grassBlock = engine.CreateEntity();
    //		grassBlock.AddPosition(0, 0, 0);
    //		//grassBlock.AddBehaviour("GrassBlock.lua", "GrassBlock");
    //		glm::mat4 renderGeomTransform{ 1.0f };
    //		renderGeomTransform = glm::translate(renderGeomTransform, localTm);
    //		grassBlock.AddImmutableRenderGeometry(
    //            renderGeomTransform,
    //			"yellowCube.obj",
    //			"yellowCube.mtl",
    //			"");
    //		glm::mat4 rigidBodyTransform = glm::mat4{ 1.0f };
    //		rigidBodyTransform = glm::translate(rigidBodyTransform, localTm);
    //		grassBlock.AddRigidBodyBox(
    //            rigidBodyTransform,
    //            1.f,
    //            1.f,
    //            1.f,
    //            0.5f,
    //            0.5f,
    //            0.5f,
    //            0.5f,
    //            0.5f,
    //            0.5f);
    //		grassBlock.AddMapRigidBodyToRenderGeometry();
    //	}
    //}
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
   /* Entity forset = engine.CreateEntity();
    forset.AddPosition(0, 0, 0);
    glm::mat4 axisTransform{1.0f};
    forset.AddImmutableRenderGeometry(
        axisTransform,
        "axes.obj",
        "axes.mtl",
        ""); */


    engine.Run();

    return 0;
}