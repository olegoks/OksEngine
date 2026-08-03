#include <PacketManager/auto_PacketManager.hpp>

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
			curl_easy_setopt(curl, CURLOPT_USERAGENT, "ecs-pm/1.0"); // GitHub требует User-Agent
			curl_easy_perform(curl);
			curl_easy_cleanup(curl);
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
	void LoadPackets::Update(
		ECS2::Entity::Id entity0Id, const CommandLineParameters* oKSENGINE__COMMANDLINEPARAMETERS0,
		const ExecutablePath* oKSENGINE__EXECUTABLEPATH0,
		const PacketManager::ECSProjectFilePath* oKSENGINE__PACKETMANAGER__ECSPROJECTFILEPATH0) {

		const auto content = Resource::LoadFileAndGetContent(oKSENGINE__PACKETMANAGER__ECSPROJECTFILEPATH0->path_);
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
				ECS2::Entity::Id requiredEntityId = CreateEntity<PACKETMANAGER__PACKAGE__PACKAGE>();
				//CreateComponent<PacketManager::Package::Tag>()

			}

		}

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

} // namespace PacketManager
