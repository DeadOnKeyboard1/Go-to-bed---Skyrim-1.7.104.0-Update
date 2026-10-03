#include "SettingsPapyrus.h"
#include "Settings.h"

namespace Gotobed::SettingsPapyrus
{
	void Write(RE::StaticFunctionTag*) {
		Settings::Get().Write();
	}

	std::int32_t GetSleepKey(RE::StaticFunctionTag*) {
		return Settings::Get().keys.sleep;
	}

	void SetSleepKey(RE::StaticFunctionTag*, std::int32_t a_key) {
		Settings::Get().keys.sleep = (a_key <= 1 || a_key > 255) ? -1 : a_key;
	}

	std::int32_t GetSleepModKey(RE::StaticFunctionTag*) {
		return Settings::Get().keys.sleepMod;
	}

	void SetSleepModKey(RE::StaticFunctionTag*, std::int32_t a_key) {
		Settings::Get().keys.sleepMod = (a_key <= 1 || a_key > 255) ? -1 : a_key;
	}

	std::int32_t GetServeTimeKey(RE::StaticFunctionTag*) {
		return Settings::Get().keys.serveTime;
	}

	void SetServeTimeKey(RE::StaticFunctionTag*, std::int32_t a_key) {
		Settings::Get().keys.serveTime = (a_key <= 1 || a_key > 255) ? -1 : a_key;
	}

	std::int32_t GetServeTimeModKey(RE::StaticFunctionTag*) {
		return Settings::Get().keys.serveTimeMod;
	}

	void SetServeTimeModKey(RE::StaticFunctionTag*, std::int32_t a_key) {
		Settings::Get().keys.serveTimeMod = (a_key <= 1 || a_key > 255) ? -1 : a_key;
	}

	void Register() {
		SKSE::GetPapyrusInterface()->Register([](RE::BSScript::IVirtualMachine* a_vm) {
			a_vm->RegisterFunction("Write", "GTB_Settings", Write);

			// Standard function names expected by GTB_Settings.psc and GTB_MCM.psc
			a_vm->RegisterFunction("GetSleepKey", "GTB_Settings", GetSleepKey);
			a_vm->RegisterFunction("SetSleepKey", "GTB_Settings", SetSleepKey);
			a_vm->RegisterFunction("GetSleepModKey", "GTB_Settings", GetSleepModKey);
			a_vm->RegisterFunction("SetSleepModKey", "GTB_Settings", SetSleepModKey);
			a_vm->RegisterFunction("GetServeTimeKey", "GTB_Settings", GetServeTimeKey);
			a_vm->RegisterFunction("SetServeTimeKey", "GTB_Settings", SetServeTimeKey);
			a_vm->RegisterFunction("GetServeTimeModKey", "GTB_Settings", GetServeTimeModKey);
			a_vm->RegisterFunction("SetServeTimeModKey", "GTB_Settings", SetServeTimeModKey);

			// Compatibility aliases with Keys_ prefix
			a_vm->RegisterFunction("Keys_GetSleepKey", "GTB_Settings", GetSleepKey);
			a_vm->RegisterFunction("Keys_SetSleepKey", "GTB_Settings", SetSleepKey);
			a_vm->RegisterFunction("Keys_GetSleepModKey", "GTB_Settings", GetSleepModKey);
			a_vm->RegisterFunction("Keys_SetSleepModKey", "GTB_Settings", SetSleepModKey);
			a_vm->RegisterFunction("Keys_GetServeTimeKey", "GTB_Settings", GetServeTimeKey);
			a_vm->RegisterFunction("Keys_SetServeTimeKey", "GTB_Settings", SetServeTimeKey);
			a_vm->RegisterFunction("Keys_GetServeTimeModKey", "GTB_Settings", GetServeTimeModKey);
			a_vm->RegisterFunction("Keys_SetServeTimeModKey", "GTB_Settings", SetServeTimeModKey);
			return true;
		});
	}
}