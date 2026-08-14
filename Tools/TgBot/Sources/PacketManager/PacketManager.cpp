#include <PacketManager/auto_PacketManager.hpp>
#include <PacketManager/PacketManager.Version.Utils.hpp>

#include <Resources/OksEngine.ResourceSystem.Utils.hpp>

#include <Lua.Context.hpp>

#include <lua.h>

#include <curl/curl.h>
#include <zip.h>

#include <cstdlib>  // для getenv

#include <nlohmann/json.hpp>
using json = nlohmann::json;
namespace fs = std::filesystem;

// =============== Вспомогательные функции ===============
AI_GENERATED
namespace {

	static size_t write_to_string(void* ptr, size_t size, size_t nmemb, std::string* data) {
		data->append((char*)ptr, size * nmemb);
		return size * nmemb;
	}


	std::string http_get_string(const std::string& url) {
		CURL* curl = curl_easy_init();
		std::string response;
		if (curl) {
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_string);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_setopt(curl, CURLOPT_USERAGENT, "ecs-pm/1.0");

			CURLcode res = curl_easy_perform(curl);

			long http_code = 0;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
			curl_easy_cleanup(curl);

			if (res != CURLE_OK || http_code != 200) {
				std::cerr << "HTTP request failed: " << url
					<< " (status: " << http_code << ")\n";
				return "";
			}
		}
		return response;
	}

	std::string get_release_asset_url(const std::string& owner,
		const std::string& repo,
		const std::string& version) {
		// Запрос к API: https://api.github.com/repos/owner/repo/releases/tags/v1.1.2
		std::string api_url = "https://api.github.com/repos/" + owner + "/" + repo
			+ "/releases/tags/v" + version;

		std::string response = http_get_string(api_url);  // нужно реализовать http_get_string
		if (response.empty()) {
			std::cerr << "Empty response from GitHub API\n";
			return "";
		}

		auto release = json::parse(response);
		if (release.contains("assets") && !release["assets"].empty()) {
			// Берём первый zip-ассет (или можно отфильтровать по имени)
			for (const auto& asset : release["assets"]) {
				std::string name = asset["name"];
				if (name.size() >= 4 && name.substr(name.size() - 4) == ".zip") {
					return asset["browser_download_url"];
				}
			}
		}
		return "";
	}

	// Запись данных, полученных curl, в файл
	size_t write_file(void* ptr, size_t size, size_t nmemb, void* stream) {
		std::ofstream* out = static_cast<std::ofstream*>(stream);
		out->write(static_cast<char*>(ptr), size * nmemb);
		return size * nmemb;
	}

	// Скачать файл по URL в указанный путь
	bool DownloadFile(const std::string& url, const std::string& output_path) {
		CURL* curl = curl_easy_init();
		if (!curl) {
			std::cerr << "Failed to init curl\n";
			return false;
		}

		std::ofstream file(output_path, std::ios::binary);
		if (!file.is_open()) {
			std::cerr << "Cannot open output file: " << output_path << '\n';
			curl_easy_cleanup(curl);
			return false;
		}

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_file);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

		CURLcode res = curl_easy_perform(curl);

		long http_code = 0;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
		curl_easy_cleanup(curl);
		file.close();

		if (res != CURLE_OK) {
			std::cerr << "Download failed: " << curl_easy_strerror(res) << '\n';
			std::filesystem::remove(output_path);
			return false;
		}

		if (http_code != 200) {
			std::cerr << "Download failed with HTTP " << http_code << '\n';
			std::filesystem::remove(output_path);
			return false;
		}

		return true;
	}

	// Распаковать zip-архив в указанную директорию
	bool ExtractZip(const std::string& zip_path, const std::string& dest_dir) {
		int err;
		zip_t* archive = zip_open(zip_path.c_str(), ZIP_RDONLY, &err);
		if (!archive) {
			std::cerr << "Cannot open zip archive: " << zip_path << '\n';
			return false;
		}

		// Создаём целевую директорию, если её нет
		fs::create_directories(dest_dir);

		int num_entries = zip_get_num_entries(archive, 0);
		for (int i = 0; i < num_entries; ++i) {
			const char* name = zip_get_name(archive, i, 0);
			if (!name) continue;

			fs::path out_path = fs::path(dest_dir) / name;

			// Если это директория (имя заканчивается на '/'), создаём её
			if (name[strlen(name) - 1] == '/') {
				fs::create_directories(out_path);
				continue;
			}

			// Создаём родительскую директорию файла
			fs::create_directories(out_path.parent_path());

			// Открываем файл внутри архива
			zip_file_t* zfile = zip_fopen_index(archive, i, 0);
			if (!zfile) {
				std::cerr << "Failed to open file in archive: " << name << '\n';
				continue;
			}

			std::ofstream out(out_path, std::ios::binary);
			if (!out.is_open()) {
				std::cerr << "Cannot create file: " << out_path << '\n';
				zip_fclose(zfile);
				continue;
			}

			char buffer[4096];
			zip_int64_t bytes_read;
			while ((bytes_read = zip_fread(zfile, buffer, sizeof(buffer))) > 0) {
				out.write(buffer, bytes_read);
			}

			out.close();
			zip_fclose(zfile);
		}

		zip_close(archive);
		return true;
	}

	fs::path GetCacheDir() {
		// Пробуем XDG_CACHE_HOME (Linux/macOS)
		const char* xdg_cache = getenv("XDG_CACHE_HOME");
		if (xdg_cache) {
			return fs::path(xdg_cache) / "ecs-pm";
		}
		// macOS fallback
#ifdef __APPLE__
		const char* home = getenv("HOME");
		if (home) {
			return fs::path(home) / "Library" / "Caches" / "ecs-pm";
		}
#else
	// Windows: LOCALAPPDATA
		const char* local_app_data = getenv("LOCALAPPDATA");
		if (local_app_data) {
			return fs::path(local_app_data) / "ecs-pm";
		}
		// Если по какой-то причине нет LOCALAPPDATA, пробуем USERPROFILE
		const char* user_profile = getenv("USERPROFILE");
		if (user_profile) {
			return fs::path(user_profile) / ".ecs-cache";
		}
#endif
		// Последний fallback — просто текущая директория/.ecs-cache
		return fs::current_path() / ".ecs-cache";
	}

	// =============== Основная команда install ===============
	int InstallPackage(const std::string& package_spec, const fs::path& packageInstallPath) {
		// Разбор строки owner/repo@version
		size_t at_pos = package_spec.find('@');
		if (at_pos == std::string::npos) {
			std::cerr << "Usage: ecs-pm install owner/repo@version\n";
			return 1;
		}
		std::string owner_repo = package_spec.substr(0, at_pos);
		std::string version_str = package_spec.substr(at_pos + 1);

		size_t slash = owner_repo.find('/');
		if (slash == std::string::npos) {
			std::cerr << "Expected format: owner/repo@version\n";
			return 1;
		}
		std::string owner = owner_repo.substr(0, slash);
		std::string repo = owner_repo.substr(slash + 1);

		// Убираем возможную 'v' в версии (если пользователь ввёл v1.2.3)
		std::string version_clean = version_str;
		if (!version_clean.empty() && version_clean[0] == 'v')
			version_clean.erase(0, 1);

		// Формируем URL релиза
		std::string zip_name = repo + "-" + version_clean + ".zip";
		std::string url = get_release_asset_url(owner, repo, version_clean);
		if (url.empty()) {
			std::cerr << "Could not find release asset for " << owner << "/" << repo << " v" << version_clean << "\n";
			return 1;
		}

		// Пути кэша и установки
		fs::path cache_dir = GetCacheDir();
		fs::create_directories(cache_dir);
		fs::path zip_path = cache_dir / zip_name;

		// Папка в проекте, куда распаковываем пакет
		fs::path dest_dir = packageInstallPath / "Packages" / repo;
		// Можно добавить версию в имя папки: ecs_packages/<repo>-<version>
		// Но пока просто имя, чтобы проще подключать в коде.

		std::cout << "Downloading " << url << " ...\n";
		if (!DownloadFile(url, zip_path.string())) {
			std::cerr << "Download failed.\n";
			return 1;
		}

		std::cout << "Extracting to " << dest_dir << " ...\n";
		// Удалим старую версию, если есть
		if (fs::exists(dest_dir))
			fs::remove_all(dest_dir);
		fs::create_directories(dest_dir);

		if (!ExtractZip(zip_path.string(), dest_dir.string())) {
			std::cerr << "Extraction failed.\n";
			return 1;
		}

		// Удаляем временный zip (опционально)
		// fs::remove(zip_path);

		std::cout << "Package " << repo << " installed successfully.\n";
		return 0;
	}
}


namespace PacketManager {
	void GetRequiredPackets::Update(ECS2::Entity::Id entity0Id, const ::CommandLineParameters* __CommandLineParameters0,
		const ::ExecutablePath* __ExecutablePath0,
		const PacketManager::ECSProjectFilePath* packetManager__ECSProjectFilePath0) {


		auto getRequiredPackages = [&]() {

			std::vector<ECS2::Entity::Id> requiredPackages;

			const auto content = Resource::LoadFileAndGetContent(packetManager__ECSProjectFilePath0->path_);
			const std::string script{ content.data(), content.size() };
			{
				lua_State* state_ = luaL_newstate();
				ASSERT_MSG(state_ != nullptr, "Error while creating Lua context.");
				luaL_openlibs(state_);
				if (luaL_dostring(state_, script.c_str())) {
					OS::LogError("lua/", lua_tostring(state_, -1));
					lua_pop(state_, 1);
					ASSERT_FAIL_MSG("Error while loading config script.");
				};
				luabridge::LuaRef ecsTable = luabridge::getGlobal(state_, "ECS");
				ASSERT(!ecsTable.isNil());
				luabridge::LuaRef packagesTable = ecsTable["Packages"];

				ASSERT(packagesTable.isTable());
				for (luabridge::Iterator it(packagesTable); !it.isNil(); ++it) {
					luabridge::LuaRef packageDescTable = it.value();
					ASSERT(packageDescTable.isTable());
					const std::string packageName = packageDescTable["Name"].cast<std::string>().value();
					ECS2::Entity::Id requiredPackageEntityId = CreateEntity<PACKETMANAGER__PACKAGE__PACKAGE>();
					CreateComponent<PacketManager::Package::Tag>(requiredPackageEntityId);
					CreateComponent<PacketManager::Package::Name>(requiredPackageEntityId, packageName);
					requiredPackages.push_back(requiredPackageEntityId);
				}

			}
			return requiredPackages;
			};


		std::vector<ECS2::Entity::Id>  requiredPackages = getRequiredPackages();


		ECS2::Entity::Id packetManagerEntityId = CreateEntity();
		CreateComponent<PacketManager::Tag>(packetManagerEntityId);
		CreateComponent<PacketManager::RequiredPackages>(packetManagerEntityId, requiredPackages);
		std::filesystem::path installPath = std::filesystem::path{ packetManager__ECSProjectFilePath0->path_ }.parent_path();
		CreateComponent<PacketManager::InstallPath>(packetManagerEntityId, installPath.string());
		//CreateComponent<PacketManager::>
		//::Lua::Context context;
		//::Lua::Script script{ std::string{ content.data(), content.size() } };
		//context.LoadScript(script);
		//luabridge::LuaRef ecsTable = luabridge::getGlobal(context.state_, "ECS");
		//ASSERT_FMSG(ecsTable.isTable(), "");
		//luabridge::LuaRef includesTable = ecsTable["Includes"];

		//std::filesystem::path packageInstallPath = [](const std::filesystem::path& ecsProjFilePath) {

		//	return std::filesystem::path{ ecsProjFilePath }.remove_filename();

		//	}(oKSENGINE__PACKETMANAGER__ECSPROJECTFILEPATH0->path_);

		//InstallPackage("olegoks/ECS.Common@1.1.2", packageInstallPath);

	}

	inline
		std::string GetPackageReleasesUrl(const std::string& owner, const std::string& repo) {

		return "https://api.github.com/repos/" + owner + "/ECS." + repo + "/releases";
	}

	inline
		std::string GetPackageManifestUrl(
			const std::string& owner,
			const std::string& repo,
			const Package::Version& version,
			const std::string& filename) {
		return "https://github.com/" + owner + "/ECS." + repo + "/releases/download/" + Package::VersionToString(version) + "/" + filename;
	}

	// Внутри namespace PacketManager:

	using ResolvedMap = std::unordered_map<std::string, Package::Version>;
	using ProcessingSet = std::unordered_set<std::string>;
	using AvailableCache = std::unordered_map<std::string, std::vector<Package::Version>>;
	using ManifestCache = std::unordered_map<std::string, std::unordered_map<std::string, std::string>>;

	std::vector<Package::Version> FetchAvailableVersions(const std::string& owner, const std::string& name) {
		std::string url = GetPackageReleasesUrl(owner, name);
		std::string response = http_get_string(url);
		std::vector<Package::Version> versions;
		if (response.empty()) return versions;
		try {
			auto json = nlohmann::json::parse(response);
			for (const auto& release : json) {
				if (release.value("draft", false) || release.value("prerelease", false)) continue;
				std::string tag = release.value("tag_name", "");
				if (tag.empty()) continue;
				versions.push_back(Package::ParseVersionString(tag));
			}
		}
		catch (...) {}
		std::sort(versions.begin(), versions.end(), std::greater<Package::Version>());
		return versions;
	}

	std::string FetchManifest(const std::string& owner, const std::string& repo, const Package::Version& version) {
		std::string url = GetPackageManifestUrl(owner, repo, version, "ECS." + repo + ".ecs_package");
		return http_get_string(url);
	}

	/**
 * Извлекает зависимости из Lua-манифеста (файл .ecs_package).
 * Ожидает глобальную таблицу "Package" с полем "Dependecies".
 * Возвращает карту: имя_зависимости → строка_диапазона.
 */
	std::unordered_map<std::string, std::string> ParseDependenciesFromManifest(const std::string& luaScript) {
		std::unordered_map<std::string, std::string> deps;

		// 1. Создаём Lua-состояние
		lua_State* L = luaL_newstate();
		{
			if (!L) {
				std::cerr << "[PacketManager] Failed to create Lua state for manifest parsing.\n";
				return deps;
			}
			luaL_openlibs(L);

			// 2. Выполняем скрипт
			if (luaL_dostring(L, luaScript.c_str()) != LUA_OK) {
				std::cerr << "[PacketManager] Lua error: " << lua_tostring(L, -1) << std::endl;
				lua_close(L);
				return deps;
			}

			// 3. Получаем таблицу Package
			luabridge::LuaRef packageTable = luabridge::getGlobal(L, "Package");
			if (!packageTable.isTable()) {
				std::cerr << "[PacketManager] Manifest does not contain 'Package' table.\n";
				lua_close(L);
				return deps;
			}

			// 4. Получаем поле Dependecies (в манифесте опечатка)
			luabridge::LuaRef dependencies = packageTable["Dependecies"];
			if (dependencies.isTable()) {
				for (luabridge::Iterator it(dependencies); !it.isNil(); ++it) {
					auto key = it.key();
					auto value = it.value();

					// Ключ должен быть строкой (имя пакета)
					if (!key.isString()) {
						std::cerr << "[PacketManager] Dependency key is not a string, skipping.\n";
						continue;
					}
					std::string depName = key.tostring();

					// Значение – строка диапазона версий
					if (value.isString()) {
						deps[depName] = value.tostring();
					}
					else {
						std::cerr << "[PacketManager] Dependency value for '" << depName << "' is not a string, skipping.\n";
					}
				}
			}
			else if (!dependencies.isNil()) {
				std::cerr << "[PacketManager] 'Dependecies' is not a table.\n";
			}
		}
		lua_close(L);
		return deps;
	}

	bool ResolveRecursive(const std::string& name,
		const VersionRange& range,
		ResolvedMap& resolved,
		ProcessingSet& processing,
		AvailableCache& availableCache,
		ManifestCache& manifestCache,
		const std::string& owner = "olegoks")
	{
		if (processing.count(name)) return false; // цикл

		auto it = resolved.find(name);
		if (it != resolved.end()) {
			return range.Satisfies(it->second);
		}

		processing.insert(name);

		if (availableCache.find(name) == availableCache.end()) {
			availableCache[name] = FetchAvailableVersions(owner, name);
		}
		const auto& versions = availableCache[name];
		if (versions.empty()) {
			processing.erase(name);
			return false;
		}

		for (const auto& ver : versions) {
			if (!range.Satisfies(ver)) continue;

			std::string manifestKey = name + "@" + std::to_string(ver.major_) + "." + std::to_string(ver.minor_) + "." + std::to_string(ver.patch_);
			if (manifestCache.find(manifestKey) == manifestCache.end()) {
				std::string manifestContent = FetchManifest(owner, name, ver);
				if (manifestContent.empty()) {
					ASSERT_FAIL_FMSG("Can't find manifest file for package {} {}", name, manifestKey);
					continue;
				}
				manifestCache[manifestKey] = ParseDependenciesFromManifest(manifestContent);
			}

			const auto& deps = manifestCache[manifestKey];
			auto snapshot = resolved;
			resolved[name] = ver;
			bool success = true;
			for (const auto& [depName, depRangeStr] : deps) {
				auto depRange = VersionRange::Parse(depRangeStr);
				if (!depRange) { success = false; break; }
				if (!ResolveRecursive(depName, *depRange, resolved, processing, availableCache, manifestCache, owner)) {
					success = false; break;
				}
			}
			if (success) {
				processing.erase(name);
				return true;
			}
			resolved = std::move(snapshot);
		}
		processing.erase(name);
		return false;
	}

	ResolvedMap ResolveAll(const std::vector<std::pair<std::string, std::optional<Package::Version>>>& requirements,
		const std::string& owner = "olegoks") {
		ResolvedMap resolved;
		ProcessingSet processing;
		AvailableCache availableCache;
		ManifestCache manifestCache;
		for (const auto& [pkgName, optVer] : requirements) {
			VersionRange range;
			if (optVer.has_value()) {
				range = VersionRange::Parse(VersionToString(optVer.value())).value(); // точная
			}
			else {
				range = *VersionRange::Parse("latest");
			}
			if (!ResolveRecursive(pkgName, range, resolved, processing, availableCache, manifestCache, owner)) {
				return {}; // ошибка
			}
		}
		return resolved;
	}

	void GetPacketsToDownload::Update(
		ECS2::Entity::Id entity0Id,
		const PacketManager::Tag* packetManager__Tag0,
		const PacketManager::RequiredPackages* packetManager__RequiredPackages0) {

		std::vector<std::pair<std::string, std::optional<PacketManager::Package::Version>>> requirements;
		for (ECS2::Entity::Id requiredPacket : packetManager__RequiredPackages0->required_) {
			const ECS2::ComponentsFilter cf = GetComponentsFilter(requiredPacket);
			std::string name = GetComponent<Package::Name>(requiredPacket)->name_;
			std::optional<PacketManager::Package::Version> ver;
			if (cf.IsSet<Package::Version>()) {
				ver = *GetComponent<Package::Version>(requiredPacket);
			}
			requirements.emplace_back(name, ver);
		}

		auto resolvedMap = ResolveAll(requirements, "olegoks");
		if (resolvedMap.empty()) {
			OS::LogError("PacketManager", "Dependency resolution failed");
			ASSERT_FAIL();
			return;
		}

		std::unordered_map<std::string, std::tuple<Common::UInt64, Common::UInt64, Common::UInt64>> resolvedLocal;

		for (auto& [key, value] : resolvedMap) {
			resolvedLocal[key] = std::tuple{ value.major_, value.minor_, value.patch_ };
		}

		CreateComponent<PacketManager::ResolvedPackages>(entity0Id, std::move(resolvedLocal));


		//std::unordered_map<std::string, std::vector<Package::Version>> packetToAvailableVersions;

		//auto getAvailableVersions = [](const std::string& name) {
		//	std::string response = http_get_string(GetPackageReleasesUrl("olegoks", "ECS." + name));
		//	ASSERT(!response.empty());
		//	std::vector<Package::Version> versions;
		//	auto json = nlohmann::json::parse(response);
		//	// response – это массив релизов
		//	for (const auto& release : json) {
		//		// Пропускаем черновики и пре-релизы, если нужно (опционально)
		//		if (release.value("draft", false) || release.value("prerelease", false)) continue;

		//		std::string tag = release.value("tag_name", "");
		//		if (tag.empty()) continue;
		//		const Package::Version version = PacketManager::Package::ParseVersionString(tag);
		//		versions.push_back(version);
		//		//auto ver = Version::parse(tag);
		//		//if (ver) {
		//		//	versions.push_back(*ver);
		//		//}
		//		//else {
		//		//	std::cerr << "Warning: can't parse version tag: " << tag << "\n";
		//		//}
		//	}

		//	std::sort(versions.begin(), versions.end(),
		//		[](const Package::Version& first, const Package::Version& second) {
		//			// Сначала сравниваем major
		//			if (first.major_ != second.major_) {
		//				return first.major_ > second.major_;  // больше → раньше
		//			}
		//			// Потом minor
		//			if (first.minor_ != second.minor_) {
		//				return first.minor_ > second.minor_;
		//			}
		//			// Потом patch
		//			return first.patch_ > second.patch_;

		//		});

		//	return versions;
		//	};

		//for (ECS2::Entity::Id requiredPacket : packetManager__RequiredPackages0->required_) {
		//	const ECS2::ComponentsFilter cf = GetComponentsFilter(requiredPacket);
		//	std::string name = GetComponent<Package::Name>(requiredPacket)->name_;
		//	if (cf.IsSet<Package::Version>()) {
		//		const auto* version = GetComponent<Package::Version>(requiredPacket);
		//		packetToAvailableVersions[name] = std::vector{ *version };
		//	}
		//	else {
		//		packetToAvailableVersions[name] = getAvailableVersions(name);
		//	}
		//	
		//	
		//	/*std::string manifest_url = "https://github.com/olegoks/ECS." + name +
		//		"/releases/download/" + version_tag + "/" + repo + ".ecs_package";*/

		//}

		//// 1. Собираем корневые требования
		//std::vector<std::pair<std::string, std::optional<Package::Version>>> requirements;
		//for (ECS2::Entity::Id requiredPacket : packetManager__RequiredPackages0->required_) {
		//	const ECS2::ComponentsFilter cf = GetComponentsFilter(requiredPacket);
		//	std::string name = GetComponent<Package::Name>(requiredPacket)->name_;
		//	std::optional<Package::Version> ver;
		//	if (cf.IsSet<Package::Version>()) {
		//		ver = *GetComponent<Package::Version>(requiredPacket);
		//	}
		//	requirements.emplace_back(name, ver);
		//}

		//// 2. Запускаем разрешение
		//auto resolved = ResolveAll(requirements, "olegoks"); // владельца можно вынести в конфиг
		//if (resolved.empty()) {
		//	std::cerr << "Dependency resolution failed" << std::endl;
		//	return;
		//}

		//// 3. Сохраняем результат в компонент
		//CreateComponent<PacketManager::ResolvedPackages>(entity0Id, std::move(resolved));
	}

	// Устанавливает один пакет по owner, repo, версии.
	// installBasePath – папка проекта (где будет создана подпапка Packages/имя_пакета).
	bool InstallSinglePackage(const std::string& owner,
		const std::string& repo,
		const PacketManager::Package::Version& version,
		const std::filesystem::path& installBasePath)
	{
		std::string version_str = PacketManager::Package::VersionToString(version); // "v1.2.3"
		std::string version_clean = version_str.substr(1);                         // "1.2.3"

		// Пытаемся получить URL ассета через API
		std::string url = get_release_asset_url(owner, "ECS." + repo, version_clean);
		if (url.empty()) {
			// Fallback – прямой URL, если ассет назван стандартно: repo-version.zip
			url = "https://github.com/" + owner + "/ECS." + repo + "/releases/download/"
				+ version_str + "/" + repo + "-" + version_clean + ".zip";
		}

		// Путь к кэшу
		std::filesystem::path cache_dir = GetCacheDir();
		std::filesystem::create_directories(cache_dir);
		std::string zip_name = repo + "-" + version_clean + ".zip";
		std::filesystem::path zip_path = cache_dir / zip_name;

		// Папка, куда распакуем пакет
		std::filesystem::path dest_dir = installBasePath / "Packages" / repo;
		
		LOG_INFO("PacketManager", "Downloading {} ...", url);

		if (!DownloadFile(url, zip_path.string())) {
			std::cerr << "Download failed for " << repo << "@" << version_clean << "\n";
			return false;
		}
		LOG_INFO("PacketManager", "Extracting to {} ...", dest_dir.string());
		if (std::filesystem::exists(dest_dir))
			std::filesystem::remove_all(dest_dir);
		std::filesystem::create_directories(dest_dir);

		if (!ExtractZip(zip_path.string(), dest_dir.string())) {
			std::cerr << "Extraction failed for " << repo << "\n";
			return false;
		}

		LOG_INFO("PacketManager", "Package {} installed successfully.", repo);
		return true;
	}

	void DownloadAndInstallPackets::Update(
		ECS2::Entity::Id entity0Id, const PacketManager::Tag* packetManager__Tag0,
		const PacketManager::InstallPath* packetManager__InstallPath0,
		const PacketManager::ResolvedPackages* packetManager__ResolvedPackages0) {

		const std::string owner = "olegoks";  // TODO: вынести в конфигурацию

		for (const auto& [packageName, version] : packetManager__ResolvedPackages0->resolved_) {
			bool ok = InstallSinglePackage(
				owner, 
				packageName,
				Package::Version{ 
					std::get<0>(version), 
					std::get<1>(version),
					std::get<2>(version) 
				},
				packetManager__InstallPath0->path_
			);
			ASSERT_FMSG(ok, "Failed to install package: {}", packageName);
		}

		LOG_INFO("PacketManager", "All packages installed.");


	}

} // namespace PacketManager
