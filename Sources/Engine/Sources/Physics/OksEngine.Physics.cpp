#pragma once
#include <Physics\auto_OksEngine.Physics.hpp>
//#include <imguizmo/imguizmo.h>

#include <implot3d/implot3d.h>
namespace OksEngine
{
	namespace Physics
	{
		//Capsule
		void EditShapeGeometryCapsule(std::shared_ptr<ECS2::World> ecsWorld, ShapeGeometryCapsule* shapeGeometryCapsule) {

			ImGui::PushID(ShapeGeometryCapsule::GetTypeId());



			ImGui::PopID();
		}

		void AddShapeGeometryCapsule(ECS2::World* ecsWorld, ECS2::Entity::Id entityId) {
			if (ImGui::CollapsingHeader("Create info"))
			{
				if (ImGui::Button("Add component"))
				{
					if (!ecsWorld->IsComponentExist<ShapeGeometryCapsule>(entityId))
					{
						ecsWorld->CreateComponent<ShapeGeometryCapsule>(entityId, 1.0, 4, 3, 4);
					}
				}
			}
		}

		ShapeGeometryCapsule ParseShapeGeometryCapsule(luabridge::LuaRef& shapeGeometryCapsuleRef) {

			float radius = shapeGeometryCapsuleRef["radius"].cast<float>().value();
			float height = shapeGeometryCapsuleRef["height"].cast<float>().value();
			float segments = shapeGeometryCapsuleRef["segments"].cast<float>().value();
			float rings = shapeGeometryCapsuleRef["rings"].cast<float>().value();


			return ShapeGeometryCapsule{ radius, height, segments, rings };
		}

		std::string SerializeShapeGeometryCapsule(const ShapeGeometryCapsule* shapeGeometryCapsule) {

			NOT_IMPLEMENTED();
			return "";
		}

		//BOX
		//void EditShapeGeometryBox(std::shared_ptr<ECS2::World> ecsWorld, ShapeGeometryBox* shapeGeometryBox) {

		//	ImGui::PushID(ShapeGeometryCapsule::GetTypeId());



		//	ImGui::PopID();
		//}

		//void AddShapeGeometryBox(ECS2::World* ecsWorld, ECS2::Entity::Id entityId) {
		//	if (ImGui::CollapsingHeader("Create info"))
		//	{
		//		if (ImGui::Button("Add component"))
		//		{
		//			if (!ecsWorld->IsComponentExist<ShapeGeometryBox>(entityId))
		//			{
		//				ecsWorld->CreateComponent<ShapeGeometryBox>(entityId, 1.0, 4, 3, 4);
		//			}
		//		}
		//	}
		//}

		//ShapeGeometryBox ParseShapeGeometryBox(luabridge::LuaRef& shapeGeometryCapsuleRef) {

		//	float radius = shapeGeometryCapsuleRef["radius"].cast<float>().value();
		//	float height = shapeGeometryCapsuleRef["height"].cast<float>().value();
		//	float segments = shapeGeometryCapsuleRef["segments"].cast<float>().value();


		//	return ShapeGeometryBox{ radius, height, segments};
		//}

		//std::string SerializeShapeGeometryBox(const ShapeGeometryCapsule* shapeGeometryCapsule) {

		//	NOT_IMPLEMENTED();
		//	return "";
		//}

		//Custom mesh shape
		void EditShapeGeometryData(std::shared_ptr<ECS2::World> ecsWorld, ShapeGeometryData* shapeGeometryData) {

			ImGui::PushID(ShapeGeometryData::GetTypeId());


			ImGui::PushStyleVar(ImGuiStyleVar_DisabledAlpha, ImGui::GetStyle().Alpha * 0.5f);
			//ImGui::PushItemFlag(ImGuiItemFlags_, true);
			ImGui::Checkbox("convexGeometry", &shapeGeometryData->convexGeometry_);
			//ImGui::PopItemFlag();
			ImGui::PopStyleVar();



			// Choose fill color
			static bool set_fill_color = true;
			static ImVec4 fill_color = ImVec4(0.8f, 0.8f, 0.2f, 0.6f);
			ImGui::Checkbox("Fill Color", &set_fill_color);
			if (set_fill_color) {
				ImGui::SameLine();
				ImGui::ColorEdit4("##MeshFillColor", (float*)&fill_color);
			}

			// Choose line color
			static bool set_line_color = true;
			static ImVec4 line_color = ImVec4(0.2f, 0.2f, 0.2f, 0.8f);
			ImGui::Checkbox("Line Color", &set_line_color);
			if (set_line_color) {
				ImGui::SameLine();
				ImGui::ColorEdit4("##MeshLineColor", (float*)&line_color);
			}

			// Choose marker color
			static bool set_marker_color = false;
			static ImVec4 marker_color = ImVec4(0.2f, 0.2f, 0.2f, 0.8f);
			ImGui::Checkbox("Marker Color", &set_marker_color);
			if (set_marker_color) {
				ImGui::SameLine();
				ImGui::ColorEdit4("##MeshMarkerColor", (float*)&marker_color);
			}
			ImPlot3D::SetCurrentContext(ImPlot3D::CreateContext());
			if (ImPlot3D::BeginPlot("Shape")) {

				std::vector<ImPlot3DPoint> imVertices;

				float max_x = -10000.0f;
				float min_x = 10000.0f;

				float max_y = -10000.0f;
				float min_y = 10000.0f;

				float max_z = -10000.0f;
				float min_z = 10000.0f;

				for (auto vertex : shapeGeometryData->vertices_) {

					max_x = std::max(max_x, vertex.x_);
					min_x = std::min(min_x, vertex.x_);

					max_y = std::max(max_y, vertex.y_);
					min_y = std::min(min_y, vertex.y_);

					max_z = std::max(max_z, vertex.z_);
					min_z = std::min(min_z, vertex.z_);

					imVertices.push_back({ vertex.x_, vertex.y_, vertex.z_ });
				}

				std::vector<unsigned int> imIndices;
				for (auto index : shapeGeometryData->indices_) {
					imIndices.push_back(index);
				}
				float offset = 1.f;
				ImPlot3D::SetupAxesLimits(
					min_x - offset, max_x + offset,
					min_y - offset, max_y + offset,
					min_z - offset, max_z + offset, ImPlot3DCond_Once);
				// Set colors
				if (set_fill_color)
					ImPlot3D::SetNextFillStyle(fill_color);
				else {
					// If not set as transparent, the fill color will be determined by the colormap
					ImPlot3D::SetNextFillStyle(ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
				}
				if (set_line_color)
					ImPlot3D::SetNextLineStyle(line_color);
				if (set_marker_color)
					ImPlot3D::SetNextMarkerStyle(ImPlot3DMarker_Square, 3, marker_color, IMPLOT3D_AUTO, marker_color);

				ImPlot3D::PlotMesh(
					"Mesh",
					imVertices.data(), imIndices.data(), imVertices.size(), imIndices.size());

				ImPlot3D::EndPlot();
			}
			ImPlot3D::SetCurrentContext(nullptr);
			ImGui::PopID();
		}

		void AddShapeGeometryData(ECS2::World* ecsWorld, ECS2::Entity::Id entityId) {

			if (ImGui::CollapsingHeader("Create info"))
			{

				bool convex = true;
				ImGui::Checkbox("Convex", &convex);

				ImGui::Text("Shape Type:");
				static bool cubeMesh = false;
				static bool sphere = false;
				static bool customMesh = false;
				if (ImGui::RadioButton("Cube", cubeMesh)) {
					cubeMesh = true;
				}
				ImGui::SameLine();
				if (ImGui::RadioButton("Sphere", sphere)) {
					sphere = true;
				}
				ImGui::SameLine();
				if (ImGui::RadioButton("Custom mesh", customMesh)) {
					customMesh = true;
				}

				std::vector<ShapeVertex> vertices;
				std::vector<Common::UInt16> indices;
				if (cubeMesh) {
					ImGui::Text("Cube Parameters:");
					static float cubeHalfExtents[3]{ 0.1f, 0.1f, 0.1f };
					if (ImGui::DragFloat3("Half Extents", cubeHalfExtents, 0.1f, 0.1f, 10.0f)) {

					}
					const glm::vec3& he{ cubeHalfExtents[0], cubeHalfExtents[1], cubeHalfExtents[2] };

					// 8 ������ ����
					vertices = {
						{-he.x, -he.y, -he.z}, { he.x, -he.y, -he.z},
						{ he.x,  he.y, -he.z}, {-he.x,  he.y, -he.z},
						{-he.x, -he.y,  he.z}, { he.x, -he.y,  he.z},
						{ he.x,  he.y,  he.z}, {-he.x,  he.y,  he.z}
					};
					indices = {
						// ������ �����
						0, 2, 1, 0, 3, 2,
						// �������� �����
						4, 5, 6, 4, 6, 7,
						// ����� �����
						0, 4, 7, 0, 7, 3,
						// ������ �����
						1, 2, 6, 1, 6, 5,
						// ������ �����
						0, 1, 5, 0, 5, 4,
						// ������� �����
						3, 7, 6, 3, 6, 2
					};
				}
				else if (sphere) {
					const float t = (1.0f + std::sqrt(5.0f)) / 2.0f;

					vertices = {
						{-1,  t,  0}, { 1,  t,  0}, {-1, -t,  0}, { 1, -t,  0},
						{ 0, -1,  t}, { 0,  1,  t}, { 0, -1, -t}, { 0,  1, -t},
						{ t,  0, -1}, { t,  0,  1}, {-t,  0, -1}, {-t,  0,  1}
					};

					indices = {
						0, 11, 5,  0, 5, 1,  0, 1, 7,  0, 7, 10, 0, 10, 11,
						1, 5, 9,  5, 11, 4, 11, 10, 2, 10, 7, 6, 7, 1, 8,
						3, 9, 4,  3, 4, 2,  3, 2, 6,  3, 6, 8,  3, 8, 9,
						4, 9, 5,  2, 4, 11, 6, 2, 10, 8, 6, 7,  9, 8, 1
					};
				}
				ImGui::Spacing();
				if (ImGui::Button("Add component"))
				{
					if (!ecsWorld->IsComponentExist<ShapeGeometryData>(entityId))
					{
						ecsWorld->CreateComponent<ShapeGeometryData>(entityId, convex, vertices, indices);
					}
				}
			}

		}

		ShapeGeometryData ParseShapeGeometryData(luabridge::LuaRef& shapeGeometryDataRef) {

			std::vector<ShapeVertex> vertices;
			std::vector<Common::UInt16> indices;

			bool convexGeometry = true;
			if (!shapeGeometryDataRef["convexGeometry"].isNil()) {
				convexGeometry = shapeGeometryDataRef["convexGeometry"].cast<bool>().value();
			}

			if (!shapeGeometryDataRef["box"].isNil()) {
				luabridge::LuaRef boxRef = shapeGeometryDataRef["box"];
				float boxXExtent = boxRef["xExtent"].cast<float>().value();
				float boxYExtent = boxRef["yExtent"].cast<float>().value();
				float boxZExtent = boxRef["zExtent"].cast<float>().value();

				static float cubeHalfExtents[3]{ boxXExtent, boxYExtent, boxZExtent };

				const glm::vec3& he{ cubeHalfExtents[0], cubeHalfExtents[1], cubeHalfExtents[2] };

				// 8 ������ ����
				vertices = {
					{-he.x, -he.y, -he.z}, { he.x, -he.y, -he.z},
					{ he.x,  he.y, -he.z}, {-he.x,  he.y, -he.z},
					{-he.x, -he.y,  he.z}, { he.x, -he.y,  he.z},
					{ he.x,  he.y,  he.z}, {-he.x,  he.y,  he.z}
				};
				indices = {
					// ������ �����
					0, 2, 1, 0, 3, 2,
					// �������� �����
					4, 5, 6, 4, 6, 7,
					// ����� �����
					0, 4, 7, 0, 7, 3,
					// ������ �����
					1, 2, 6, 1, 6, 5,
					// ������ �����
					0, 1, 5, 0, 5, 4,
					// ������� �����
					3, 7, 6, 3, 6, 2
				};
			}
			else if (!shapeGeometryDataRef["mesh"].isNil()) {

				luabridge::LuaRef meshRef = shapeGeometryDataRef["mesh"];

				luabridge::LuaRef verticesRef = meshRef["vertices"];
				luabridge::LuaRef indicesRef = meshRef["indices"];

				for (luabridge::Iterator it(verticesRef); !it.isNil(); ++it) {

					luabridge::LuaRef vertexRef = it.value();

					const float x = vertexRef["x"].cast<float>().value();
					const float y = vertexRef["y"].cast<float>().value();
					const float z = vertexRef["z"].cast<float>().value();
					vertices.push_back({ x, y, z });

				}

				for (luabridge::Iterator it(indicesRef); !it.isNil(); ++it) {
					indices.push_back(it.value().cast<Common::UInt16>().value());
				}
			}
			else if (!shapeGeometryDataRef["capsule"].isNil()) {
				luabridge::LuaRef capsuleRef = shapeGeometryDataRef["capsule"];

				float radius = capsuleRef["radius"].cast<float>().value();
				float height = capsuleRef["height"].cast<float>().value();
				float segments = capsuleRef["segments"].cast<float>().value();
				float rings = capsuleRef["rings"].cast<float>().value();

				float halfHeight = height * 0.5f;
				int totalRings = rings * 2 + 1; // +1 ��� ������������ ������

				// ��������� ���������
				for (int ring = 0; ring <= totalRings; ++ring) {
					// ��������������� ���������� �� ������ �� -1 �� 1
					float v = static_cast<float>(ring) / totalRings;
					float y = (v - 0.5f) * 2.0f;

					for (int segment = 0; segment <= segments; ++segment) {
						// ���� ������ ��� Y
						float u = static_cast<float>(segment) / segments;
						float angle = u * 2.0f * 3.14;

						float cosA = cos(angle);
						float sinA = sin(angle);

						glm::vec3 vertex;
						glm::vec3 normal;
						//glm::vec2 texCoord(u, 1.0f - v);

						if (y > 0.5f) {
							// ������� ���������
							float sphereY = (y - 0.5f) * 2.0f; // ����������� � ��������� [0,1]
							float phi = acosf(sphereY);
							float cosPhi = cosf(phi);
							float sinPhi = sinf(phi);

							vertex = glm::vec3(
								radius * sinPhi * cosA,
								halfHeight + radius * sphereY,
								radius * sinPhi * sinA
							);
							//normal = glm::vec3(sinPhi * cosA, sphereY, sinPhi * sinA);
						}
						else if (y < -0.5f) {
							// ������ ���������
							float sphereY = (y + 0.5f) * -2.0f; // ����������� � ��������� [0,1]
							float phi = acosf(sphereY);
							float cosPhi = cosf(phi);
							float sinPhi = sinf(phi);

							vertex = glm::vec3(
								radius * sinPhi * cosA,
								-halfHeight - radius * sphereY,
								radius * sinPhi * sinA
							);
							//normal = glm::vec3(sinPhi * cosA, -sphereY, sinPhi * sinA);
						}
						else {
							// �������������� �����
							vertex = glm::vec3(
								radius * cosA,
								y * height,
								radius * sinA
							);
							//normal = glm::vec3(cosA, 0.0f, sinA);
						}

						vertices.push_back(ShapeVertex{ vertex.x,vertex.y,vertex.z });
						//mesh.normals.push_back(glm::normalize(normal));
						//mesh.texCoords.push_back(texCoord);
					}
				}

				// ��������� ��������
				int verticesPerRing = segments + 1;

				for (int ring = 0; ring < totalRings; ++ring) {
					for (int segment = 0; segment < segments; ++segment) {
						int current = ring * verticesPerRing + segment;
						int next = current + verticesPerRing;

						// ������ �����������
						indices.push_back(current);
						indices.push_back(next);
						indices.push_back(current + 1);

						// ������ �����������
						indices.push_back(current + 1);
						indices.push_back(next);
						indices.push_back(next + 1);
					}
				}

			}

			return ShapeGeometryData{ convexGeometry, vertices,indices };
		}

		std::string SerializeShapeGeometryData(const ShapeGeometryData* shapeGeometryData) {
			NOT_IMPLEMENTED();
			return "";
		}

		void BindVelocity(::Lua::Context& context) {
			context.GetGlobalNamespace()
				.beginClass<Velocity>("Velocity")
				.addProperty("velocity", &Velocity::value_)
				.endClass();
		}

		//SetVelocityRequests


		void BindSetVelocityRequests(::Lua::Context& context) {
			context.GetGlobalNamespace()
				.beginClass<SetVelocityRequests>("SetVelocityRequests")
				.addFunction("AddSetVelocityRequest", [](SetVelocityRequests* self, float dirX, float dirY, float dirZ, float velocity) {
				self->requests_.push_back(
					SetVelocityRequest{ glm::vec3{ dirX, dirY, dirZ }, velocity }
				);
					})
				.endClass();
		}

		SetVelocityRequests ParseSetVelocityRequests(luabridge::LuaRef& setVelocityRequestsRef) {

			return SetVelocityRequests{};
		}

		std::string SerializeSetVelocityRequests(const SetVelocityRequests* setVelocityRequests) {
			return {};
		}

		//AddVelocityRequests
		void BindAddVelocityRequests(::Lua::Context& context) {
			context.GetGlobalNamespace()
				.beginClass<AddVelocityRequests>("AddVelocityRequests")
				.addFunction("AddAddVelocityRequest", [](AddVelocityRequests* self, float dirX, float dirY, float dirZ, float velocity) {
				self->requests_.push_back(
					AddVelocityRequest{ glm::vec3{ dirX, dirY, dirZ }, velocity }
				);
					})
				.endClass();
		}

		AddVelocityRequests ParseAddVelocityRequests(luabridge::LuaRef& addVelocityRequestsRef) {

			return AddVelocityRequests{};
		}

		std::string SerializeAddVelocityRequests(const AddVelocityRequests* addVelocityRequests) {
			return {};
		}

		//SetAngularVelocityRequests
		void BindSetAngularVelocityRequests(::Lua::Context& context) {
			context.GetGlobalNamespace()
				.beginClass<SetAngularVelocityRequests>("SetAngularVelocityRequests")
				.addFunction("AddSetAngularVelocityRequest", [](SetAngularVelocityRequests* self, float rotateAxisX, float rotateAxisY, float rotateAxisZ, float degreesInSecond) {
				self->requests_.push_back(
					SetAngularVelocityRequest{ glm::normalize(glm::vec3{ rotateAxisX, rotateAxisY, rotateAxisZ }), degreesInSecond }
				);
					})
				.endClass();
		}


		SetAngularVelocityRequests ParseSetAngularVelocityRequests(luabridge::LuaRef& setVelocityRequestsRef) {

			return SetAngularVelocityRequests{};
		}

		std::string SerializeSetAngularVelocityRequests(const SetAngularVelocityRequests* setAngularVelocityRequests) {
			return {};
		}

		//RigidBodyEntityId

		void BindRigidBodyEntityId(::Lua::Context& context) {
			context.GetGlobalNamespace()
				.beginClass<RigidBodyEntityId>("RigidBodyEntityId")
				.addProperty("id",
					[](RigidBodyEntityId* rigidBodyEntityId) {
						return rigidBodyEntityId->id_.GetRawValue();
					},
					[](RigidBodyEntityId* rigidBodyEntityId, ECS2::Entity::Id::ValueType entityId) {
						rigidBodyEntityId->id_ = entityId;
					})

				.endClass();
		}

		//RigidBodyEntityIds
		void EditRigidBodyEntityIds(std::shared_ptr<ECS2::World> ecsWorld, RigidBodyEntityIds* rigidBodyEntityIds) {
			NOT_IMPLEMENTED();
		}
		void BindRigidBodyEntityIds(::Lua::Context& context) {
			context.GetGlobalNamespace()
				.beginClass<RigidBodyEntityIds>("RigidBodyEntityIds")
				.endClass();
		}

		RigidBodyEntityIds ParseRigidBodyEntityIds(luabridge::LuaRef& rigidBodyEntityIdsRef) {
			NOT_IMPLEMENTED();
			return RigidBodyEntityIds{};
		}

		std::string SerializeRigidBodyEntityIds(const RigidBodyEntityIds* rigidBodyEntityIds) {
			NOT_IMPLEMENTED();
			return "";
		}

		void CreateEngine::Update() {

			ECS2::Entity::Id physicsEngineEntity = CreateEntity();

			auto physicsEngine = std::make_shared<PE::PhysicsEngine>();
			CreateComponent<Engine>(physicsEngineEntity, physicsEngine);
			CreateComponent<SimulationGranularity>(physicsEngineEntity, 1);

		}

		void CreateWorld::Update(ECS2::Entity::Id entity0id, const Physics::Engine* engine0) {

			PAL::World::CreateInfo worldCreateInfo{
				.gravity_ = { 0.f, -9.81f, 0.f }
			};

			auto world = engine0->engine_->CreateWorld(worldCreateInfo);

			ECS2::Entity::Id physicsWorldEntity = CreateEntity();
			CreateComponent<World>(physicsWorldEntity, world);


		};

		void CreateStaticRigidBody::Update(
			ECS2::Entity::Id entity0id,
			Physics::World* world0,
			ECS2::Entity::Id entity1id,
			const Name* name1,
			const StaticRigidBody* staticRigidBody1,
			const WorldPosition3D* position3D1,
			const WorldRotation3D* rotation3D1,
			const Physics::Material* material1,
			const Physics::PhysicsShape* physicsShape1) {


			PAL::StaticRigidBody::CreateInfo srbCreateInfo{
				.rbCreateInfo_ = {
					.transform_ = glm::mat4{ glm::translate(glm::vec3(position3D1->x_, position3D1->y_, position3D1->z_)) } ,
					.shape_ = physicsShape1->shape_,
					.name_ = name1->value_
				}
			};

			const PAL::StaticRigidBody::Id srbId = world0->world_->CreateStaticRigidBody(srbCreateInfo);
			world0->world_->AddStaticRigidBody(srbId);

			CreateComponent<RigidBodyId>(entity1id, srbId);


		};

		void CreatePhysicsShapeCapsule::Update(
			ECS2::Entity::Id entity0id,
			Physics::Engine* engine0,

			ECS2::Entity::Id entity1id,
			const Physics::Material* material1,
			const Physics::ShapeGeometryCapsule* shapeGeometryCapsule1) {

			PAL::Shape::Material material{
				.staticFriction_ = material1->staticFriction_,
				.dynamicFriction_ = material1->dynamicFriction_,
				.restitution_ = material1->restitution_
			};

			PAL::Shape::CreateInfoCapsule shapeCreateInfo{
				.material_ = material,
				.radius_ = shapeGeometryCapsule1->radius_,
				.height_ = shapeGeometryCapsule1->height_
			};

			auto shape = engine0->engine_->CreateShape(shapeCreateInfo);
			CreateComponent<PhysicsShape>(entity1id, shape);
		}

		void CreatePhysicsShapeBox::Update(ECS2::Entity::Id entity0id, Physics::Engine* engine0, ECS2::Entity::Id entity1id,
			const Physics::Material* material1, const Physics::ShapeGeometryBox* shapeGeometryBox1) {

			PAL::Shape::Material material{
				.staticFriction_ = material1->staticFriction_,
				.dynamicFriction_ = material1->dynamicFriction_,
				.restitution_ = material1->restitution_
			};

			PAL::Shape::CreateInfoBox shapeCreateInfo{
				.material_ = material,
				.halfExtentX_ = shapeGeometryBox1->lenght_ / 2,
				.halfExtentY_ = shapeGeometryBox1->height_ / 2,
				.halfExtentZ_ = shapeGeometryBox1->width_ / 2
			};

			auto shape = engine0->engine_->CreateShape(shapeCreateInfo);
			CreateComponent<PhysicsShape>(entity1id, shape);

			const auto* shapeData = shapeGeometryBox1;

			float halfW = shapeData->lenght_ * 0.5f;
			float halfH = shapeData->height_ * 0.5f;
			float halfD = shapeData->width_ * 0.5f;

			// 8 уникальных вершин куба
			Geom::Vertex3f uniqueVertices[8] = {
				// Передняя грань
				{ Geom::Vertex3f(-halfW, -halfH,  halfD) },// 0: лево-низ-перед
				{Geom::Vertex3f(halfW, -halfH,  halfD)  }, // 1: право-низ-перед
				{Geom::Vertex3f(halfW,  halfH,  halfD)  },// 2: право-верх-перед
				{Geom::Vertex3f(-halfW,  halfH,  halfD) }, // 3: лево-верх-перед

				// Задняя грань
				{Geom::Vertex3f(-halfW, -halfH, -halfD)}, // 4: лево-низ-зад
				{Geom::Vertex3f(halfW, -halfH, -halfD) },// 5: право-низ-зад
				{Geom::Vertex3f(halfW,  halfH, -halfD) },// 6: право-верх-зад
				{Geom::Vertex3f(-halfW,  halfH, -halfD) } // 7: лево-верх-зад
			};

			// 12 треугольников (2 на каждую грань) напрямую как вершины
			Geom::VertexCloud<Geom::Vertex3f> result;

			Geom::Vertex3f boxVertices[] = {
				// Передняя грань (2 треугольника)
				{ uniqueVertices[0] }, { uniqueVertices[1] }, { uniqueVertices[2] }, // Треугольник 1
				{ uniqueVertices[2] }, { uniqueVertices[3] }, { uniqueVertices[0] }, // Треугольник 2

				// Правая грань
				{ uniqueVertices[1] }, { uniqueVertices[5] }, { uniqueVertices[6] },
				{ uniqueVertices[6] }, { uniqueVertices[2] }, { uniqueVertices[1] },

				// Задняя грань
				{ uniqueVertices[5] }, { uniqueVertices[4] }, { uniqueVertices[7] },
				{ uniqueVertices[7] }, { uniqueVertices[6] }, { uniqueVertices[5] },

				// Левая грань
				{ uniqueVertices[4] }, { uniqueVertices[0] }, { uniqueVertices[3] },
				{ uniqueVertices[3] }, { uniqueVertices[7] }, { uniqueVertices[4] },

				// Верхняя грань
				{ uniqueVertices[3] }, { uniqueVertices[2] }, { uniqueVertices[6] },
				{ uniqueVertices[6] }, { uniqueVertices[7] }, { uniqueVertices[3] },

				// Нижняя грань
				{ uniqueVertices[0] }, { uniqueVertices[4] }, { uniqueVertices[5] },
				{ uniqueVertices[5] }, { uniqueVertices[1] }, { uniqueVertices[0] }
			};

			result.Add(boxVertices, 36);

			CreateComponent<Vertices3D>(entity1id, result);
			Geom::IndexBuffer<Geom::Index32> indices;
			for (Common::Index i = 0; i < result.GetVerticesNumber(); i++) {
				indices.Add(i);
			}
			CreateComponent<Indices>(entity1id, indices);

		}	

		void CreatePhysicsShapeCylinder::Update(ECS2::Entity::Id entity0id, Physics::Engine* engine0, ECS2::Entity::Id entity1id,
			const Physics::Material* material1, const Physics::ShapeGeometryCylinder* shapeGeometryCylinder1) {

			PAL::Shape::Material material{
				.staticFriction_ = material1->staticFriction_,
				.dynamicFriction_ = material1->dynamicFriction_,
				.restitution_ = material1->restitution_
			};

			AI_GENERATED
				// �������� ������ ��������
				Geometry::VertexCloud<Geom::Vertex3f> vertices;
			//Geom::IndexBuffer<Geom::Index16> indices;
			{
				Common::Size numSides = 16;
				vertices.Reserve(2 * numSides + 2); // +2 ��� ������� ���������

				// ������ ���������
				vertices.Add({ 0, -shapeGeometryCylinder1->height_ / 2, 0 });  // ������ �����
				vertices.Add({ 0, shapeGeometryCylinder1->height_ / 2, 0 });   // ������� �����

				// ������� ������� �����������
				for (int i = 0; i < numSides; ++i) {
					float angle = 2.0f * Math::pi * i / numSides;
					float x = shapeGeometryCylinder1->radius_ * cos(angle);
					float z = shapeGeometryCylinder1->radius_ * sin(angle);

					// ������ ���������
					vertices.Add({ x, -shapeGeometryCylinder1->height_, z });
					// ������� ���������
					vertices.Add({ x, shapeGeometryCylinder1->height_, z });
				}
				//for (Common::Index i = 0; i < 2 * numSides + 2; i++) {
				//	indices.Add(i);
				//}
			}

			PAL::Shape::CreateInfoMesh shapeCreateInfo{
				.material_ = material,
				.convexGeometry_ = true,
				.vertices_ = vertices,
				//.indices_ = indices
			};

			auto shape = engine0->engine_->CreateShape(shapeCreateInfo);
			CreateComponent<PhysicsShape>(entity1id, shape);

			Geom::IndexBuffer<Geom::Index32> indices;
			for (Common::Index i = 0; i < vertices.GetVerticesNumber(); i++) {
				indices.Add(i);
			}
			CreateComponent<Vertices3D>(entity1id, vertices);
			CreateComponent<Indices>(entity1id, indices);

		}

		namespace Character {
			void CreateCapsuleController::Update(
				ECS2::Entity::Id entity0id, Physics::World* world0, ECS2::Entity::Id entity1id,
				const Physics::Character::CapsuleController* capsuleController1, const Physics::Material* material1,
				const WorldPosition3D* worldPosition3D1, const Physics::ShapeGeometryCapsule* shapeGeometryCapsule1) {

				PAL::CapsuleController::CreateInfo ci{
					.position_ = { worldPosition3D1->x_, worldPosition3D1->y_, worldPosition3D1->z_ },
					.capsuleHeight_ = shapeGeometryCapsule1->height_,
					.capsuleRadius_ = shapeGeometryCapsule1->radius_
				};

				PAL::CapsuleController::Id capsuleControllerId = world0->world_->CreateCapsuleController(ci);

				CreateComponent<Physics::Character::CapsuleControllerId>(entity1id, capsuleControllerId);

			}


		}
		void CreatePhysicsShape::Update(
			ECS2::Entity::Id entity0id,
			Physics::Engine* engine0,

			ECS2::Entity::Id entity1id,
			const Physics::Material* material1,
			const Physics::ShapeGeometryData* shapeGeometryData1) {

			PAL::Shape::Material material{
				.staticFriction_ = material1->staticFriction_,
				.dynamicFriction_ = material1->dynamicFriction_,
				.restitution_ = material1->restitution_
			};

			Geometry::VertexCloud<Geom::Vertex3f> vertices;
			vertices.Reserve(shapeGeometryData1->vertices_.size());

			for (auto vertex : shapeGeometryData1->vertices_) {
				vertices.Add({ vertex.x_, vertex.y_, vertex.z_ });
			}

			Geom::IndexBuffer<Geom::Index16> indices16;
			indices16.Reserve(shapeGeometryData1->indices_.size());
			indices16.Add(
				shapeGeometryData1->indices_.data(),
				shapeGeometryData1->indices_.size());

			PAL::Shape::CreateInfoMesh shapeCreateInfo{
				.material_ = material,
				.convexGeometry_ = shapeGeometryData1->convexGeometry_,
				.vertices_ = vertices,
				.indices_ = indices16
			};

			auto shape = engine0->engine_->CreateShape(shapeCreateInfo);
			CreateComponent<PhysicsShape>(entity1id, shape);

			Geom::IndexBuffer<Geom::Index32> indices32;
			for (Common::Index i = 0; i < indices16.GetIndicesNumber(); i++) {
				indices32.Add(indices16[i]);
			}
			CreateComponent<Indices>(entity1id, indices32);
			CreateComponent<Vertices3D>(entity1id, vertices);

		};

		void UpdateDynamicRigidBodyTransform::Update(ECS2::Entity::Id entity0id, const OksEngine::Physics::Engine* engine0,
			const OksEngine::Physics::EnableSimulation* enableSimulation0, ECS2::Entity::Id entity1id,
			const OksEngine::Physics::World* world1, ECS2::Entity::Id entity2id,
			OksEngine::WorldPosition3D* worldPosition3D2, OksEngine::WorldRotation3D* worldRotation3D2,
			const OksEngine::Physics::RigidBodyId* rigidBodyId2) {

			Math::Vector3f position = world1->world_->GetRigidBodyPosition(rigidBodyId2->id_);

			worldPosition3D2->x_ = position.GetX();
			worldPosition3D2->y_ = position.GetY();
			worldPosition3D2->z_ = position.GetZ();

			//Move to separate system or rename this function
			glm::quat rotation = world1->world_->GetRigidBodyRotation(rigidBodyId2->id_);

			worldRotation3D2->w_ = rotation.w;
			worldRotation3D2->x_ = rotation.x;
			worldRotation3D2->y_ = rotation.y;
			worldRotation3D2->z_ = rotation.z;

		}

		//void CreatePhysicsShapeForDynamicRigidBody::Update(
		//	ECS2::Entity::Id entity0id,
		//	const Material* material0,
		//	const Vertices3D* vertices3D0,
		//	const Indices* indices0,
		//	const DynamicRigidBodyCustomMeshShape* dynamicRigidBodyCustomMeshShape0,

		//	ECS2::Entity::Id entity1id,
		//	Engine* physicsEngine1) {


		//	//
		//	//		auto* meshsEntities = world->GetComponent<ChildEntities>(modelEntity->id_);
		//	//
		//	//		const ECS::Entity::Id meshEntityId = meshsEntities->entitiesIds_[0];
		//	//
		//	//		auto* vertices = world->GetComponent<Vertices3D>(meshEntityId);
		//	//		auto* indices = world->GetComponent<Indices>(meshEntityId);
		//	//
		//	//		PAL::Shape::Material shapeMaterial{
		//	//			.staticFriction_ = material->staticFriction_,
		//	//			.dynamicFriction_ = material->dynamicFriction_,
		//	//			.restitution_ = material->restitution_
		//	//		};
		//	//
		//	//		PAL::Shape::CreateInfoMesh shapeCreateInfo{
		//	//			.material_ = material,
		//	//			.vertices_ = vertices->vertices_,
		//	//			.indices_ = indices->indices_
		//	//		};
		//	//
		//	//		auto shape = GetContext().GetPhysicsSubsystem()->CreateShape(shapeCreateInfo);
		//	//
		//	//		world->CreateComponent<PhysicsShape>(entityId, shape);
		//	//
		//	//		//{
		//	////	DynamicRigidBodyCustomMeshShape* dynamicRigidBody = world->GetComponent<DynamicRigidBodyCustomMeshShape>(entityId);
		//	//
		//	////	if (dynamicRigidBody != nullptr) {
		//	////		auto physicsSubsystem = GetContext().GetPhysicsSubsystem();
		//	////		if (dynamicRigidBody->id_ == Common::Limits<Common::Index>::Max()) {
		//	////			auto modelGeom = physicsSubsystem->GetGeom(dynamicRigidBody->meshName_);
		//	////			PAL::Shape::CreateInfoMesh shapeCreateInfo{
		//	////				.material_ = PAL::Shape::Material{
		//	////					.staticFriction_ = dynamicRigidBody->material_.staticFriction_,
		//	////					.dynamicFriction_ = dynamicRigidBody->material_.dynamicFriction_,
		//	////					.restitution_ = dynamicRigidBody->material_.restitution_
		//	////				},
		//	////				.shape_ = *(modelGeom->begin())
		//	////			};
		//	//
		//	////			auto shape = physicsSubsystem->CreateShape(shapeCreateInfo);
		//	////			PAL::RigidBody::CreateInfo createInfo{
		//	////				.transform_ = dynamicRigidBody->transform_,
		//	////				.shape_ = shape
		//	////			};
		//	////			dynamicRigidBody->id_ = physicsSubsystem->CreateRigidBody(createInfo);
		//	////			physicsSubsystem->AddRigidBodyToWorld(dynamicRigidBody->id_);
		//	////		}
		//	//
		//	////		//}
		//	////		//auto rbTransform = physicsSubsystem->GetRigidBodyTransform(rigidBodyCapsule->id_);
		//	////		////Delete rotate component
		//	////		//rbTransform[0][0] = 1.0f; rbTransform[0][1] = 0.0f; rbTransform[0][2] = 0.0f;
		//	////		//rbTransform[1][0] = 0.0f; rbTransform[1][1] = 1.f; rbTransform[1][2] = 0.0f;
		//	////		//rbTransform[2][0] = 0.0f; rbTransform[2][1] = 0.0f; rbTransform[2][2] = 1.0f;
		//	////		//rbTransform = glm::rotate(rbTransform, glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
		//	////		//physicsSubsystem->SetRigidBodyTransform(rigidBodyCapsule->id_, rbTransform);
		//	////		//rigidBodyCapsule->SetTransform(rbTransform);
		//	////	}
		//	////}

		//};

		void CreateDynamicRigidBody::Update(
			ECS2::Entity::Id entity0id,
			Physics::World* world0,

			ECS2::Entity::Id entity1id,
			const Physics::DynamicRigidBody* dynamicRigidBody1,
			const WorldPosition3D* worldPosition3D1,
			const Physics::Material* material1,
			const Physics::Mass* mass1,
			const Physics::PhysicsShape* physicsShape1) {

			auto rbComponentsFilter = GetComponentsFilter(entity1id);


			float angularDamping = 0.05;
			float linearDamping = 0.1;
			if (rbComponentsFilter.IsSet<AngularDamping>()) {
				auto* angularDampingComponent = GetComponent<AngularDamping>(entity1id);
				angularDamping = angularDampingComponent->value_;
			}
			if (rbComponentsFilter.IsSet<LinearDamping>()) {
				auto* linearDampingComponent = GetComponent<LinearDamping>(entity1id);
				linearDamping = linearDampingComponent->value_;
			}

			PAL::DynamicRigidBody::CreateInfo createInfo{
				.rbCreateInfo_ = {
					.transform_ = glm::mat4{ glm::translate(glm::vec3(worldPosition3D1->x_, worldPosition3D1->y_, worldPosition3D1->z_)) },
					.shape_ = physicsShape1->shape_,
					.name_ = "DynamicRigidBody"
				},
				.mass_ = mass1->value_,
				.linearDamping_ = linearDamping,
				.angularDamping_ = angularDamping,
				.lockAngularX_ = rbComponentsFilter.IsSet<LockAngularX>(),
				.lockAngularY_ = rbComponentsFilter.IsSet<LockAngularY>(),
				.lockAngularZ_ = rbComponentsFilter.IsSet<LockAngularZ>()
			};
			PAL::DRB::Id drbId = world0->world_->CreateDynamicRigidBody(createInfo);
			world0->world_->AddDynamicRigidBody(drbId);

			CreateComponent<RigidBodyId>(entity1id, drbId);


		};

		void SetAngularVelocityToDynamicRigidBody::Update(
			ECS2::Entity::Id entity0id, Physics::World* world0, ECS2::Entity::Id entity1id,
			const Physics::DynamicRigidBody* dynamicRigidBody1, const Physics::RigidBodyId* rigidBodyId1,
			const Physics::PhysicsShape* physicsShape1,
			Physics::SetAngularVelocityRequests* setAngularVelocityRequests1) {

			if (setAngularVelocityRequests1->requests_.empty()) return;

			auto rb = Common::pointer_cast<PAL::DynamicRigidBody>(world0->world_->GetRigidBodyById(rigidBodyId1->id_));

			const SetAngularVelocityRequest& request = setAngularVelocityRequests1->requests_.back();
			rb->SetAngularVelocity(request.axis_, request.degreesInSecond_);

			setAngularVelocityRequests1->requests_.clear();

		}

		void SetVelocityToDynamicRigidBody::Update(
			ECS2::Entity::Id entity0id, Physics::World* world0, ECS2::Entity::Id entity1id,
			const Physics::DynamicRigidBody* dynamicRigidBody1, const Physics::RigidBodyId* rigidBodyId1,
			const Physics::PhysicsShape* physicsShape1, Physics::SetVelocityRequests* setVelocityRequests1) {

			if (setVelocityRequests1->requests_.empty()) return;

			auto rb = Common::pointer_cast<PAL::DynamicRigidBody>(world0->world_->GetRigidBodyById(rigidBodyId1->id_));

			const SetVelocityRequest& request = setVelocityRequests1->requests_.back();
			//rb->
			rb->SetLinearVelocity(request.direction_, request.velocity_);

			setVelocityRequests1->requests_.clear();

		}

		void AddVelocityToDynamicRigidBody::Update(
			ECS2::Entity::Id entity0id, Physics::World* world0, ECS2::Entity::Id entity1id,
			const Physics::DynamicRigidBody* dynamicRigidBody1, const Physics::RigidBodyId* rigidBodyId1,
			const Physics::PhysicsShape* physicsShape1, Physics::AddVelocityRequests* addVelocityRequests1) {

			if (addVelocityRequests1->requests_.empty()) return;

			auto rb = Common::pointer_cast<PAL::DynamicRigidBody>(world0->world_->GetRigidBodyById(rigidBodyId1->id_));

			const AddVelocityRequest& request = addVelocityRequests1->requests_.back();
			glm::vec3 linearVelocity = rb->GetLinearVelocity();

			for (auto& request : addVelocityRequests1->requests_) {
				request.direction_ = glm::normalize(request.direction_);
				request.direction_ *= request.velocity_;
				linearVelocity += request.direction_;
			}

			rb->SetLinearVelocity(glm::normalize(linearVelocity), linearVelocity.length());

			addVelocityRequests1->requests_.clear();

		}



		void SimulatePhysics::Update(
			ECS2::Entity::Id entity0id, const OksEngine::Clock* clock0,
			const OksEngine::FrameStartTimePoint* frameStartTimePoint0,
			const OksEngine::PreviousFrameDuration* previousFrameDuration0, ECS2::Entity::Id entity1id,
			OksEngine::Physics::Engine* engine1, const OksEngine::Physics::EnableSimulation* enableSimulation1,
			const OksEngine::Physics::SimulationGranularity* simulationGranularity1) {
			//using namespace std::chrono_literals;
			//static std::chrono::high_resolution_clock::time_point previousUpdate = std::chrono::high_resolution_clock::now();
			//static std::chrono::high_resolution_clock::duration remainder = 0ms;
			//const auto simulationGranularity = 3ms;
			//const auto now = std::chrono::high_resolution_clock::now();
			//const auto delta = (now - previousUpdate);
			//auto toSimulate = delta + remainder;
			//while (toSimulate >= simulationGranularity) {
			//	const Common::Size msToSimulate = simulationGranularity.count();
			//	engine1->engine_->GetWorld()->Simulate(msToSimulate / 1000.0);
			//	toSimulate -= simulationGranularity;
			//}
			//remainder = toSimulate;
			//previousUpdate = std::chrono::high_resolution_clock::now();
			const float fullMsToSimulate = previousFrameDuration0->microseconds_ / 1000.0f;
			float msLeftToSimulate = fullMsToSimulate;

			BEGIN_PROFILE("Simulate %f", fullMsToSimulate);
			while (msLeftToSimulate >= simulationGranularity1->ms_) {
				engine1->engine_->GetWorld()->Simulate(simulationGranularity1->ms_ / 1000.0f);
				msLeftToSimulate -= simulationGranularity1->ms_;
			}

			if (!Math::IsEqual(msLeftToSimulate / 1000.0f, 0.0f)) {
				engine1->engine_->GetWorld()->Simulate(msLeftToSimulate / 1000.0f);
			}
			END_PROFILE();
		};

	}

} // namespace OksEngine