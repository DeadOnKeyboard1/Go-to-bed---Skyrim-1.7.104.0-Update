#include "Settings.h"
#include <iomanip>
#include <fstream>

namespace Gotobed
{
	namespace detail
	{
		std::filesystem::path GetSettingsPath() {
			std::vector<wchar_t> buf(4096);
			auto size = GetModuleFileNameW(nullptr, &buf[0], static_cast<DWORD>(buf.size()));

			if (size == 0 || size == buf.size()) {
				spdlog::error("error getting settings path: {}", GetLastError());
				return "";
			}

			auto path = std::filesystem::path(&buf[0]).replace_filename(L"Data\\SKSE\\Plugins\\gotobed\\settings.json");
			if (!std::filesystem::exists(path)) {
				auto altPath = std::filesystem::path(&buf[0]).replace_filename(L"Data\\SKSE\\Plugins\\gotobed.json");
				if (std::filesystem::exists(altPath)) {
					return altPath;
				}
			}
			return path;
		}
	}

	Settings& Settings::Get() {
		static Settings settings;
		return settings;
	}

	void Settings::Read() {
		try {
			auto path = detail::GetSettingsPath();
			if (!path.empty() && std::filesystem::exists(path)) {
				std::ifstream f(path);
				*this = json::parse(f).get<Settings>();
			}
		} catch (const json::exception& e) {
			spdlog::error("error reading settings: {}", e.what());
		}
	}

	void Settings::Write() {
		try {
			auto path = detail::GetSettingsPath();
			if (!path.empty()) {
				std::error_code ec;
				std::filesystem::create_directories(path.parent_path(), ec);
				std::ofstream f(path);
				f << std::setw(4) << json(*this);
			}
		} catch (const json::exception& e) {
			spdlog::error("error writing settings: {}", e.what());
		}
	}
}