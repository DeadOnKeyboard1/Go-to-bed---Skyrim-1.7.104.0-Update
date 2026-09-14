#include "SettingsPapyrus.h"
#include "Settings.h"

namespace Gotobed::SettingsPapyrus
{
	void Write(RE::StaticFunctionTag*) {
		Settings::Get().Write();
	}

	std::int32_t Keys_GetSleepKey(RE::StaticFunctionTag*) {
		auto& settings = Settings::Get();
		return settings.keys.sleep;
	}

	void Keys_SetSleepKey(RE::StaticFunctionTag*, std::int32_t a_key) {
		auto& settings = Settings::Get();
		settings.keys.sleep = (a_key <= 1 || a_key > 255) ? -1 : a_key;
	}

	std::int32_t Keys_GetSleepModKey(RE::StaticFunctionTag*) {
		auto& settings = Settings::Get();
		return settings.keys.sleepMod;
	}

	void Keys_SetSleepModKey(RE::StaticFunctionTag*, std::int32_t a_key) {
		auto& settings = Settings::Get();
		settings.keys.sleepMod = (a_key <= 1 || a_key > 255) ? -1 : a_key;
	}

	std::int32_t Keys_GetServeTimeKey(RE::StaticFunctionTag*) {
		auto& settings = Settings::Get();
		return settings.keys.serveTime;
	}

	void Keys_SetServeTimeKey(RE::StaticFunctionTag*, std::int32_t a_key) {
		auto& settings = Settings::Get();
		settings.keys.serveTime = (a_key <= 1 || a_key > 255) ? -1 : a_key;
	}

	std::int32_t Keys_GetServeTimeModKey(RE::StaticFunctionTag*) {
		auto& settings = Settings::Get();
		return settings.keys.serveTimeMod;
	}

	void Keys_SetServeTimeModKey(RE::StaticFunctionTag*, std::int32_t a_key) {
		auto& settings = Settings::Get();
		settings.keys.serveTimeMod = (a_key <= 1 || a_key > 255) ? -1 : a_key;
	}

	void Register() {
		SKSE::GetPapyrusInterface()->Register([](RE::BSScript::IVirtualMachine* a_vm) {
			a_vm->RegisterFunction("Write", "GTB_Settings", Write);

			// Standard function names expected by GTB_Settings.psc and GTB_MCM.psc
			a_vm->RegisterFunction("GetSleepKey", "GTB_Settings", Keys_GetSleepKey);
			a_vm->RegisterFunction("SetSleepKey", "GTB_Settings", Keys_SetSleepKey);
			a_vm->RegisterFunction("GetSleepModKey", "GTB_Settings", Keys_GetSleepModKey);
			a_vm->RegisterFunction("SetSleepModKey", "GTB_Settings", Keys_SetSleepModKey);
			a_vm->RegisterFunction("GetServeTimeKey", "GTB_Settings", Keys_GetServeTimeKey);
			a_vm->RegisterFunction("SetServeTimeKey", "GTB_Settings", Keys_SetServeTimeKey);
			a_vm->RegisterFunction("GetServeTimeModKey", "GTB_Settings", Keys_GetServeTimeModKey);
			a_vm->RegisterFunction("SetServeTimeModKey", "GTB_Settings", Keys_SetServeTimeModKey);

			// Compatibility aliases with Keys_ prefix
			a_vm->RegisterFunction("Keys_GetSleepKey", "GTB_Settings", Keys_GetSleepKey);
			a_vm->RegisterFunction("Keys_SetSleepKey", "GTB_Settings", Keys_SetSleepKey);
			a_vm->RegisterFunction("Keys_GetSleepModKey", "GTB_Settings", Keys_GetSleepModKey);
			a_vm->RegisterFunction("Keys_SetSleepModKey", "GTB_Settings", Keys_SetSleepModKey);
			a_vm->RegisterFunction("Keys_GetServeTimeKey", "GTB_Settings", Keys_GetServeTimeKey);
			a_vm->RegisterFunction("Keys_SetServeTimeKey", "GTB_Settings", Keys_SetServeTimeKey);
			a_vm->RegisterFunction("Keys_GetServeTimeModKey", "GTB_Settings", Keys_GetServeTimeModKey);
			a_vm->RegisterFunction("Keys_SetServeTimeModKey", "GTB_Settings", Keys_SetServeTimeModKey);
			return true;
		});
	}
}