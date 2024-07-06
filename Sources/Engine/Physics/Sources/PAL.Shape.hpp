#pragma once 

namespace PAL {

	class Shape {
	public:

		struct CreateInfo1 {
			float halfExtentX_ = 0.5f;
			float halfExtentY_ = 0.5f;
			float halfExtentZ_ = 0.5f;
		};

		struct CreateInfo2 {
			float radius_ = 0.5f;
			float height_ = 2.f;
		};

		Shape(const CreateInfo1& createInfo) {}
		Shape(const CreateInfo2& createInfo) {}

	private:
	};

}