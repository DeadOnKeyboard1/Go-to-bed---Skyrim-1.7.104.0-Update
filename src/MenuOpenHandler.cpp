#include "MenuOpenHandler.h"
#include "Offsets.h"
#include "Settings.h"

namespace Gotobed
{
	std::uintptr_t CanProcess_Orig_Addr{0};
	std::uintptr_t ProcessButton_Orig_Addr{0};

	namespace {
		std::uint32_t GetGamepadKeyCode(std::uint32_t a_idCode)
		{
			static std::unordered_map<std::uint32_t, std::uint32_t> map {
				{0x0001, 266}, // dpad up
				{0x0002, 267}, // dpad down
				{0x0004, 268}, // dpad left
				{0x0008, 269}, // dpad right
				{0x0010, 270}, // start
				{0x0020, 271}, // back
				{0x0040, 272}, // left thumb
				{0x0080, 273}, // right thumb
				{0x0100, 274}, // left shoulder
				{0x0200, 275}, // right shoulder
				{0x1000, 276}, // a
				{0x2000, 277}, // b
				{0x4000, 278}, // x
				{0x8000, 279}, // y
				{0x0009, 280}, // left trigger
				{0x000A, 281}, // right trigger
			};

			if (!map.contains(a_idCode)) {
				return 0;
			}

			return map.at(a_idCode);
		}

		bool IsSleepKey(RE::ButtonEvent& a_event) {
			auto& settings = Settings::Get();
			auto userEvents = RE::UserEvents::GetSingleton();

			if (a_event.userEvent == userEvents->wait) {
				return true;
			}

			if (settings.keys.sleep != -1) {
				if (a_event.device == RE::INPUT_DEVICE::kKeyboard) {
					return a_event.idCode == settings.keys.sleep;
				}

				if (a_event.device == RE::INPUT_DEVICE::kGamepad) {
					return GetGamepadKeyCode(a_event.idCode) == settings.keys.sleep;
				}
			}

			return false;
		}

		bool IsServeTimeKey(RE::ButtonEvent& a_event) {
			auto& settings = Settings::Get();
			auto userEvents = RE::UserEvents::GetSingleton();

			if (settings.keys.serveTime == -1) {
				return a_event.userEvent == userEvents->wait;
			}

			if (a_event.device == RE::INPUT_DEVICE::kKeyboard) {
				return a_event.idCode == settings.keys.serveTime;
			}

			if (a_event.device == RE::INPUT_DEVICE::kGamepad) {
				return GetGamepadKeyCode(a_event.idCode) == settings.keys.serveTime;
			}

			return false;
		}

		template<class... Args>
		bool ShowMessageBox(const char* a_msg, void (*a_callback)(std::uint8_t), std::uint8_t a_unk3, std::uint32_t a_unk4, std::uint32_t a_unk5, Args... a_args) {
			using func_t = decltype(&ShowMessageBox<Args...>);
			REL::Relocation<func_t> func{Offsets::ShowMessageBox};
			return func(a_msg, a_callback, a_unk3, a_unk4, a_unk5, a_args...);
		}

		void ShowSleepWaitMenu(bool a_sleep) {
			using func_t = decltype(&ShowSleepWaitMenu);
			REL::Relocation<func_t> func{Offsets::ShowSleepWaitMenu};
			func(a_sleep);
		}

		void ShowServeSentenceQuestion() {
			auto settings = RE::GameSettingCollection::GetSingleton();
			auto sServeSentenceQuestion = settings->GetSetting("sServeSentenceQuestion")->GetString();
			auto sYes = settings->GetSetting("sYes")->GetString();
			auto sNo = settings->GetSetting("sNo")->GetString();
			auto callback = [](std::uint8_t a_idx) {
				if (a_idx == 1) {
					RE::PlayerCharacter::GetSingleton()->ServePrisonTime();
				}
			};

			ShowMessageBox(sServeSentenceQuestion, callback, 1, 0x19, 4, sYes, sNo, nullptr);
		}
	}

	bool MenuOpenHandler::CanProcess_Orig(RE::InputEvent* a_event) {
		using func_t = decltype(&MenuOpenHandler::CanProcess_Orig);
		REL::Relocation<func_t> func{CanProcess_Orig_Addr};
		return func(this, a_event);
	}

	bool MenuOpenHandler::CanProcess_Hook(RE::InputEvent* a_event) {
		if (a_event && a_event->eventType == RE::INPUT_EVENT_TYPE::kButton) {
			if (IsSleepKey(*a_event->AsButtonEvent()) || IsServeTimeKey(*a_event->AsButtonEvent())) {
				return true;
			}
		}

		return CanProcess_Orig(a_event);
	}

	bool MenuOpenHandler::ProcessButton_Orig(RE::ButtonEvent* a_event) {
		using func_t = decltype(&MenuOpenHandler::ProcessButton_Orig);
		REL::Relocation<func_t> func{ProcessButton_Orig_Addr};
		return func(this, a_event);
	}

	bool MenuOpenHandler::ProcessButton_Hook(RE::ButtonEvent* a_event) {
		if (a_event && a_event->IsDown()) {
			if (IsServeTimeKey(*a_event) && OnServeTimeButtonDown()) {
				return true;
			}

			if (IsSleepKey(*a_event)) {
				if (OnSleepButtonDown()) {
					return true;
				}

				// If player is in bed, consume the key so vanilla doesn't open the Wait menu in bed!
				auto player = RE::PlayerCharacter::GetSingleton();
				auto actorState = player ? player->AsActorState() : nullptr;
				if (actorState) {
					auto sitSleepState = actorState->GetSitSleepState();
					if (sitSleepState == RE::SIT_SLEEP_STATE::kIsSleeping ||
					    sitSleepState == RE::SIT_SLEEP_STATE::kWaitingForSleepAnim ||
					    sitSleepState == RE::SIT_SLEEP_STATE::kWantToSleep) {
						return true;
					}
				}
			}
		}

		return ProcessButton_Orig(a_event);
	}

	static inline bool IsKeyPressed(const RE::BSWin32KeyboardDevice* a_kb, std::uint32_t a_keyCode) {
		return a_kb && (a_keyCode < sizeof(a_kb->curState)) && ((a_kb->curState[a_keyCode] & 0x80) != 0);
	}

	bool MenuOpenHandler::OnSleepButtonDown() {
		auto& settings = Settings::Get();

		auto ui = RE::UI::GetSingleton();

		if (ui->numPausesGame > 0 || ui->IsMenuOpen(RE::FaderMenu::MENU_NAME)) {
			spdlog::info("Sleep button ignored: game is paused or fader menu is open");
			return false;
		}

		auto player = RE::PlayerCharacter::GetSingleton();
		auto actorState = player ? player->AsActorState() : nullptr;

		if (!actorState || actorState->GetLifeState() == RE::ACTOR_LIFE_STATE::kDying || actorState->GetLifeState() == RE::ACTOR_LIFE_STATE::kDead) {
			spdlog::info("Sleep button ignored: actorState invalid or player dying/dead");
			return false;
		}

		auto sitSleepState = actorState->GetSitSleepState();
		if (sitSleepState != RE::SIT_SLEEP_STATE::kIsSleeping &&
		    sitSleepState != RE::SIT_SLEEP_STATE::kWaitingForSleepAnim &&
		    sitSleepState != RE::SIT_SLEEP_STATE::kWantToSleep) {
			spdlog::info("Sleep button ignored: player is not in bed (sitSleepState = {}, expected 7/kIsSleeping)", static_cast<std::uint32_t>(sitSleepState));
			return false;
		}

		// Only check modifier key if a custom sleep key is configured AND a modifier key is configured
		if (settings.keys.sleep != -1 && settings.keys.sleepMod != -1) {
			auto kb = RE::BSInputDeviceManager::GetSingleton()->GetKeyboard();
			if (!IsKeyPressed(kb, settings.keys.sleepMod)) {
				spdlog::info("Sleep button ignored: modifier key {} not pressed", settings.keys.sleepMod);
				return false;
			}
		}

		spdlog::info("Sleep button accepted: opening sleep menu!");
		ShowSleepWaitMenu(true);

		return true;
	}

	bool MenuOpenHandler::OnServeTimeButtonDown() {
		auto& settings = Settings::Get();

		if (settings.keys.serveTimeMod != -1) {
			auto kb = RE::BSInputDeviceManager::GetSingleton()->GetKeyboard();
			if (!IsKeyPressed(kb, settings.keys.serveTimeMod)) {
				return false;
			}
		}

		auto ui = RE::UI::GetSingleton();

		if (ui->numPausesGame > 0 || ui->IsMenuOpen(RE::FaderMenu::MENU_NAME)) {
			return false;
		}

		auto player = RE::PlayerCharacter::GetSingleton();
		auto actorState = player ? player->AsActorState() : nullptr;

		if (!actorState || actorState->GetLifeState() == RE::ACTOR_LIFE_STATE::kDying || actorState->GetLifeState() == RE::ACTOR_LIFE_STATE::kDead) {
			return false;
		}

		auto& playerData = player->GetPlayerRuntimeData();
		if (playerData.jailSentence <= 0 || playerData.playerFlags.escaping) {
			return false;
		}

		ShowServeSentenceQuestion();

		return true;
	}

	void MenuOpenHandler::InstallHooks() {
		CanProcess_Orig_Addr = Offsets::MenuOpenHandler::CanProcess.address();
		auto CanProcess_Hook_Addr = &MenuOpenHandler::CanProcess_Hook;
		ProcessButton_Orig_Addr = Offsets::MenuOpenHandler::ProcessButton.address();
		auto ProcessButton_Hook_Addr = &MenuOpenHandler::ProcessButton_Hook;

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(reinterpret_cast<PVOID*>(&CanProcess_Orig_Addr), reinterpret_cast<PVOID&>(CanProcess_Hook_Addr));
		DetourAttach(reinterpret_cast<PVOID*>(&ProcessButton_Orig_Addr), reinterpret_cast<PVOID&>(ProcessButton_Hook_Addr));

		if (DetourTransactionCommit() != NO_ERROR) {
			spdlog::error("failed to attach detour");
		}
	}
} 