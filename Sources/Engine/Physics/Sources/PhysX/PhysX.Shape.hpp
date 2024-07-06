#pragma once 

#include <PAL.Shape.hpp>

namespace PhysX {


	class Shape : public PAL::Shape {
	public:

		struct CreateInfo1 {
			PAL::Shape::CreateInfo1 palCreateInfo_;
		};

		struct CreateInfo2 {
			PAL::Shape::CreateInfo2 palCreateInfo_;
		};

		Shape(const CreateInfo1& createInfo);
		Shape(const CreateInfo2& createInfo);

	private:
	};
}