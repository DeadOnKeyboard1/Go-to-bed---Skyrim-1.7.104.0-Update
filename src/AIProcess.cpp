#include "AIProcess.h"
#include "Actor.h"
#include "Offsets.h"

namespace Gotobed
{
	namespace Hooks
	{
		stl::HookData OnSitSleepStateChange{&AIProcess::OnSitSleepStateChange};
	}

	void AIProcess::OnSitSleepStateChange(Actor* a_actor, std::uint32_t a_newState, RE::RefHandle* a_refHandle, std::int32_t a_marker) {
		Hooks::OnSitSleepStateChange.call_orig(this, a_actor, a_newState, a_refHandle, a_marker);

		if (a_actor) {
			auto actorState = a_actor->AsActorState();
			auto sitSleepState = actorState ? actorState->GetSitSleepState() : RE::SIT_SLEEP_STATE::kNormal;
			spdlog::info("AIProcess::OnSitSleepStateChange: actor={:08X}, newState={}, sitSleepState={}",
				a_actor->GetFormID(), a_newState, static_cast<std::uint32_t>(sitSleepState));
			if (sitSleepState == RE::SIT_SLEEP_STATE::kNormal ||
			    (sitSleepState >= RE::SIT_SLEEP_STATE::kWantToSleep && sitSleepState <= RE::SIT_SLEEP_STATE::kWantToWake)) {
				a_actor->UpdateOutfit();
			}
		}
	}

	void AIProcess::InstallHooks() {
		Hooks::OnSitSleepStateChange.write_detour(Offsets::AIProcess::OnSitSleepStateChange.address());
		spdlog::info("AIProcess::InstallHooks: OnSitSleepStateChange detour installed");
	}
}