#include "plugin.h"
#include "init.h"
#include "spdlog/sinks/basic_file_sink.h"

namespace
{
	constexpr SKSE::PluginVersionData GetPluginVersion() {
		SKSE::PluginVersionData version;

		version.PluginVersion({Plugin::Version::major, Plugin::Version::minor, Plugin::Version::patch, Plugin::Version::build});
		version.PluginName(Plugin::name);
		version.UsesAddressLibrary();
		version.UsesUpdatedStructs();

		return version;
	}

	void InitLog() {
		auto path = SKSE::log::log_directory();
		if (path) {
			*path /= Plugin::name;
			*path += ".log";
			try {
				spdlog::drop("default");
				spdlog::set_default_logger(spdlog::basic_logger_mt("default", path->string(), true));
				spdlog::set_level(spdlog::level::info);
				spdlog::flush_on(spdlog::level::info);
			} catch (...) {}
		}
	}
}

extern "C" __declspec(dllexport) constinit SKSE::PluginVersionData SKSEPlugin_Version{GetPluginVersion()};

extern "C" __declspec(dllexport) bool SKSEPlugin_Load(const SKSE::LoadInterface* a_skse) {
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