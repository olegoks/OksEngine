
#include <PhysX.World.hpp>
#include <OS.Assert.hpp>
#include <PxPhysicsAPI.h>

namespace PhysX {
	
	World::World() {
		static physx::PxDefaultErrorCallback gDefaultErrorCallback;
		static physx::PxDefaultAllocator gDefaultAllocatorCallback;

		physx::PxFoundation* foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback,
			gDefaultErrorCallback);
		OS::Assert(foundation != nullptr);
			
	}
}