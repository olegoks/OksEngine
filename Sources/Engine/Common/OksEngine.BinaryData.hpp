#pragma once

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	class BinaryData : public ECSComponent<BinaryData> {
	public:

		BinaryData() : ECSComponent{ nullptr } {

		}
		BinaryData(const std::vector<Common::Byte>& data) 
			: ECSComponent{ nullptr },
			data_{ data } {

		}
		std::vector<Common::Byte> data_;
	};


	template<>
	inline void Edit<BinaryData>(BinaryData* binaryData) {

	}

}