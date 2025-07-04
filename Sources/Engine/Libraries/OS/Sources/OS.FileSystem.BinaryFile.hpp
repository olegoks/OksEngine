#pragma once

#include <string>
#include <fstream>
#include <filesystem>

#include <Datastructures.Vector.hpp>
#include <OS.FileSystem.File.hpp>
#include <Common.Types.hpp>
#include <OS.Logger.hpp>
#include <OS.Assert.hpp>
#include <OS.Exception.hpp>

namespace OS {

	class [[nodiscard]] BinaryFile : public File {
	public:

		BinaryFile() = default;
		BinaryFile(
			const std::filesystem::path& path,
			Memory::AllocationCallbacks allocationCallbacks = Memory::AllocationCallbacks{}) noexcept :
				File{ path, allocationCallbacks }, fstream_{ nullptr } { }

		virtual void Create() override {
			OS::AssertMessage(fstream_ == nullptr, "");

			fstream_ = std::make_unique<std::fstream>(path_, std::ios::out | std::ios::in | std::ios::trunc);

			OS::AssertMessage(IsOpened(), "");
		}

		void Open() override {
			if (!IsExist()) {
				//throw Exception{ "Attempt to open file that doesn't exist." };
			}
			OS::AssertMessage(fstream_ == nullptr, { "File {} was loaded early.", path_.string() });
			const std::filesystem::path fullPath = std::filesystem::absolute(GetPath());
			fstream_ = std::make_unique<std::fstream>();
			fstream_->open(fullPath.c_str(), std::ios::ate | std::ios::binary | std::ios::in | std::ios::out);
				const std::error_condition errorCondition = std::system_category().default_error_condition(errno);
#pragma region Assert
				OS::AssertMessage(IsOpened(), { "Error while openning file. %s with path %s.", errorCondition.message(), fullPath.string() });
#pragma endregion

			//OS::LogInfo("/OS/File/", { "File %s was opened successfuly.", GetPath().filename().string().c_str() });
		}



		void Load() override {
			OS::AssertMessage(IsOpened(), "Attempt to load file that wasn't opened.");
			OS::AssertMessage(!IsLoaded(), "Attempt to load file that was loaded yet.");

			size_t size = std::filesystem::file_size(GetPath());//(size_t)fstream_.tellg();
			fstream_->seekg(0);

			data_.Resize(size);
			fstream_->read(data_.GetData(), size);
			
			if (fstream_->bad()) {

				const std::error_condition errorCondition = std::system_category().default_error_condition(errno);
				throw Exception{ { 
						"Error while reading file %s . %s",
						GetName(),
						errorCondition.message()
					}};
			}
			//OS::LogInfo("/OS/File/", { "File %s was loaded successfuly.", GetPath().filename().string().c_str() });
		}

		void Unload() override {
			OS::AssertMessage(IsOpened(), "Attempt to unload file that wasn't opened.");
			OS::AssertMessage(IsLoaded(), "Attempt to unload file that wasn't loaded.");
			data_.Clear();
		}

		void Close() override {
			OS::AssertMessage(IsOpened(), "Attempt to unload file that wasn't opened.");
			fstream_->close();
		}
		struct WriteInfo {
			const Common::Byte* data_ = nullptr;
			Common::Size size_ = 0;
		};
		BinaryFile& operator<<(const WriteInfo& writeInfo) {
			fstream_->write(writeInfo.data_, writeInfo.size_);
			return *this;
		}

		[[nodiscard]]
		bool IsLoaded() const noexcept override { return (data_.GetSize() != 0); }
		[[nodiscard]]
		const Common::Byte* GetData() const { return data_.GetData(); }
		[[nodiscard]]
		std::string GetName() const { return GetPath().filename().string(); }
		[[nodiscard]]
		bool IsOpened() const { return fstream_->is_open(); }
		[[nodiscard]]
		bool IsExist() const { return std::filesystem::exists(GetPath()); }
		[[nodiscard]]
		Common::Size GetSize() const {
			OS::AssertMessage(IsExist(), "Attempt to get files size that doesn't exist.");
			return std::filesystem::file_size(path_);
		}
		[[nodiscard]]
		const std::filesystem::path& GetPath() const { return path_; }

	private:
		DS::Vector<Common::Byte> data_;
		std::unique_ptr<std::fstream> fstream_ = nullptr;
	};
}