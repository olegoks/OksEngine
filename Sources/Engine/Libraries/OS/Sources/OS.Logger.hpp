#pragma once

#include <source_location>

#include <string>
#include <loguru/loguru.hpp>

#include <Common.hpp>
#include <Common.Format.hpp>

namespace OS {

	class Logger {
	public:

		struct CreateInfo {
			int argc_ = 0;
			char** argv_ = nullptr;
		};

		class Severity {
		public:
			enum Type {
				Error,
				Warning,
				Info
			};

			Severity(Severity::Type severity) : type_{ severity } { }

			operator std::string () const noexcept {
				return magic_enum::enum_name(type_).data();
			}

			operator std::string_view() const noexcept {
				return magic_enum::enum_name(type_);
			}

			operator Type() const noexcept {
				return GetType();
			}

			[[nodiscard]]
			Type GetType() const noexcept { return type_; }

		private:
			Type type_;
		};

		Logger(CreateInfo& createInfo) {
			loguru::init(createInfo.argc_, createInfo.argv_);
			loguru::add_file("auto_engine.log", loguru::Append, loguru::Verbosity_MAX);
		}

		void Log(Severity severity, const std::string_view path, const Common::Format& format, const std::source_location& location = std::source_location::current()) {
			Common::DiscardUnusedParameter(severity);
			Common::DiscardUnusedParameter(path);
			Common::DiscardUnusedParameter(location);
			using namespace std::string_literals;
			LOG_F(INFO, ("Location: "s + location.file_name() + " " + static_cast<std::string>(format)).c_str());
		}

	private:
		//class Entry {
		//public:

		//	class Column {
		//	public:
		//		Column(Common::Size width, const std::string_view& text) noexcept :
		//			width_{ width } {
		//			textLines_ = SeparateToLines(text);
		//			for (auto& line : textLines_) {
		//				textLinesViews_.emplace_back(Common::StringView{ line }).SetView(0, width);
		//			}
		//		}

		//		static std::vector<std::string> SeparateToLines(const std::string_view& text) noexcept {
		//			std::vector<std::string> lines;
		//			const Common::Index textLastSymbolIndex = text.size() - 1;
		//			const Common::Size textSize = text.size();
		//			for (
		//				Common::Index lineStart = 0,
		//				symbolIndex = 0; 
		//				symbolIndex < textSize;
		//				symbolIndex++) {
		//				const char symbol = text[symbolIndex];
		//				if (symbol == '\n') {
		//					const std::string line{ text.substr(lineStart, symbolIndex - lineStart) };
		//					lines.push_back(line);
		//					lineStart = symbolIndex + 1;
		//				}
		//				else if (symbolIndex == textLastSymbolIndex) {
		//					const std::string line{ text.substr(lineStart, symbolIndex - lineStart + 1).data() };
		//					lines.push_back(line);
		//				}
		//			}
		//			return lines;
		//		}
		//		
		//		void NextColumnLine() noexcept {
		//			Common::StringView& lineView = textLinesViews_[currentTextLine_];
		//			lineView.OffsetViewRight(width_);
		//			if (lineView.GetViewLength() == 0) {
		//				const bool isLastTextLine = (currentTextLine_ == textLinesViews_.size() - 1);
		//				if (!isLastTextLine) {
		//					++currentTextLine_;
		//				}
		//			}
		//		}

		//		[[nodiscard]]
		//		Common::Size GetWidth() const noexcept {
		//			return width_;
		//		}

		//		[[nodiscard]]
		//		std::optional<std::string> GetColumnLine() const noexcept {
		//			const Common::StringView& textLineView = textLinesViews_[currentTextLine_];
		//			if (textLineView.GetViewLength() != 0) {
		//				return textLineView.GetView();
		//			}
		//			return {};
		//		}

		//	private:
		//		Common::Index currentTextLine_ = 0;
		//		Common::Size width_ = 20;
		//		std::vector<std::string> textLines_;
		//		std::vector<Common::StringView> textLinesViews_;
		//	};

		//	Entry(Severity severity, const std::string_view path, const Common::Format& format, const std::source_location& location) {

		//		OS::AssertMessage(!path.empty(), "Attempt to use empty log path.");

		//		columns_.emplace_back(Column{ 10, severity });
		//		columns_.emplace_back(Column{ 30, path.data() });
		//		columns_.emplace_back(Column{ 60, format });
		//		columns_.emplace_back(Column{ 20, location.function_name() });
		//		std::filesystem::path pathToSource{ location.file_name() };
		//		columns_.emplace_back(Column{ 30, pathToSource.filename().string() });
		//		columns_.emplace_back(Column{  5, std::to_string(location.line()) });

		//		bool isLastLine = true;
		//		do {
		//			isLastLine = true;
		//			std::stringstream line;
		//			for (Column& column : columns_) {
		//				std::optional<std::string> maybeColumnLine = column.GetColumnLine();
		//				line << std::setw(column.GetWidth());
		//				line << std::left;
		//				if (maybeColumnLine.has_value()) {
		//					line << maybeColumnLine.value();
		//					column.NextColumnLine();
		//					if (column.GetColumnLine().has_value()) {
		//						isLastLine = false;
		//					}
		//				}
		//				line << "";
		//				line << delimiter_;
		//			}
		//			spirv_ << line.str();
		//			spirv_ << std::endl;
		//		} while (!isLastLine);

		//	}

		//	friend std::ostream& operator<<(std::ostream& os, Entry& entry) {
		//		return os << entry.spirv_.str();
		//	}

		//private:
		//	std::vector<Common::Size> columnsWidth_{ 10, 30, 90, 20, 30, 5 };
		//	char delimiter_ = '|';
		//	std::stringstream spirv_;
		//	std::vector<Column> columns_;

		//};
	};

	void InitializeLogger(int argc, char** argv);

	void Log(
		Logger::Severity severity, 
		const std::string_view path,
		Common::Format&& format,
		const std::source_location& location = std::source_location::current());

	void LogError(
		const std::string_view path, 
		Common::Format&& format,
		std::source_location location = std::source_location::current());

	void LogWarning(
		const std::string_view path,
		Common::Format&& format,
		std::source_location location = std::source_location::current());

	void LogInfo(
		const std::string_view path,
		Common::Format&& format,
		std::source_location location = std::source_location::current());

}
