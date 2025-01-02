#pragma once

#include <compare>
#include <OS.FileSystem.BinaryFile.hpp>

namespace OS {

	class TextFile : private BinaryFile {
	public:

		TextFile(const std::filesystem::path& path) : BinaryFile{ path } {

		}

		virtual void Create() override {
			BinaryFile::Create();
		}

		void Open() override {
			BinaryFile::Open();
		}

		void Load() override {
			BinaryFile::Load();
		}

		void Unload() override {
			BinaryFile::Unload();
		}

		TextFile& operator<<(const std::string& text) {
			BinaryFile::WriteInfo writeInfo{
				.data_ = text.c_str(),
				.size_ = text.size()
			};
			BinaryFile::operator<<(writeInfo);

			return *this;
		}

		void Close() override {
			BinaryFile::Close();
		}
		
	};

}