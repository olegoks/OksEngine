#pragma once
#include <PacketManager/auto_PacketManager.Version.hpp>
#include <regex>
#include <optional>
#include <string>
#include <cstdio>
#include <cinttypes>

namespace PacketManager {
    namespace Package {

        // Парсинг строки "v1.2.3" или "1.2.3" → Version
        inline Version ParseVersionString(const std::string& str) {
            if (str == "latest") {
                return { Common::Limits<Common::UInt64>::Max(),
                         Common::Limits<Common::UInt64>::Max(),
                         Common::Limits<Common::UInt64>::Max() };
            }
            std::regex pattern(R"(^v?(\d+)\.(\d+)\.(\d+)$)");
            std::smatch match;
            if (std::regex_match(str, match, pattern)) {
                return { std::stoul(match[1].str()),
                         std::stoul(match[2].str()),
                         std::stoul(match[3].str()) };
            }
            // fallback через sscanf, если regex не подошёл (на всякий случай)
            Version v;
            std::string s = (str.size() > 0 && str[0] == 'v') ? str.substr(1) : str;
            sscanf_s(s.c_str(), "%" SCNu64 ".%" SCNu64 ".%" SCNu64, &v.major_, &v.minor_, &v.patch_);
            return v;
        }

        // Version → "v1.2.3"
        inline std::string VersionToString(const Version& version) {
            return "v" + std::to_string(version.major_) + "." +
                std::to_string(version.minor_) + "." + std::to_string(version.patch_);
        }

        // Операторы сравнения
        inline bool operator<(const Version& a, const Version& b) {
            if (a.major_ != b.major_) return a.major_ < b.major_;
            if (a.minor_ != b.minor_) return a.minor_ < b.minor_;
            return a.patch_ < b.patch_;
        }
        inline bool operator==(const Version& a, const Version& b) {
            return a.major_ == b.major_ && a.minor_ == b.minor_ && a.patch_ == b.patch_;
        }
        inline bool operator!=(const Version& a, const Version& b) { return !(a == b); }
        inline bool operator>(const Version& a, const Version& b) { return b < a; }
        inline bool operator<=(const Version& a, const Version& b) { return !(a > b); }
        inline bool operator>=(const Version& a, const Version& b) { return !(a < b); }

    } // namespace Package

    // Диапазон версий для зависимостей
    struct VersionRange {
        Package::Version min;
        Package::Version max;
        bool min_inclusive = true;
        bool max_inclusive = false; // обычно верхняя граница исключительна

        bool Satisfies(const Package::Version& v) const {
            if (min_inclusive) {
                if (v < min) return false;
            }
            else {
                if (v <= min) return false;
            }
            if (max_inclusive) {
                if (v > max) return false;
            }
            else {
                if (v >= max) return false;
            }
            return true;
        }

        static std::optional<VersionRange> Parse(const std::string& str) {
            if (str == "*" || str == "latest" || str.empty()) {
                VersionRange r;
                r.min = { 0, 0, 0 };
                r.max = { UINT64_MAX, UINT64_MAX, UINT64_MAX };
                r.min_inclusive = true;
                r.max_inclusive = true;
                return r;
            }
            if (str[0] == '^') {
                Package::Version ver = Package::ParseVersionString(str.substr(1));
                VersionRange r;
                r.min = ver;
                r.min_inclusive = true;
                r.max = { ver.major_ + 1, 0, 0 };
                r.max_inclusive = false;
                return r;
            }
            if (str[0] == '~') {
                Package::Version ver = Package::ParseVersionString(str.substr(1));
                VersionRange r;
                r.min = ver;
                r.min_inclusive = true;

                r.max = Package::Version{ ver.major_, (Common::UInt64)(ver.minor_ + 1ull), (Common::UInt64)0ull };
                r.max_inclusive = false;
                return r;
            }
            // Точная версия
            Package::Version ver = Package::ParseVersionString(str);
            VersionRange r;
            r.min = ver;
            r.max = ver;
            r.min_inclusive = true;
            r.max_inclusive = true;
            return r;
        }
    };

} // namespace PacketManager