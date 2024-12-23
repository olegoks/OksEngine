
#include <Engine.hpp>

struct ImGUIPosition2D : public OksEngine::Position {};

int main(int argc, char** argv) {

	using namespace OksEngine;

	//OS::Assert(ImGUIPosition2D::GetTypeId() != OksEngine::Position::GetTypeId());

	OS::AssertMessage(argc > 0, "First parameter must be config file name.");

	CommandLineParameters commandLineParameters{ argc, argv };
	const Engine::CreateInfo engineCreateInfo{
		commandLineParameters
	};

	Engine engine{ engineCreateInfo };

	

	Entity camera = engine.CreateEntity();
	camera.AddCamera({ -100, 10, 0 }, { 0, 1, 0, }, true);
	camera.AddPosition(50, 20, 0);
	camera.AddDirection(-20, -4, 0);
	camera.AddUp(0, 1, 0);
	camera.AddWidth(1000);
	camera.AddHeight(700);
	camera.AddActive();
	camera.AddBehaviour("Camera.lua", "Camera");

	Common::Size terrainWidth = 10;
	Common::Size terrainLength = 10;
	//for (Common::Int64 x = -2; x < 2; x++) {
	//	for (Common::Int64 z = -2; z < 2; z++) {

			{
				Entity terrain = engine.CreateEntity();
				terrain.AddName("Terrain");
				terrain.AddPosition(0, 0, 0);
				terrain.AddRotation({0, 1, 0}, 0);
				terrain.AddImmutableRenderGeometry("BigRockyTerrain.geom");
				terrain.AddStaticRigidBodyCustomMeshShape();
				terrain.AddMaterial(0.1, 0.1, 0.1);

				Entity skybox = engine.CreateEntity();
				skybox.AddName("SkyBox");
				skybox.AddPosition(0, -300, 0);
				skybox.AddRotation({ 0, 1, 0 }, 0);
				skybox.AddImmutableRenderGeometry("SkyBox.geom");
			}
			//{
			//	Entity terrain = engine.CreateEntity();
			//	terrain.AddName("Terrain");
			//	terrain.AddPosition(x * 20.0f, 0, z * 20.0f);
			//	terrain.AddImmutableRenderGeometry("BigRockyTerrain.geom");
			//	terrain.AddStaticRigidBodyCustomMeshShape();
			//	terrain.AddMaterial(0.1, 0.1, 0.1);
			//}
	//	}
	//}
	//  }
  //}
		  //{
		  //    Entity terrain1 = engine.CreateEntity();
		  //    terrain1.AddName("Terrain");
		  //    terrain1.AddPosition(/*x **/ 0.f, 0, /*z **/ 0.f);
		  //    terrain1.AddImmutableRenderGeometry("Terrain.geom");
		  //    terrain1.AddStaticRigidBodyCustomMeshShape();
		  //    terrain1.AddMaterial(0.1, 0.1, 0.1);
		  //    terrainCells.push_back(terrain1);
		  //}
	Entity dragonLore = engine.CreateEntity();
	dragonLore.AddName("DragonLore");
	dragonLore.AddPosition(0, 10, 0);
	dragonLore.AddRotation({ 0, 1, 0 }, 0);
	dragonLore.AddImmutableRenderGeometry("dragon_lore.geom");
	dragonLore.AddDynamicRigidBodyCustomMeshShape();
	dragonLore.AddMapRigidBodyToRenderGeometry();
	dragonLore.AddMass(30);
	dragonLore.AddMaterial(0.1, 0.1, 0.1);

	//entity.AddImmutableRenderGeometry("dragon_lore.geom");

   /* Entity entity = engine.CreateEntity();
	entity.AddPosition(0, 0, 0);
	entity.AddBehaviour("TestObject.lua", "TestObject");

	Entity dragonLore = engine.CreateEntity();
	{
		dragonLore.AddPosition(0, 0, 0);
		dragonLore.AddBehaviour("TestObject.lua", "TestObject");
		glm::mat4 renderGeomTransform{ 1.0f };
		renderGeomTransform = glm::translate(renderGeomTransform, { 0, 0, 0 });
		dragonLore.AddImmutableRenderGeometry(renderGeomTransform, "dragon_lore.obj", "dragon_lore.mtl", { "dragon_lore.bmp" });
		glm::mat4 rigidBodyTransform = glm::mat4{ 1.0f };
		rigidBodyTransform = glm::translate(rigidBodyTransform, { 0, 10.0, 0.f });
		dragonLore.AddDynamicRigidBodyCustomMeshShape(rigidBodyTransform, "dragon_lore",10, 0, 0, 0.1, 0.1, 0.1);
		dragonLore.AddMapRigidBodyToRenderGeometry();
	}*/

	//Entity flashLight = engine.CreateEntity();
	//{
	//    flashLight.AddPosition(0, 0, 0);
	//    glm::mat4 renderGeomTransform{ 1.0f };
	//    renderGeomTransform = glm::translate(renderGeomTransform, { 0, 0, 0 });
	//   /* flashLight.AddImmutableRenderGeometry();*/
	//}

	//Entity animated = engine.CreateEntity();
	//{
	//    animated.AddPosition(0, 0, 0);
	//    glm::mat4 capsuleTransform = glm::mat4{ 1.0f };
	//    animated.AddSkinnedGeometry(capsuleTransform, "lowPolyHand.fbx", "", {});
	//}

	//Entity staticGeom = engine.CreateEntity();
	//{
	//    staticGeom.AddPosition(0, 0, 0);
	//    staticGeom.AddStaticRigidBodyCustomMeshShape(
	//        glm::mat4{ 1.f },
	//        " - de_dust2",
	//        0.5,
	//        0.5, 
	//        0.5);
	//}

	/*Entity flyCamera = engine.CreateEntity();
	{
		flyCamera.AddPosition(0, 0, 0);
		flyCamera.AddCamera({ 0, 0, 0 }, { 1, 0, 0 }, { 0, 1, 0 });
		flyCamera.AddBehaviour("Camera.lua", "Camera");
	}*/
	//Entity character = engine.CreateEntity();
	//{
	//    glm::mat4 capsuleTransform = glm::mat4{ 1.0f };
	//    capsuleTransform = glm::translate(capsuleTransform, { 300.f, 140.0, 0.f });

	//    character.AddPosition(1000.f, 0.f, 0.f);
	//    character.AddBehaviour("Character.lua", "Character");
	//    character.AddRigidBodyCapsule(
	//        capsuleTransform,
	//        80.f,
	//        0.1f,
	//        0.f,
	//        0.5f,
	//        0.5f,
	//        0.1f,
	//        30.f,
	//        40.f);
	//    //character.AddBehaviour("Camera.lua", "Camera");
	//    character.AddCamera({ 0.f, 0.f, -5.f }, { 0.f, 0.f, 5.f }, { 0.f, 1.f, 0.f });
	//    character.AddAttachedCamera(glm::mat4{ 1.f });
	//}

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


	//Entity dust2 = engine.CreateEntity();
	//{
	//    dust2.AddPosition(0, 0, 0);
	//    glm::mat4 renderGeomTransform{ 1.0f };
	//    renderGeomTransform = glm::translate(renderGeomTransform, { 0.f, 0.f, 0.f });
	//    dust2.AddImmutableRenderGeometry(
	//        renderGeomTransform,
	//        " - de_dust2.obj",
	//        " - de_dust2.mtl",
	//        {});
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