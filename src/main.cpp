#include "plugin.h"
#include "init.h"
#include "spdlog/sinks/basic_file_sink.h"

#include <ShlObj.h>

namespace
{
	constexpr SKSE::PluginVersionData GetPluginVersion() {
		SKSE::PluginVersionData version;

		version.PluginVersion({Plugin::Version::major, Plugin::Version::minor, Plugin::Version::patch, Plugin::Version::build});
		version.PluginName(Plugin::name);
		version.UsesAddressLibrary(true);
		version.UsesStructsPost629(true);

		return version;
	}

	void InitLog() {
		std::filesystem::path path;
		PWSTR buffer{ nullptr };
		const auto result = SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_DEFAULT, nullptr, std::addressof(buffer));
		std::unique_ptr<wchar_t[], decltype(&CoTaskMemFree)> knownPath(buffer, CoTaskMemFree);
		if (knownPath && result == S_OK) {
			path = knownPath.get();
			path /= "My Games";
			path /= std::filesystem::exists("steam_api64.dll") ? "Skyrim Special Edition" : "Skyrim Special Edition GOG";
			path /= "SKSE";
			path /= Plugin::name;
			path += ".log";
		} else {
			path = Plugin::name;
			path += ".log";
		}

		try {
			spdlog::drop("default");
			spdlog::set_default_logger(spdlog::basic_logger_mt("default", path.string(), true));
			spdlog::set_level(spdlog::level::info);
			spdlog::flush_on(spdlog::level::info);
		} catch (...) {}
	}
}

extern "C" __declspec(dllexport) constinit SKSE::PluginVersionData SKSEPlugin_Version{GetPluginVersion()};

extern "C" __declspec(dllexport) bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse) {
	try {
		InitLog();
		spdlog::info("Loading {} {}.{}.{}...", Plugin::name, Plugin::Version::major, Plugin::Version::minor, Plugin::Version::patch);
		SKSE::Init(a_skse);
		Gotobed::Init();
		spdlog::info("Successfully loaded {}!", Plugin::name);
		return true;
	} catch (const std::exception& e) {
		spdlog::critical("Exception occurred during SKSEPlugin_Load: {}", e.what());
		return false;
	} catch (...) {
		spdlog::critical("Unknown exception occurred during SKSEPlugin_Load");
		return false;
	}
}