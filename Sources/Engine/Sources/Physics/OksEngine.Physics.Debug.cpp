#pragma once
#include <Physics\auto_OksEngine.Physics.Debug.hpp>

namespace OksEngine
{

	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
	};

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
			void AddToRenderDynamicRigidBodyShape::Update(ECS2::Entity::Id entity0id, const Render::Debug::Renderer* render__Debug__Renderer0,
				Render::Debug::VertexBuffer* render__Debug__VertexBuffer0, ECS2::Entity::Id entity1id,
				const WorldPosition3D* worldPosition3D1, const WorldRotation3D* worldRotation3D1,
				const WorldScale3D* worldScale3D1, const Physics::DynamicRigidBody* dynamicRigidBody1,
				const Physics::DynamicRigidBodyId* dynamicRigidBodyId1, const Physics::PhysicsShape* physicsShape1) {

				const glm::vec3 translate{ worldPosition3D1->x_, worldPosition3D1->y_, worldPosition3D1->z_ };
				const glm::quat rotation{ worldRotation3D1->w_, worldRotation3D1->x_, worldRotation3D1->y_, worldRotation3D1->z_ };


				const ECS2::ComponentsFilter filter = GetComponentsFilter(entity1id);

				Geom::VertexCloud<Geom::Vertex3fnc> vertices;

				if (filter.IsSet<Physics::ShapeGeometryData>()) {
					const auto* shapeData = GetComponent<Physics::ShapeGeometryData>(entity1id);

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

						const glm::vec3 edge0 = v1 - v0;
						const glm::vec3 edge1 = v2 - v0;

						const glm::vec3 faceNormal = glm::cross(edge0, edge1);

						const glm::vec3 scale{ worldScale3D1->x_, worldScale3D1->y_, worldScale3D1->z_ };

						const glm::vec3 transformedV0 = translate + rotation * (scale * v0);
						const glm::vec3 transformedV1 = translate + rotation * (scale * v1);
						const glm::vec3 transformedV2 = translate + rotation * (scale * v2);

						vertices.Add(
							{
								Geom::Vector3f{ transformedV0.x,transformedV0.y, transformedV0.z },
								Geom::Normal3f{ faceNormal.x, faceNormal.y, faceNormal.z },
								Geom::Color3f{ 0.0, 1.0, 0.0 }
							});

						vertices.Add(
							{
								Geom::Vector3f{ transformedV1.x,transformedV1.y, transformedV1.z },
								Geom::Normal3f{ faceNormal.x, faceNormal.y, faceNormal.z },
								Geom::Color3f{ 0.0, 1.0, 0.0 }
							});

						vertices.Add(
							{
								Geom::Vector3f{ transformedV2.x,transformedV2.y, transformedV2.z },
								Geom::Normal3f{ faceNormal.x, faceNormal.y, faceNormal.z },
								Geom::Color3f{ 0.0, 1.0, 0.0 }
							});
						
					}

					render__Debug__VertexBuffer0->vertices_.Add(vertices.GetData(), vertices.GetVerticesNumber());

				}
				else if (filter.IsSet<Physics::ShapeGeometryCapsule>()) {
					const auto* shapeData = GetComponent<Physics::ShapeGeometryCapsule>(entity1id);
					auto capsuleVertices = generateCapsuleTriangles(shapeData->radius_, shapeData->height_, shapeData->segments_, shapeData->rings_);

					for (Common::Index i = 0; i < capsuleVertices.size(); i += 3) {

						const Vertex& shapeVertex0 = capsuleVertices[i];
						const Vertex& shapeVertex1 = capsuleVertices[i + 1];
						const Vertex& shapeVertex2 = capsuleVertices[i + 2];

						const glm::vec3 v0 = shapeVertex0.position;
						const glm::vec3 v1 = shapeVertex1.position;
						const glm::vec3 v2 = shapeVertex2.position;

						const glm::vec3 edge0 = v1 - v0;
						const glm::vec3 edge1 = v2 - v0;

						const glm::vec3 faceNormal = glm::cross(edge0, edge1);

						const glm::vec3 scale{ worldScale3D1->x_, worldScale3D1->y_, worldScale3D1->z_ };

						const glm::vec3 transformedV0 = translate + rotation * (scale * v0);
						const glm::vec3 transformedV1 = translate + rotation * (scale * v1);
						const glm::vec3 transformedV2 = translate + rotation * (scale * v2);

						vertices.Add(
							{
								Geom::Vector3f{ transformedV0.x,transformedV0.y, transformedV0.z },
								Geom::Normal3f{ faceNormal.x, faceNormal.y, faceNormal.z },
								Geom::Color3f{ 0.0, 1.0, 0.0 }
							});

						vertices.Add(
							{
								Geom::Vector3f{ transformedV1.x,transformedV1.y, transformedV1.z },
								Geom::Normal3f{ faceNormal.x, faceNormal.y, faceNormal.z },
								Geom::Color3f{ 0.0, 1.0, 0.0 }
							});

						vertices.Add(
							{
								Geom::Vector3f{ transformedV2.x,transformedV2.y, transformedV2.z },
								Geom::Normal3f{ faceNormal.x, faceNormal.y, faceNormal.z },
								Geom::Color3f{ 0.0, 1.0, 0.0 }
							});

					}

					render__Debug__VertexBuffer0->vertices_.Add(vertices.GetData(), vertices.GetVerticesNumber());

				}

			};

		}

	} // namespace Physics

	namespace Physics
	{
		namespace Debug
		{
			void AddToRenderStaticRigidBodyShape::Update(ECS2::Entity::Id entity0id, const Render::Debug::Renderer* render__Debug__Renderer0,
				Render::Debug::VertexBuffer* render__Debug__VertexBuffer0, ECS2::Entity::Id entity1id,
				const WorldPosition3D* worldPosition3D1, const WorldRotation3D* worldRotation3D1,
				const WorldScale3D* worldScale3D1, const Physics::StaticRigidBody* staticRigidBody1,
				const Physics::StaticRigidBodyId* staticRigidBodyId1, const Physics::PhysicsShape* physicsShape1) {


				const glm::vec3 translate{ worldPosition3D1->x_, worldPosition3D1->y_, worldPosition3D1->z_ };
				const glm::quat rotation{ worldRotation3D1->w_, worldRotation3D1->x_, worldRotation3D1->y_, worldRotation3D1->z_ };


				const ECS2::ComponentsFilter filter = GetComponentsFilter(entity1id);

				Geom::VertexCloud<Geom::Vertex3fnc> vertices;

				if (filter.IsSet<Physics::ShapeGeometryData>()) {
					const auto* shapeData = GetComponent<Physics::ShapeGeometryData>(entity1id);

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

						const glm::vec3 edge0 = v1 - v0;
						const glm::vec3 edge1 = v2 - v0;

						const glm::vec3 faceNormal = glm::cross(edge0, edge1);

						const glm::vec3 scale{ worldScale3D1->x_, worldScale3D1->y_, worldScale3D1->z_ };

						const glm::vec3 transformedV0 = translate + rotation * (scale * v0);
						const glm::vec3 transformedV1 = translate + rotation * (scale * v1);
						const glm::vec3 transformedV2 = translate + rotation * (scale * v2);

						vertices.Add(
							{
								Geom::Vector3f{ transformedV0.x,transformedV0.y, transformedV0.z },
								Geom::Normal3f{ faceNormal.x, faceNormal.y, faceNormal.z },
								Geom::Color3f{ 0.0, 1.0, 0.0 }
							});

						vertices.Add(
							{
								Geom::Vector3f{ transformedV1.x,transformedV1.y, transformedV1.z },
								Geom::Normal3f{ faceNormal.x, faceNormal.y, faceNormal.z },
								Geom::Color3f{ 0.0, 1.0, 0.0 }
							});

						vertices.Add(
							{
								Geom::Vector3f{ transformedV2.x,transformedV2.y, transformedV2.z },
								Geom::Normal3f{ faceNormal.x, faceNormal.y, faceNormal.z },
								Geom::Color3f{ 0.0, 1.0, 0.0 }
							});

					}

					render__Debug__VertexBuffer0->vertices_.Add(vertices.GetData(), vertices.GetVerticesNumber());

				}


			};

		}

	} // namespace Physics

} // namespace OksEngine