#pragma once
#include <Physics\auto_OksEngine.Physics.Debug.hpp>

namespace OksEngine
{
	AI_GENERATED
		struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
	};

	AI_GENERATED
		std::vector<Vertex> generateCapsuleVertices(float radius, float height, int segments, int rings) {
		std::vector<Vertex> vertices;

		// Общая высота капсулы = высота цилиндра + 2 * радиус полусфер
		float cylinderHeight = height - 2.0f * radius;
		if (cylinderHeight < 0) cylinderHeight = 0; // если высота меньше диаметра

		// 1. Генерация верхней полусферы
		for (int i = 0; i <= rings; ++i) {
			float phi = (float)i / rings * (3.14 / 2.0f); // от 0 до π/2
			float y = cos(phi) * radius + cylinderHeight / 2.0f;
			float sliceRadius = sin(phi) * radius;

			for (int j = 0; j <= segments; ++j) {
				float theta = (float)j / segments * 2.0f * 3.14;

				Vertex vertex;
				vertex.position.x = sliceRadius * cos(theta);
				vertex.position.z = sliceRadius * sin(theta);
				vertex.position.y = y;

				// Нормаль для полусферы (направлена от центра)
				vertex.normal = glm::normalize(vertex.position - glm::vec3(0, cylinderHeight / 2, 0));

				//// Текстурные координаты (опционально)
				//vertex.texCoord.x = (float)j / segments;
				//vertex.texCoord.y = (float)i / rings * 0.5f; // верхняя половина текстуры

				vertices.push_back(vertex);
			}
		}

		// 2. Генерация цилиндрической части
		int cylinderRings = rings / 2; // количество разбиений по высоте цилиндра
		for (int i = 0; i <= cylinderRings; ++i) {
			float y = cylinderHeight / 2.0f - (float)i / cylinderRings * cylinderHeight;

			for (int j = 0; j <= segments; ++j) {
				float theta = (float)j / segments * 2.0f * 3.14;

				Vertex vertex;
				vertex.position.x = radius * cos(theta);
				vertex.position.z = radius * sin(theta);
				vertex.position.y = y;

				// Нормаль для цилиндра (перпендикулярна оси Y)
				vertex.normal = glm::normalize(glm::vec3(cos(theta), 0, sin(theta)));

				//// Текстурные координаты
				//vertex.texCoord.x = (float)j / segments;
				//vertex.texCoord.y = 0.5f + (float)i / cylinderRings * 0.5f;

				vertices.push_back(vertex);
			}
		}

		// 3. Генерация нижней полусферы
		for (int i = 0; i <= rings; ++i) {
			float phi = (float)i / rings * (3.14 / 2.0f); // от 0 до π/2
			float y = -cos(phi) * radius - cylinderHeight / 2.0f;
			float sliceRadius = sin(phi) * radius;

			for (int j = 0; j <= segments; ++j) {
				float theta = (float)j / segments * 2.0f * 3.14;

				Vertex vertex;
				vertex.position.x = sliceRadius * cos(theta);
				vertex.position.z = sliceRadius * sin(theta);
				vertex.position.y = y;

				// Нормаль для нижней полусферы
				vertex.normal = glm::normalize(vertex.position - glm::vec3(0, -cylinderHeight / 2, 0));

				//// Текстурные координаты
				//vertex.texCoord.x = (float)j / segments;
				//vertex.texCoord.y = 1.0f - (float)i / rings * 0.5f;

				vertices.push_back(vertex);
			}
		}

		return vertices;
	}
	AI_GENERATED
		std::vector<Vertex> generateCapsuleTriangles(float radius, float height, int segments, int rings) {
		auto vertices = generateCapsuleVertices(radius, height, segments, rings);
		std::vector<Vertex> triangles;

		int totalRings = rings * 2 + rings / 2; // полусферы + цилиндр

		// Создаем треугольники из вершин
		for (int i = 0; i < totalRings; ++i) {
			for (int j = 0; j < segments; ++j) {
				// Индексы четырех вершин текущего квада
				int i0 = i * (segments + 1) + j;
				int i1 = i * (segments + 1) + j + 1;
				int i2 = (i + 1) * (segments + 1) + j;
				int i3 = (i + 1) * (segments + 1) + j + 1;

				// Проверяем, что индексы в пределах массива
				if (i2 < vertices.size() && i3 < vertices.size()) {
					// Первый треугольник
					triangles.push_back(vertices[i0]);
					triangles.push_back(vertices[i1]);
					triangles.push_back(vertices[i2]);

					// Второй треугольник
					triangles.push_back(vertices[i1]);
					triangles.push_back(vertices[i3]);
					triangles.push_back(vertices[i2]);
				}
			}
		}

		return triangles;
	}

	namespace Physics
	{
		namespace Debug
		{

			void UpdateVisualizationParameters::Update(ECS2::Entity::Id entity0id, OksEngine::Physics::World* world0, ECS2::Entity::Id entity1id,
				const OksEngine::Physics::Debug::Debugger* debugger1,
				const OksEngine::Physics::Debug::EnableDebugRender* enableDebugRender1,
				const OksEngine::Physics::Debug::CommonParameters* commonParameters1,
				const OksEngine::Physics::Debug::ContactPointsParameters* contactPointsParameters1,
				const OksEngine::Physics::Debug::ConstraintsParameters* constraintsParameters1) {

				world0->world_->SetDebugRenderParameters(PAL::World::DebugRenderParameters::eCOLLISION_AABBS, commonParameters1->drawAABB_);

				world0->world_->SetDebugRenderParameters(PAL::World::DebugRenderParameters::eCOLLISION_FNORMALS, commonParameters1->drawNormals_);

				world0->world_->SetDebugRenderParameters(PAL::World::DebugRenderParameters::eCOLLISION_EDGES, commonParameters1->drawEdges_);

				world0->world_->SetDebugRenderParameters(PAL::World::DebugRenderParameters::eBODY_AXES, commonParameters1->drawBodyAxes_);

				world0->world_->SetDebugRenderParameters(PAL::World::DebugRenderParameters::eACTOR_AXES, commonParameters1->drawActorAxes_);

				world0->world_->SetDebugRenderParameters(PAL::World::DebugRenderParameters::eWORLD_AXES, commonParameters1->drawWorldAxes_);

				world0->world_->SetDebugRenderParameters(PAL::World::DebugRenderParameters::eCOLLISION_STATIC, commonParameters1->drawCollisionStatic_);

				world0->world_->SetDebugRenderParameters(PAL::World::DebugRenderParameters::eCOLLISION_DYNAMIC, commonParameters1->drawCollisionDynamic_);

				world0->world_->SetDebugRenderParameters(PAL::World::DebugRenderParameters::eCONTACT_POINT, contactPointsParameters1->drawContactPoints_);

				world0->world_->SetDebugRenderParameters(PAL::World::DebugRenderParameters::eCONTACT_NORMAL, contactPointsParameters1->drawContactNormal_);

				world0->world_->SetDebugRenderParameters(PAL::World::DebugRenderParameters::eJOINT_LOCAL_FRAMES, constraintsParameters1->drawJointLocalFrames_);

				world0->world_->SetDebugRenderParameters(PAL::World::DebugRenderParameters::eJOINT_LIMITS, constraintsParameters1->drawJointLimits_);

			}

			void AddToRenderDynamicRigidBodyShape::Update(ECS2::Entity::Id entity0id, const OksEngine::Render::Debug::Debugger* render__Debug__Debugger0,
				OksEngine::Render::Debug::FlatShade::VertexBuffer* render__Debug__FlatShade__VertexBuffer0,
				ECS2::Entity::Id entity1id, const OksEngine::Physics::Debug::Debugger* debugger1,
				const OksEngine::Physics::Debug::EnableDebugRender* enableDebugRender1,
				const OksEngine::Physics::Debug::CommonParameters* commonParameters1,
				const OksEngine::Physics::Debug::ContactPointsParameters* contactPointsParameters1,
				const OksEngine::Physics::Debug::ConstraintsParameters* constraintsParameters1,
				ECS2::Entity::Id entity2id, const OksEngine::WorldPosition3D* worldPosition3D2,
				const OksEngine::WorldRotation3D* worldRotation3D2, const OksEngine::WorldScale3D* worldScale3D2,
				const OksEngine::Physics::DynamicRigidBody* dynamicRigidBody2,
				const OksEngine::Physics::RigidBodyId* rigidBodyId2,
				const OksEngine::Physics::PhysicsShape* physicsShape2) {

				const glm::vec3 translate{ worldPosition3D2->x_, worldPosition3D2->y_, worldPosition3D2->z_ };
				const glm::quat rotation{ worldRotation3D2->w_, worldRotation3D2->x_, worldRotation3D2->y_, worldRotation3D2->z_ };
				const glm::vec3 scale{ worldScale3D2->x_, worldScale3D2->y_, worldScale3D2->z_ };

				const ECS2::ComponentsFilter filter = GetComponentsFilter(entity2id);

				Geom::VertexCloud<Geom::Vertex3fc> vertices;

				if (filter.IsSet<Physics::ShapeGeometryData>()) {
					const auto* shapeData = GetComponent<Physics::ShapeGeometryData>(entity2id);

					ASSERT((shapeData->indices_.size() % 3) == 0);

					for (Common::Index i = 0; i < shapeData->indices_.size(); i += 3) {

						const Common::UInt16 index0 = shapeData->indices_[i];
						const Common::UInt16 index1 = shapeData->indices_[i + 1];
						const Common::UInt16 index2 = shapeData->indices_[i + 2];

						const Physics::ShapeVertex& shapeVertex0 = shapeData->vertices_[index0];
						const Physics::ShapeVertex& shapeVertex1 = shapeData->vertices_[index1];
						const Physics::ShapeVertex& shapeVertex2 = shapeData->vertices_[index2];

						const glm::vec3 v0 = glm::vec3{ shapeVertex0.x_, shapeVertex0.y_, shapeVertex0.z_ };
						const glm::vec3 v1 = glm::vec3{ shapeVertex1.x_, shapeVertex1.y_, shapeVertex1.z_ };
						const glm::vec3 v2 = glm::vec3{ shapeVertex2.x_, shapeVertex2.y_, shapeVertex2.z_ };

						//const glm::vec3 edge0 = v1 - v0;
						//const glm::vec3 edge1 = v2 - v0;

						//const glm::vec3 faceNormal = glm::cross(edge0, edge1);

						const glm::vec3 scale{ worldScale3D2->x_, worldScale3D2->y_, worldScale3D2->z_ };

						const glm::vec3 transformedV0 = translate + rotation * (scale * v0);
						const glm::vec3 transformedV1 = translate + rotation * (scale * v1);
						const glm::vec3 transformedV2 = translate + rotation * (scale * v2);

						vertices.Add(
							{
								Geom::Vector3f{ transformedV0.x,transformedV0.y, transformedV0.z },
								/*Geom::Normal3f{ faceNormal.x, faceNormal.y, faceNormal.z },*/
								Geom::Color3f{ 0.0, 1.0, 0.0 }
							});

						vertices.Add(
							{
								Geom::Vector3f{ transformedV1.x,transformedV1.y, transformedV1.z },
								/*Geom::Normal3f{ faceNormal.x, faceNormal.y, faceNormal.z },*/
								Geom::Color3f{ 0.0, 1.0, 0.0 }
							});

						vertices.Add(
							{
								Geom::Vector3f{ transformedV2.x,transformedV2.y, transformedV2.z },
								/*Geom::Normal3f{ faceNormal.x, faceNormal.y, faceNormal.z },*/
								Geom::Color3f{ 0.0, 1.0, 0.0 }
							});

					}

					render__Debug__FlatShade__VertexBuffer0->vertices_.Add(vertices.GetData(), vertices.GetVerticesNumber());

				}
				else if (filter.IsSet<Physics::ShapeGeometryCapsule>()) {
					const auto* shapeData = GetComponent<Physics::ShapeGeometryCapsule>(entity2id);
					auto capsuleVertices = generateCapsuleTriangles(shapeData->radius_, shapeData->height_, shapeData->segments_, shapeData->rings_);

					for (Common::Index i = 0; i < capsuleVertices.size(); i += 3) {

						const Vertex& shapeVertex0 = capsuleVertices[i];
						const Vertex& shapeVertex1 = capsuleVertices[i + 1];
						const Vertex& shapeVertex2 = capsuleVertices[i + 2];

						const glm::vec3 v0 = shapeVertex0.position;
						const glm::vec3 v1 = shapeVertex1.position;
						const glm::vec3 v2 = shapeVertex2.position;

						//const glm::vec3 edge0 = v1 - v0;
						//const glm::vec3 edge1 = v2 - v0;

						//const glm::vec3 faceNormal = glm::cross(edge0, edge1);



						const glm::vec3 transformedV0 = translate + rotation * (scale * v0);
						const glm::vec3 transformedV1 = translate + rotation * (scale * v1);
						const glm::vec3 transformedV2 = translate + rotation * (scale * v2);

						vertices.Add(
							{
								Geom::Vector3f{ transformedV0.x,transformedV0.y, transformedV0.z },
								/*Geom::Normal3f{ faceNormal.x, faceNormal.y, faceNormal.z },*/
								Geom::Color3f{ 0.0, 1.0, 0.0 }
							});

						vertices.Add(
							{
								Geom::Vector3f{ transformedV1.x,transformedV1.y, transformedV1.z },
								/*Geom::Normal3f{ faceNormal.x, faceNormal.y, faceNormal.z },*/
								Geom::Color3f{ 0.0, 1.0, 0.0 }
							});

						vertices.Add(
							{
								Geom::Vector3f{ transformedV2.x,transformedV2.y, transformedV2.z },
								/*Geom::Normal3f{ faceNormal.x, faceNormal.y, faceNormal.z },*/
								Geom::Color3f{ 0.0, 1.0, 0.0 }
							});

					}

					render__Debug__FlatShade__VertexBuffer0->vertices_.Add(vertices.GetData(), vertices.GetVerticesNumber());

				}
				else if (filter.IsSet<Physics::ShapeGeometryBox>()) {


					const auto* shapeData = GetComponent<Physics::ShapeGeometryBox>(entity2id);

					float halfW = shapeData->lenght_ * 0.5f;
					float halfH = shapeData->height_ * 0.5f;
					float halfD = shapeData->width_ * 0.5f;

					// 8 уникальных вершин куба
					Geom::Vertex3fc uniqueVertices[8] = {
						// Передняя грань
						{ Geom::Vertex3f(-halfW, -halfH,  halfD), Geom::Color3f{ 0.0, 1.0, 0.0 }},// 0: лево-низ-перед
						{Geom::Vertex3f(halfW, -halfH,  halfD), Geom::Color3f{ 0.0, 1.0, 0.0 } }, // 1: право-низ-перед
						{Geom::Vertex3f(halfW,  halfH,  halfD), Geom::Color3f{ 0.0, 1.0, 0.0 } },// 2: право-верх-перед
						{Geom::Vertex3f(-halfW,  halfH,  halfD),Geom::Color3f{ 0.0, 1.0, 0.0 } }, // 3: лево-верх-перед

						// Задняя грань
						{Geom::Vertex3f(-halfW, -halfH, -halfD),Geom::Color3f{ 0.0, 1.0, 0.0 } }, // 4: лево-низ-зад
						{Geom::Vertex3f(halfW, -halfH, -halfD), Geom::Color3f{ 0.0, 1.0, 0.0 } },// 5: право-низ-зад
						{Geom::Vertex3f(halfW,  halfH, -halfD), Geom::Color3f{ 0.0, 1.0, 0.0 } },// 6: право-верх-зад
						{Geom::Vertex3f(-halfW,  halfH, -halfD), Geom::Color3f{ 0.0, 1.0, 0.0 } } // 7: лево-верх-зад
					};

					// 12 треугольников (2 на каждую грань) напрямую как вершины
					Geom::VertexCloud<Geom::Vertex3fc> result;

					Geom::Vertex3fc boxVertices[] = {
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

					for (Common::Index i = 0; i < 36; i++) {
						glm::vec3 position = glm::vec3{ boxVertices[i].position_.GetX(),  boxVertices[i].position_.GetY(), boxVertices[i].position_.GetZ() };
						const glm::vec3 transformedV0 = translate + rotation * (scale * position);
						boxVertices[i].position_.GetX() = transformedV0.x;
						boxVertices[i].position_.GetY() = transformedV0.y;
						boxVertices[i].position_.GetZ() = transformedV0.z;
					}

					result.Add(boxVertices, 36);



					render__Debug__FlatShade__VertexBuffer0->vertices_.Add(result.GetData(), result.GetVerticesNumber());

				}
				else if (filter.IsSet<Physics::ShapeGeometryCylinder>()) {
					const auto* shapeData = GetComponent<Physics::ShapeGeometryCylinder>(entity2id);
					float numSides = 16;
					AI_GENERATED
						std::vector<glm::vec3> cylinderVertices;
					cylinderVertices.reserve(numSides * 12);

					float halfHeight = shapeData->height_ * 0.5f;

					// Боковая поверхность
					for (int i = 0; i < numSides; ++i) {
						float a1 = 2.0f * glm::pi<float>() * i / numSides;
						float a2 = 2.0f * glm::pi<float>() * (i + 1) / numSides;

						glm::vec3 v0(shapeData->radius_ * cos(a1), -halfHeight, shapeData->radius_ * sin(a1));
						glm::vec3 v1(shapeData->radius_ * cos(a2), -halfHeight, shapeData->radius_ * sin(a2));
						glm::vec3 v2(shapeData->radius_ * cos(a2), halfHeight, shapeData->radius_ * sin(a2));
						glm::vec3 v3(shapeData->radius_ * cos(a1), halfHeight, shapeData->radius_ * sin(a1));

						// Два треугольника на сегмент
						cylinderVertices.push_back(v2);

						cylinderVertices.push_back(v1);
						cylinderVertices.push_back(v0);

						cylinderVertices.push_back(v0);
						cylinderVertices.push_back(v3);
						cylinderVertices.push_back(v2);
					}

					// Верхняя крышка
					glm::vec3 topCenter(0.0f, halfHeight, 0.0f);
					for (int i = 0; i < numSides; ++i) {
						float a1 = 2.0f * glm::pi<float>() * i / numSides;
						float a2 = 2.0f * glm::pi<float>() * (i + 1) / numSides;

						cylinderVertices.push_back(glm::vec3(shapeData->radius_ * cos(a2), halfHeight, shapeData->radius_ * sin(a2)));
						cylinderVertices.push_back(glm::vec3(shapeData->radius_ * cos(a1), halfHeight, shapeData->radius_ * sin(a1)));

						cylinderVertices.push_back(topCenter);
					}

					// Нижняя крышка
					glm::vec3 bottomCenter(0.0f, -halfHeight, 0.0f);
					for (int i = 0; i < numSides; ++i) {
						float a1 = 2.0f * glm::pi<float>() * i / numSides;
						float a2 = 2.0f * glm::pi<float>() * (i + 1) / numSides;

						cylinderVertices.push_back(glm::vec3(shapeData->radius_ * cos(a1), -halfHeight, shapeData->radius_ * sin(a1)));
						cylinderVertices.push_back(glm::vec3(shapeData->radius_ * cos(a2), -halfHeight, shapeData->radius_ * sin(a2)));
						cylinderVertices.push_back(bottomCenter);
					}

					for (Common::Index i = 0; i < cylinderVertices.size(); i++) {
						glm::vec3 position = glm::vec3{ cylinderVertices[i].x,  cylinderVertices[i].y, cylinderVertices[i].z };
						const glm::vec3 transformedV0 = translate + rotation * (scale * position);

						render__Debug__FlatShade__VertexBuffer0->vertices_.Add(Geom::Vertex3fc{ {transformedV0.x, transformedV0.y, transformedV0.z}, Geom::Color3f{ 0.0, 1.0, 0.0 } });

					}
				}
			};

			void AddToRenderStaticRigidBodyShape::Update(ECS2::Entity::Id entity0id, const OksEngine::Physics::Debug::Debugger* debugger0,
				const OksEngine::Physics::Debug::EnableDebugRender* enableDebugRender0,
				const OksEngine::Physics::Debug::CommonParameters* commonParameters0,
				const OksEngine::Physics::Debug::ConstraintsParameters* constraintsParameters0,
				ECS2::Entity::Id entity1id, const OksEngine::Render::Debug::Debugger* render__Debug__Debugger1,
				OksEngine::Render::Debug::FlatShade::VertexBuffer* render__Debug__VertexBuffer1, ECS2::Entity::Id entity2id,
				const OksEngine::WorldPosition3D* worldPosition3D2, const OksEngine::WorldRotation3D* worldRotation3D2,
				const OksEngine::WorldScale3D* worldScale3D2,
				const OksEngine::Physics::StaticRigidBody* staticRigidBody2,
				const OksEngine::Physics::RigidBodyId* rigidBodyId2,
				const OksEngine::Physics::PhysicsShape* physicsShape2) {


				const glm::vec3 translate{ worldPosition3D2->x_, worldPosition3D2->y_, worldPosition3D2->z_ };
				const glm::quat rotation{ worldRotation3D2->w_, worldRotation3D2->x_, worldRotation3D2->y_, worldRotation3D2->z_ };


				const ECS2::ComponentsFilter filter = GetComponentsFilter(entity2id);

				Geom::VertexCloud<Geom::Vertex3fc> vertices;

				if (filter.IsSet<Physics::ShapeGeometryData>()) {
					const auto* shapeData = GetComponent<Physics::ShapeGeometryData>(entity2id);

					ASSERT((shapeData->indices_.size() % 3) == 0);

					for (Common::Index i = 0; i < shapeData->indices_.size(); i += 3) {

						const Common::UInt16 index0 = shapeData->indices_[i];
						const Common::UInt16 index1 = shapeData->indices_[i + 1];
						const Common::UInt16 index2 = shapeData->indices_[i + 2];

						const Physics::ShapeVertex& shapeVertex0 = shapeData->vertices_[index0];
						const Physics::ShapeVertex& shapeVertex1 = shapeData->vertices_[index1];
						const Physics::ShapeVertex& shapeVertex2 = shapeData->vertices_[index2];

						const glm::vec3 v0 = glm::vec3{ shapeVertex0.x_, shapeVertex0.y_, shapeVertex0.z_ };
						const glm::vec3 v1 = glm::vec3{ shapeVertex1.x_, shapeVertex1.y_, shapeVertex1.z_ };
						const glm::vec3 v2 = glm::vec3{ shapeVertex2.x_, shapeVertex2.y_, shapeVertex2.z_ };

						//const glm::vec3 edge0 = v1 - v0;
						//const glm::vec3 edge1 = v2 - v0;

						//const glm::vec3 faceNormal = glm::cross(edge0, edge1);

						const glm::vec3 scale{ worldScale3D2->x_, worldScale3D2->y_, worldScale3D2->z_ };

						const glm::vec3 transformedV0 = translate + rotation * (scale * v0);
						const glm::vec3 transformedV1 = translate + rotation * (scale * v1);
						const glm::vec3 transformedV2 = translate + rotation * (scale * v2);

						vertices.Add(
							{
								Geom::Vector3f{ transformedV0.x,transformedV0.y, transformedV0.z },
								/*Geom::Normal3f{ faceNormal.x, faceNormal.y, faceNormal.z },*/
								Geom::Color3f{ 0.5, 0.5, 0.5 }
							});

						vertices.Add(
							{
								Geom::Vector3f{ transformedV1.x,transformedV1.y, transformedV1.z },
								/*Geom::Normal3f{ faceNormal.x, faceNormal.y, faceNormal.z },*/
								Geom::Color3f{ 0.5, 0.5, 0.5 }
							});

						vertices.Add(
							{
								Geom::Vector3f{ transformedV2.x,transformedV2.y, transformedV2.z },
								/*Geom::Normal3f{ faceNormal.x, faceNormal.y, faceNormal.z },*/
								Geom::Color3f{ 0.5, 0.5, 0.5 }
							});

					}

					render__Debug__VertexBuffer1->vertices_.Add(vertices.GetData(), vertices.GetVerticesNumber());

				}


			};

			void AddToRenderRevoluteConstraint::Update(ECS2::Entity::Id entity0id, const OksEngine::Physics::Debug::Debugger* debugger0,
				const OksEngine::Physics::Debug::EnableDebugRender* enableDebugRender0,
				const OksEngine::Physics::Debug::CommonParameters* commonParameters0,
				const OksEngine::Physics::Debug::ContactPointsParameters* contactPointsParameters0,
				const OksEngine::Physics::Debug::ConstraintsParameters* constraintsParameters0, ECS2::Entity::Id entity1id,
				const OksEngine::Render::Debug::Debugger* render__Debug__Debugger1,
				OksEngine::Render::Debug::Lines::VertexBuffer* render__Debug__Lines__VertexBuffer1, ECS2::Entity::Id entity2id,
				const OksEngine::Physics::Constraint* constraint2, const OksEngine::Physics::ConstraintId* constraintId2,
				const OksEngine::Physics::RevoluteConstraint* revoluteConstraint2,
				const OksEngine::Physics::RevoluteAxis* revoluteAxis2,
				const OksEngine::Physics::FirstConstraintedRigidBodyEntityId* firstConstraintedRigidBodyEntityId2,
				const OksEngine::Physics::SecondConstraintedRigidBodyEntityId* secondConstraintedRigidBodyEntityId2,
				const OksEngine::Physics::FirstConstraintedRigidBodyJointPosition3D* firstConstraintedRigidBodyJointPosition3D2,
				const OksEngine::Physics::FirstConstraintedRigidBodyJointRotation3D* firstConstraintedRigidBodyJointRotation3D2,
				const OksEngine::Physics::SecondConstraintedRigidBodyJointPosition3D
				* secondConstraintedRigidBodyJointPosition3D2,
				const OksEngine::Physics::SecondConstraintedRigidBodyJointRotation3D
				* secondConstraintedRigidBodyJointRotation3D2) {

				const ECS2::ComponentsFilter firstRBFilter = GetComponentsFilter(firstConstraintedRigidBodyEntityId2->id_);
				const ECS2::ComponentsFilter secondRBFilter = GetComponentsFilter(secondConstraintedRigidBodyEntityId2->id_);

				if (!firstRBFilter.IsSet<RigidBodyId>() || !secondRBFilter.IsSet<RigidBodyId>()) {
					return;
				}

				{
					glm::vec3 xAxis{ 3.0, 0.0, 0.0 };
					glm::vec3 yAxis{ 0.0, 3.0, 0.0 };
					glm::vec3 zAxis{ 0.0, 0.0, 3.0 };

					Geom::VertexCloud<Geom::Vertex3fc> lines;

					auto firstRBPosition3D = GetComponent<WorldPosition3D>(firstConstraintedRigidBodyEntityId2->id_);
					auto firstRBRotation3D = GetComponent<WorldRotation3D>(firstConstraintedRigidBodyEntityId2->id_);

					glm::vec3 worldPosition{ firstRBPosition3D->x_, firstRBPosition3D->y_, firstRBPosition3D->z_ };
					glm::quat worldRotation{ firstRBRotation3D->w_, firstRBRotation3D->x_, firstRBRotation3D->y_, firstRBRotation3D->z_ };

					{
						glm::vec3 start = worldPosition;
						glm::vec3 endX = worldPosition + (worldRotation * xAxis);

						lines.Add(Geom::Vertex3fc{
							{ start.x, start.y, start.z },
							Geom::Color3f{ 1.0, 0.0, 0.0 }
							});

						lines.Add(Geom::Vertex3fc{
							{ endX.x, endX.y, endX.z },
							Geom::Color3f{ 1.0, 0.0, 0.0 }
							});

						glm::vec3 endY = worldPosition + (worldRotation * yAxis);

						lines.Add(Geom::Vertex3fc{
							{ start.x, start.y, start.z },
							Geom::Color3f{ 0.0, 1.0, 0.0 }
							});

						lines.Add(Geom::Vertex3fc{
							{ endY.x, endY.y, endY.z },
							Geom::Color3f{ 0.0, 1.0, 0.0 }
							});

						glm::vec3 endZ = worldPosition + (worldRotation * zAxis);

						lines.Add(Geom::Vertex3fc{
							{ start.x, start.y, start.z },
							Geom::Color3f{ 0.0, 1.0, 0.0 }
							});

						lines.Add(Geom::Vertex3fc{
							{ endZ.x, endZ.y, endZ.z },
							Geom::Color3f{ 0.0, 0.0, 1.0 }
							});

						render__Debug__Lines__VertexBuffer1->vertices_.Add(lines.GetData(), lines.GetVerticesNumber());
					}

					glm::quat localRotation{
						firstConstraintedRigidBodyJointRotation3D2->w_,
						firstConstraintedRigidBodyJointRotation3D2->x_,
						firstConstraintedRigidBodyJointRotation3D2->y_,
						firstConstraintedRigidBodyJointRotation3D2->z_ };

					glm::quat rotation = worldRotation * localRotation;

					glm::vec3 position =
						worldPosition + worldRotation * glm::vec3{
						firstConstraintedRigidBodyJointPosition3D2->x_,
						firstConstraintedRigidBodyJointPosition3D2->y_,
						firstConstraintedRigidBodyJointPosition3D2->z_ };




					//Axis start
					render__Debug__Lines__VertexBuffer1->vertices_.Add(
						{
							Geom::Vertex3f{ position.x, position.y, position.z},
							Geom::Color3f{ 1.0, 0.0, 0.0 }
						}
					);

					//Axis end
					glm::vec3 axisEnd = position + (rotation * glm::vec3{ 1.0, 0.0, 0.0 });
					render__Debug__Lines__VertexBuffer1->vertices_.Add(
						{
							Geom::Vertex3f{axisEnd.x, axisEnd.y, axisEnd.z},
							Geom::Color3f{ 1.0, 1.0, 1.0 }
						}
					);
				}
				{
					auto firstRBPosition3D = GetComponent<WorldPosition3D>(secondConstraintedRigidBodyEntityId2->id_);
					auto firstRBRotation3D = GetComponent<WorldRotation3D>(secondConstraintedRigidBodyEntityId2->id_);

					glm::vec3 xAxis{ 3.0, 0.0, 0.0 };
					glm::vec3 yAxis{ 0.0, 3.0, 0.0 };
					glm::vec3 zAxis{ 0.0, 0.0, 3.0 };

					Geom::VertexCloud<Geom::Vertex3fc> lines;

					glm::vec3 worldPosition{ firstRBPosition3D->x_, firstRBPosition3D->y_, firstRBPosition3D->z_ };
					glm::quat worldRotation{ firstRBRotation3D->w_, firstRBRotation3D->x_, firstRBRotation3D->y_, firstRBRotation3D->z_ };
					{
						glm::vec3 start = worldPosition;
						glm::vec3 endX = worldPosition + (worldRotation * xAxis);

						lines.Add(Geom::Vertex3fc{
							{ start.x, start.y, start.z },
							Geom::Color3f{ 1.0, 0.0, 0.0 }
							});

						lines.Add(Geom::Vertex3fc{
							{ endX.x, endX.y, endX.z },
							Geom::Color3f{ 1.0, 0.0, 0.0 }
							});

						glm::vec3 endY = worldPosition + (worldRotation * yAxis);

						lines.Add(Geom::Vertex3fc{
							{ start.x, start.y, start.z },
							Geom::Color3f{ 0.0, 1.0, 0.0 }
							});

						lines.Add(Geom::Vertex3fc{
							{ endY.x, endY.y, endY.z },
							Geom::Color3f{ 0.0, 1.0, 0.0 }
							});

						glm::vec3 endZ = worldPosition + (worldRotation * zAxis);

						lines.Add(Geom::Vertex3fc{
							{ start.x, start.y, start.z },
							Geom::Color3f{ 0.0, 0.0, 1.0 }
							});

						lines.Add(Geom::Vertex3fc{
							{ endZ.x, endZ.y, endZ.z },
							Geom::Color3f{ 0.0, 0.0, 1.0 }
							});

						render__Debug__Lines__VertexBuffer1->vertices_.Add(lines.GetData(), lines.GetVerticesNumber());
					}
					glm::quat localRotation{
						secondConstraintedRigidBodyJointRotation3D2->w_,
						secondConstraintedRigidBodyJointRotation3D2->x_,
						secondConstraintedRigidBodyJointRotation3D2->y_,
						secondConstraintedRigidBodyJointRotation3D2->z_ };

					glm::quat rotation = worldRotation * localRotation;
					glm::vec3 position =
						glm::vec3{ firstRBPosition3D->x_, firstRBPosition3D->y_, firstRBPosition3D->z_ }
						+ worldRotation * glm::vec3{
							secondConstraintedRigidBodyJointPosition3D2->x_,
							secondConstraintedRigidBodyJointPosition3D2->y_,
							secondConstraintedRigidBodyJointPosition3D2->z_ };

					auto* requests = GetComponent<SetAngularVelocityRequests>(secondConstraintedRigidBodyEntityId2->id_);
					requests->requests_.push_back({ glm::vec3{ 1.0, 0.0, 0.0 }, -50.0 });
					//Axis start
					render__Debug__Lines__VertexBuffer1->vertices_.Add(
						{
							Geom::Vertex3f{ position.x, position.y, position.z},
							Geom::Color3f{ 1.0, 0.0, 0.0 }
						}
					);

					//Axis end
					glm::vec3 axisEnd = position + (rotation * glm::vec3{ 1.0, 0.0, 0.0 });
					render__Debug__Lines__VertexBuffer1->vertices_.Add(
						{
							Geom::Vertex3f{axisEnd.x, axisEnd.y, axisEnd.z},
							Geom::Color3f{ 1.0, 1.0, 1.0 }
						}
					);
				}
				//auto& firstRB


			};

			void RenderPhysXDebugRenderBuffer::Update(ECS2::Entity::Id entity0id, const OksEngine::Physics::World* world0, ECS2::Entity::Id entity1id,
				const OksEngine::Render::Debug::Debugger* render__Debug__Debugger1,
				OksEngine::Render::Debug::FlatShade::VertexBuffer* render__Debug__FlatShade__VertexBuffer1,
				OksEngine::Render::Debug::Lines::VertexBuffer* render__Debug__Lines__VertexBuffer1,
				ECS2::Entity::Id entity2id, const OksEngine::Physics::Debug::Debugger* debugger2,
				const OksEngine::Physics::Debug::EnableDebugRender* enableDebugRender2,
				const OksEngine::Physics::Debug::CommonParameters* commonParameters2,
				const OksEngine::Physics::Debug::ContactPointsParameters* contactPointsParameters2,
				const OksEngine::Physics::Debug::ConstraintsParameters* constraintsParameters2) {

				auto bufferLines = world0->world_->GetDebugRenderBufferLines();
				if (bufferLines.GetVerticesNumber() > 0) {
					render__Debug__Lines__VertexBuffer1->vertices_.Add(bufferLines.GetData(), bufferLines.GetVerticesNumber());
				}

				auto buffer = world0->world_->GetDebugRenderBufferTriangles();
				if (buffer.GetVerticesNumber() > 0) {
					render__Debug__FlatShade__VertexBuffer1->vertices_.Add(buffer.GetData(), buffer.GetVerticesNumber());
				}
			}


			//For test render phys bodies.
			void AddRigidBodyToSelection::Update(

				ECS2::Entity::Id entity0id,
				const OksEngine::Render::Outline::State* state0,
				const OksEngine::Render::Outline::EnableSelection* enableSelection0,
				const OksEngine::CursorEvents* cursorEvents0,
				const OksEngine::MouseEvents* mouseEvents0,

				ECS2::Entity::Id entity1id,
				OksEngine::RenderDriver* renderDriver1,
				const OksEngine::Render::Outline::IdsTextureRender::RenderPassId* renderPassId1,
				const OksEngine::Render::Outline::IdsTextureRender::PipelineId* pipelineId1,
				const OksEngine::Render::Outline::DataStorageBuffer* dataStorageBuffer1,
				const OksEngine::Render::Outline::DataStorageBufferResource* dataStorageBufferResource1,

				ECS2::Entity::Id entity2id,
				const OksEngine::MainWindow* mainWindow2,
				const OksEngine::MainWindowFramebufferSize* mainWindowFramebufferSize2,
				const OksEngine::MainWindowWorkAreaSize* mainWindowWorkAreaSize2,

				ECS2::Entity::Id entity3id,
				const OksEngine::Camera* camera3, const OksEngine::Active* active3,
				const OksEngine::DriverViewProjectionUniformBuffer* driverViewProjectionUniformBuffer3,
				const OksEngine::CameraTransformResource* cameraTransformResource3,


				ECS2::Entity::Id entity4id,
				const OksEngine::WorldPosition3D* worldPosition3D4, const OksEngine::WorldRotation3D* worldRotation3D4,
				const OksEngine::WorldScale3D* worldScale3D4,
				const OksEngine::Physics::DynamicRigidBody* physics__DynamicRigidBody4,
				const OksEngine::Physics::RigidBodyId* physics__RigidBodyId4,
				const OksEngine::Physics::PhysicsShape* physics__PhysicsShape4,
				const OksEngine::DriverVertexBuffer* driverVertexBuffer4,
				const OksEngine::DriverIndexBuffer* driverIndexBuffer4, const OksEngine::Vertices3D* vertices3D4,
				const OksEngine::Indices* indices4) {

				Render::Outline::IdsTextureRender::MeshInfo meshInfo{
					entity4id.GetRawValue(),
					worldPosition3D4->x_,
					worldPosition3D4->y_,
					worldPosition3D4->z_,
					worldRotation3D4->w_,
					worldRotation3D4->x_,
					worldRotation3D4->y_,
					worldRotation3D4->z_
				};

				auto driver = renderDriver1->driver_;

				Render::Outline::Data data{
					-1,
					-1,
					0,
					{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0 }
				};

				if (!cursorEvents0->events_.empty()) {


					float scaleX = 2560/*(float)mainWindowFramebufferSize1->width_*/ / (float)mainWindowWorkAreaSize2->width_;
					float scaleY = 1440/*(float)mainWindowFramebufferSize1->height_*/ / (float)mainWindowWorkAreaSize2->height_;

					// Вариант 1: Координаты в пикселях фреймбуфера (для прямого сравнения с gl_FragCoord)
					data.cursorPosX_ = cursorEvents0->events_.back().position_.x_ * scaleX;
					data.cursorPosY_ = (/*mainWindowWorkAreaSize1->height_ - */cursorEvents0->events_.back().position_.y_) * scaleY; // Инвертируем Y
					driver->StorageBufferWrite(
						dataStorageBuffer1->id_, 0, &data,
						//Write only cursor info.
						offsetof(Render::Outline::Data, Render::Outline::Data::selectedIds_));
				}



				driver->BindPipeline(pipelineId1->id_);

				driver->Bind(pipelineId1->id_, 0,
					{
						cameraTransformResource3->id_,
						dataStorageBufferResource1->id_
					}
				);

				driver->PushConstants(
					pipelineId1->id_,
					RAL::Driver::Shader::Stage::VertexShader,
					sizeof(meshInfo), &meshInfo);

				driver->BindVertexBuffer(driverVertexBuffer4->id_, 0);
				driver->BindIndexBuffer(driverIndexBuffer4->id_, 0);


				driver->DrawIndexed(indices4->indices_.GetIndicesNumber());


			};
		}

	} // namespace Physics

} // namespace OksEngine