
#include <Engine.hpp>

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

	//Common::Size terrainWidth = 10;
	//Common::Size terrainLength = 10;

	{
		Entity terrain = engine.CreateEntity();
		terrain.AddName("Terrain");
		terrain.AddPosition(0, 0, 0);
		terrain.AddRotation({ 0, 1, 0 }, 0);
		terrain.AddImmutableRenderGeometry("BigRockyTerrain.geom");
		terrain.AddStaticRigidBodyCustomMeshShape();
		terrain.AddMaterial(0.1, 0.1, 0.1);

		Entity skybox = engine.CreateEntity();
		skybox.AddName("SkyBox");
		skybox.AddPosition(0, -300, 0);
		skybox.AddRotation({ 0, 1, 0 }, 0);
		skybox.AddImmutableRenderGeometry("SkyBox.geom");
	}

	//Entity dragonLore = engine.CreateEntity();
	//dragonLore.AddName("DragonLore");
	//dragonLore.AddPosition(0, 10, 0);
	//dragonLore.AddRotation({ 0, 1, 0 }, 0);
	//dragonLore.AddImmutableRenderGeometry("dragon_lore.geom");
	//dragonLore.AddDynamicRigidBodyCustomMeshShape();
	//dragonLore.AddMapRigidBodyToRenderGeometry();
	//dragonLore.AddMass(30);
	//dragonLore.AddMaterial(0.1, 0.1, 0.1);


	//Entity lamp = engine.CreateEntity();
	//lamp.AddName("Lamp");
	//lamp.AddPosition(0, 5, 0);
	//lamp.AddRotation({ 0, 1, 0 }, 0);
	//lamp.AddImmutableRenderGeometry("Lamp.geom");

	Entity axe = engine.CreateEntity();
	axe.AddName("Axe");
	axe.AddPosition(0, 11, 0);
	axe.AddLocalPosition(0, 0, 0);
	axe.AddRotation({ 0, 1, 0 }, 0);
	axe.AddImmutableRenderGeometry("Axe.geom");



	engine.Run();

	return 0;
}