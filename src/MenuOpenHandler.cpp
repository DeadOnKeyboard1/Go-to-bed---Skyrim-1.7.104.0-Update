#include "MenuOpenHandler.h"
#include "Offsets.h"
#include "UIFuncs.h"
#include "Settings.h"

namespace Gotobed
{
	namespace Hooks
	{
		stl::HookData ProcessButton{&MenuOpenHandler::ProcessButtonHook};
		stl::HookData CanProcess{&MenuOpenHandler::CanProcessHook};
	}

	namespace detail
	{
		std::uint32_t GetGamepadKeyCode(std::uint32_t a_idCode)
		{
			static const std::unordered_map<std::uint32_t, std::uint32_t> map {
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

			auto it = map.find(a_idCode);
			return it != map.end() ? it->second : 0;
		}

		bool IsKeyPressed(std::uint32_t a_key) {
			const auto kb = RE::BSInputDeviceManager::GetSingleton()->GetKeyboard();
			if (!kb) {
				return false;
			}
			const auto& keys = kb->GetRuntimeData().curState;
			return a_key < sizeof(keys) && (keys[a_key] & 0x80) != 0;
		}

		bool IsSleepKey(RE::ButtonEvent* a_event) {
			if (!a_event) {
				return false;
			}
			auto& settings = Settings::Get();
			auto userEvents = RE::UserEvents::GetSingleton();

			if (a_event->userEvent == userEvents->wait) {
				return true;
			}

			if (settings.keys.sleep != -1) {
				if (a_event->device == RE::INPUT_DEVICE::kKeyboard) {
					return a_event->idCode == static_cast<std::uint32_t>(settings.keys.sleep);
				}
				if (a_event->device == RE::INPUT_DEVICE::kGamepad) {
					return GetGamepadKeyCode(a_event->idCode) == static_cast<std::uint32_t>(settings.keys.sleep);
				}
			}

			return false;
		}

		bool IsServeTimeKey(RE::ButtonEvent* a_event) {
			if (!a_event) {
				return false;
			}
			auto& settings = Settings::Get();
			auto userEvents = RE::UserEvents::GetSingleton();

			if (settings.keys.serveTime == -1) {
				return a_event->userEvent == userEvents->wait;
			}

			if (a_event->device == RE::INPUT_DEVICE::kKeyboard) {
				return a_event->idCode == static_cast<std::uint32_t>(settings.keys.serveTime);
			}

			if (a_event->device == RE::INPUT_DEVICE::kGamepad) {
				return GetGamepadKeyCode(a_event->idCode) == static_cast<std::uint32_t>(settings.keys.serveTime);
			}

			return false;
		}
	}

	bool MenuOpenHandler::CanProcessHook(RE::InputEvent* a_event) {
		if (a_event && a_event->eventType == RE::INPUT_EVENT_TYPE::kButton) {
			if (detail::IsSleepKey(a_event->AsButtonEvent()) || detail::IsServeTimeKey(a_event->AsButtonEvent())) {
				return true;
			}
		}

		return Hooks::CanProcess.call_orig(this, a_event);
	}

	bool MenuOpenHandler::ProcessButtonHook(RE::ButtonEvent* a_event) {
		auto& settings = Settings::Get();

		if (a_event && a_event->IsDown()) {
			if (detail::IsServeTimeKey(a_event) &&
			    (settings.keys.serveTimeMod == -1 || detail::IsKeyPressed(static_cast<std::uint32_t>(settings.keys.serveTimeMod))) &&
			    OnServeTimeButtonDown()) {
				return true;
			}

			if (detail::IsSleepKey(a_event)) {
				if ((settings.keys.sleepMod == -1 || detail::IsKeyPressed(static_cast<std::uint32_t>(settings.keys.sleepMod))) &&
				    OnSleepButtonDown()) {
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

		return Hooks::ProcessButton.call_orig(this, a_event);
	}

	bool MenuOpenHandler::OnSleepButtonDown() {
		auto ui = RE::UI::GetSingleton();

		if (ui->numPausesGame > 0 || ui->IsMenuOpen(RE::FaderMenu::MENU_NAME)) {
			return false;
		}

		auto player = RE::PlayerCharacter::GetSingleton();
		auto actorState = player ? player->AsActorState() : nullptr;

		if (!actorState || actorState->GetLifeState() == RE::ACTOR_LIFE_STATE::kDying || actorState->GetLifeState() == RE::ACTOR_LIFE_STATE::kDead) {
			return false;
		}

		auto sitSleepState = actorState->GetSitSleepState();
		if (sitSleepState != RE::SIT_SLEEP_STATE::kIsSleeping &&
		    sitSleepState != RE::SIT_SLEEP_STATE::kWaitingForSleepAnim &&
		    sitSleepState != RE::SIT_SLEEP_STATE::kWantToSleep) {
			return false;
		}

		UIFuncs::ShowSleepWaitMenu(true);

		return true;
	}

	bool MenuOpenHandler::OnServeTimeButtonDown() {
		auto ui = RE::UI::GetSingleton();

		if (ui->numPausesGame > 0 || ui->IsMenuOpen(RE::FaderMenu::MENU_NAME)) {
			return false;
		}

		auto player = RE::PlayerCharacter::GetSingleton();
		auto actorState = player ? player->AsActorState() : nullptr;

		if (!actorState || actorState->GetLifeState() == RE::ACTOR_LIFE_STATE::kDying || actorState->GetLifeState() == RE::ACTOR_LIFE_STATE::kDead) {
			return false;
		}

		if (player->GetPlayerRuntimeData().jailSentence <= 0 || player->GetPlayerFlags().escaping) {
			return false;
		}

		UIFuncs::ShowServeSentenceQuestion();

		return true;
	}

	void MenuOpenHandler::InstallHooks() {
		Hooks::CanProcess.write_detour(Offsets::MenuOpenHandler::CanProcess.address());
		Hooks::ProcessButton.write_detour(Offsets::MenuOpenHandler::ProcessButton.address());
	}
}