#pragma once 

namespace PAL {

	class Shape {
	public:

		struct Material {
			float staticFriction_ = 0.5f;
			float dynamicFriction_ = 0.5f;
			float restitution_ = 0.5f;
		};

		struct CreateInfoBox {
			Material material_;
			float halfExtentX_ = 0.5f;
			float halfExtentY_ = 0.5f;
			float halfExtentZ_ = 0.5f;
		};

		struct CreateInfoCapsule {
			Material material_;
			float radius_ = 0.5f;
			float height_ = 2.f;
		};

		Shape(const CreateInfoBox& createInfo) {}
		Shape(const CreateInfoCapsule& createInfo) {}

	private:
	};

}