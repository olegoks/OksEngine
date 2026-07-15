#pragma once
#include <Engine\auto_OksEngine.SystemMarks.hpp>

namespace OksEngine
{

	namespace SystemMarks
	{
		void CreateConnectionEntity::Update() {

			const ECS2::Entity::Id entityId = CreateEntity();

			CreateComponent<Connection>(entityId);

		};

		void FRAME_HEAD_START::Update(ECS2::Entity::Id entity0id, const Connection* connection0) {};

		void FRAME_HEAD_END::Update(ECS2::Entity::Id entity0id, const Connection* connection0) {};

		void FRAME_BODY_START::Update(ECS2::Entity::Id entity0id, const Connection* connection0) {};

		void FRAME_BODY_END::Update(ECS2::Entity::Id entity0id, const Connection* connection0) {};

		void FRAME_TAIL_START::Update(ECS2::Entity::Id entity0id, const Connection* connection0) {};

		void FRAME_TAIL_END::Update(ECS2::Entity::Id entity0id, const Connection* connection0) {};

	}

} // namespace Engine
