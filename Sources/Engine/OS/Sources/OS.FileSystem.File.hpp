#pragma once

#include <OS.Memory.AllocationCallbacks.hpp>

namespace OS {

	class [[nodiscard]] File {
	public:

		File(
			const std::filesystem::path& path,
			const Memory::AllocationCallbacks& allocationCallbacks = Memory::AllocationCallbacks{}) noexcept :
			path_{ path },
			allocationCallbacks_{ allocationCallbacks } { }


		virtual void Open() = 0;
		virtual void Load() = 0;
		virtual void Unload() = 0;
		virtual void Close() = 0;

		virtual ~File() = default;

	protected:
		std::filesystem::path path_;
		Memory::AllocationCallbacks allocationCallbacks_;
	};
}
