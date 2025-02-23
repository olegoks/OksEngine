
#include <Render/Model/auto_OksEngine.AddMeshToRender.hpp>


namespace OksEngine {

	void AddMeshToRender::Update(
		ECS2::Entity::Id entityId,
		const DriverTransform3D* driverTransform3D, 
		const Meshes* meshes) {

		////return;
		//auto driver = GetContext().GetRenderSubsystem()->GetDriver();
		//auto* transform = world->GetComponent<DriverTransform3D>(entityId);
		//auto* meshes = world->GetComponent<Meshes>(entityId);

		//for (ECS::Entity::Id meshEntityId : meshes->entitiesIds_) {

		//	if (!world->IsComponentExist<DriverTexture>(meshEntityId)) {
		//		continue;
		//	}
		//	auto* vertices = world->GetComponent<DriverVertexBuffer>(meshEntityId);
		//	auto* indices = world->GetComponent<DriverIndexBuffer>(meshEntityId);
		//	//auto* uvs = world->GetComponent<UVs>(meshEntityId);
		//	//auto* normals = world->GetComponent<Normals>(meshEntityId);

		//	auto* driverTexture = world->GetComponent<DriverTexture>(meshEntityId);


		//	std::vector<RAL::Driver::ShaderBinding::Data> shaderBindings;

		//	//CAMERA BINDING
		//	auto* cameraViewProjUniformBuffer = world->GetConstComponent<UniformBuffer>(secondEntityId);
		//	{
		//		RAL::Driver::ShaderBinding::Data cameraBinding{
		//			.type_ = RAL::Driver::ShaderBinding::Type::Uniform,
		//			.stage_ = RAL::Driver::ShaderBinding::Stage::VertexShader,
		//			.uniformBufferId_ = cameraViewProjUniformBuffer->id_
		//		};
		//		shaderBindings.push_back(cameraBinding);
		//	}

		//	//TRANSFORM BINDING
		//	{
		//		RAL::Driver::ShaderBinding::Data transformBinding{
		//			.type_ = RAL::Driver::ShaderBinding::Type::Uniform,
		//			.stage_ = RAL::Driver::ShaderBinding::Stage::VertexShader,
		//			.uniformBufferId_ = transform->UBId_
		//		};
		//		shaderBindings.push_back(transformBinding);
		//	}

		//	//TEXTURE BINDING
		//	{

		//		RAL::Driver::ShaderBinding::Data textureBinding{
		//			.type_ = RAL::Driver::ShaderBinding::Type::Sampler,
		//			.stage_ = RAL::Driver::ShaderBinding::Stage::FragmentShader,
		//			.textureId_ = driverTexture->driverTextureId_
		//		};

		//		shaderBindings.push_back(textureBinding);
		//	}

		//	//std::vector<RAL::Vertex3fnt> vertices3fnt;
		//	//vertices3fnt.reserve(vertices->vertices_.GetVerticesNumber());
		//	//for (Common::Index i = 0; i < vertices->vertices_.GetVerticesNumber(); i++) {
		//	//	RAL::Vertex3fnt vertex;
		//	//	vertex.position_ = vertices->vertices_[i].position_;
		//	//	vertex.normal_ = normals->normals_[i];
		//	//	vertex.uv_ = uvs->uvs_[i];
		//	//	vertices3fnt.push_back(vertex);
		//	//}
		//	Common::Id driverMeshId = driver->DrawMesh(
		//		"Textured Pipeline",
		//		/*(const RAL::Vertex3fnt*)vertices3fnt.data(),
		//		vertices->vertices_.GetVerticesNumber(),
		//		RAL::Driver::VertexType::VF3_NF3_TF2,*/
		//		vertices->id_,
		//		indices->id_,
		//		/*(const RAL::Index16*)indices->indices_.GetData(),
		//		indices->indices_.GetIndicesNumber(),
		//		RAL::Driver::IndexType::UI16,*/
		//		shaderBindings
		//	);
		//}
	
		////world->CreateComponent<DriverMesh>(entityId, driverMeshId);
	}


	//std::pair<ECS::Entity::Filter, ECS::Entity::Filter> AddMeshToRender::GetFilter() const noexcept {
	//	static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}
	//		.Include<Node>()
	//		.Include<DriverTransform3D>()
	//		, ECS::Entity::Filter{}.Include<Camera>().Include<UniformBuffer>() };
	//	return filter;
	//}

	//Common::TypeId AddMeshToRender::GetTypeId() const noexcept {
	//	return Common::TypeInfo<AddMeshToRender>().GetId();
	//}

}
