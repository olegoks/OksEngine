
#include <Engine.hpp>
#include <OksEngine.ECS.hpp>

int main(int argc, char** argv) {

	using namespace OksEngine;

	//OS::Assert(ImGUIPosition2D::GetTypeId() != OksEngine::Position::GetTypeId());

	OS::AssertMessage(argc > 0, "First parameter must be config file name.");

	const Engine::CreateInfo engineCreateInfo{
		argc, argv
	};

	Engine engine{ engineCreateInfo };
	//Entity testEntity = engine.CreateEntity();
	//testEntity.AddClock();

	//Entity camera = engine.CreateEntity();
	//camera.AddCamera(0.1f, 1000.0f, true);
	//camera.AddPosition(0.5, 0.5, 0);
	//camera.AddDirection(-20, -15, 0);
	//camera.AddUp3D(0, 1, 0);
	//camera.AddZNear(0.1);
	//camera.AddZFar(1000);
	//camera.AddWidth(1000);
	//camera.AddHeight(700);
	//camera.AddActive();
	//camera.AddBehaviour("Camera.lua", "Camera");

	//{
		//Entity terrain = engine.CreateEntity<
		//	Name,
		//	Position3D,
		//	Rotation3D,
		//	DriverTransform3D,
		//	ImmutableRenderGeometry,
		//	LoadGDFTask,
		//	GDF,
		//	GDFInfo,
		//	GDFLoaded,
		//	StaticRigidBodyCustomMeshShape,
		//	Material>();
		//terrain.AddName("Terrain");
		//terrain.AddPosition(0, 0, 0);
		//terrain.AddRotation({ 0, 1, 0 }, 0);
		//terrain.AddImmutableRenderGeometry("BigRockyTerrain.geom", "");
		//terrain.AddStaticRigidBodyCustomMeshShape();
		//terrain.AddMaterial(0.1, 0.1, 0.1);

	//	Entity skybox = engine.CreateEntity();
	//	skybox.AddName("SkyBox");
	//	skybox.AddPosition(0, -300, 0);
	//	skybox.AddRotation({ 0, 1, 0 }, 0);
	//	skybox.AddImmutableRenderGeometry("SkyBox.geom");
	//}

	//Entity dragonLore = engine.CreateEntity();
	//dragonLore.AddName("DragonLore");
	//dragonLore.AddPosition(0, 10, 0);
	//dragonLore.AddRotation({ 0, 1, 0 }, 0);
	//dragonLore.AddImmutableRenderGeometry("dragon_lore.geom", );
	//dragonLore.AddDynamicRigidBodyCustomMeshShape();
	//dragonLore.AddMapRigidBodyToRenderGeometry();
	//dragonLore.AddMass(30);
	//dragonLore.AddMaterial(0.1, 0.1, 0.1);


	////Entity lamp = engine.CreateEntity();
	////lamp.AddName("Lamp");
	////lamp.AddPosition(0, 5, 0);
	////lamp.AddRotation({ 0, 1, 0 }, 0);
	////lamp.AddImmutableRenderGeometry("Lamp.geom");

	////Entity axe = engine.CreateEntity();
	////axe.AddName("m1911");
	////axe.AddPosition(0, 11, 0);
	////axe.AddLocalPosition(0, 0, 0);
	////axe.AddRotation({ 0, 1, 0 }, 0);
	////axe.AddImmutableRenderGeometry("m1911.geom");

	//Entity axe = engine.CreateEntity();
	//axe.AddName("Pistol");
	//axe.AddPosition(0, 11, 0);
	//axe.AddRotation({ 0, 1, 0 }, 0);
	//axe.AddImmutableRenderGeometry("Pistol.geom");

	engine.Run();

	return 0;
}