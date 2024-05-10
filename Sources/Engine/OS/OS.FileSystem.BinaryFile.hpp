#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <memory>
#include <filesystem>

#include <OS.FileSystem.File.hpp>
#include <Common.Types.hpp>
#include <OS.Logger.hpp>
#include <OS.Assert.hpp>
#include <OS.Exception.hpp>

export namespace OS {

	class BinaryFile : public File {
	public:

		BinaryFile() = default;
		BinaryFile(const char* pathStr)  : path_{pathStr} {}
		BinaryFile(const std::filesystem::path& path) : data_{ nullptr }, path_{ path }, fstream_{} { }

		void Open() override {
			if (!IsExist()) {
				throw Exception{ "Attempt to open file that doesn't exist." };
			}
			const std::filesystem::path fullPath = std::filesystem::absolute(GetPath());
			fstream_.open(fullPath.c_str(), std::ios::ate | std::ios::binary | std::ios::in | std::ios::out);
			if (!IsOpened()) {
				const std::error_condition econd = std::system_category().default_error_condition(errno);
				throw Exception{ { "Error while openning file. %s.", econd.message().c_str() }};
			}

			OS::LogInfo("/OS/File/", { "File %s was opened successfuly.", GetPath().filename().string().c_str() });
		}

		void Load() override {
			OS::AssertMessage(IsOpened(), "Attempt to load file that wasn't opened.");
			OS::AssertMessage(!IsLoaded(), "Attempt to load file that was loaded yet.");

			size_t size = (size_t)fstream_.tellg();
			fstream_.seekg(0);

			dataArray_.resize(size);
			data_ = std::shared_ptr<Common::Byte>(dataArray_.data());
			fstream_.read(dataArray_.data(), size);
			
			
			if (fstream_.bad()) {

				const std::error_condition econd = std::system_category().default_error_condition(errno);
				throw Exception{ { "Error while reading file %s . %s", GetName().c_str(), econd.message().c_str()}};
			}
			OS::LogInfo("/OS/File/", { "File %s was loaded successfuly.", GetPath().filename().string().c_str() });
		}

		void Unload() override {
			OS::AssertMessage(IsOpened(),"Attempt to unload file that wasn't opened." );
			OS::AssertMessage(IsLoaded(), "Attempt to unload file that wasn't loaded.");
			OS::AssertMessage(!AreThereLinksToData(), "Impossible to undload file if there are links to data.");


		}

		void Close() override {

		}

		bool IsLoaded() const { return (data_ != nullptr); }
		const std::shared_ptr<Common::Byte> GetData() const { return data_; }

		std::string GetName() const { return GetPath().filename().string(); }
		bool IsOpened() const { return fstream_.is_open(); }
		bool IsExist() const { return true;/* std::filesystem::exists(GetPath());*/ }
		Common::Size GetSize() const {
			OS::AssertMessage(IsExist(), "Attempt to get files size that doesn't exist.");
			return std::filesystem::file_size(path_);
		}
		const std::filesystem::path& GetPath() const { return path_; }


		bool AreThereLinksToData() const {
			return (data_.use_count() > 1);
		}
	private:
		std::vector<Common::Byte> dataArray_;
		std::shared_ptr<Common::Byte> data_ = nullptr;
		const std::filesystem::path path_;
		std::fstream fstream_;
	};
}