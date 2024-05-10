#pragma once

namespace OS {

	class File {
	public:

		File() = default;

		virtual void Open() = 0;
		virtual void Load() = 0;
		virtual void Unload() = 0;
		virtual void Close() = 0;

		virtual ~File() = default;
	};
}