#pragma once

#include <compare>
#include <OS.FileSystem.BinaryFile.hpp>

namespace OS {

	class TextFile : private BinaryFile {
	public:

		void Open() override {
			BinaryFile::Open();
		}

		void Load() override {
			BinaryFile::Load();
		}

		void Unload() override {
			BinaryFile::Unload();
		}

		void Close() override {
			BinaryFile::Close();
		}
		
	};

}