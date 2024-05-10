#include<vector>

#include <ECS.Entity.hpp>
#include <ECS.Component.hpp>

namespace ECS {

	class World;

	class System {
	public:
		virtual void Update(World* world, Entity::Id entityId) const = 0;
	};

}