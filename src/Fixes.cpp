#include "Fixes.h"
#include "Offsets.h"

namespace Gotobed::Fixes
{
	namespace MultipleMarkersReservation
	{
		class TESObjectREFR: public RE::TESObjectREFR
		{
		public:
			bool SetMarkerReserved(std::uint32_t a_marker, RE::Actor* a_actor, bool a_reserved, bool a_ignoreUsed);
		};

		namespace Hooks
		{
			stl::HookData SetMarkerReserved{&TESObjectREFR::SetMarkerReserved};
		};

		bool TESObjectREFR::SetMarkerReserved(std::uint32_t a_marker, RE::Actor* a_actor, bool a_reserved, bool a_ignoreUsed) {
			// CommonLibSSE-NG reaches Actor's runtime data through an accessor, because its base
			// offset shifts between runtimes (0xE0 before 1.6.629, 0xE8 after).
			auto middleHigh = a_actor->GetActorRuntimeData().currentProcess->middleHigh;

			if (middleHigh && middleHigh->reservationSlot != a_marker) {					
				if (middleHigh->reservationSlot != -1) {
					Hooks::SetMarkerReserved.call_orig(this, middleHigh->reservationSlot, nullptr, false, false);
				}
				middleHigh->reservationSlot = a_marker;
			}

			return Hooks::SetMarkerReserved.call_orig(this, a_marker, a_actor, a_reserved, a_ignoreUsed);
		}


		void Install() {
			Hooks::SetMarkerReserved.write_thunk(Offsets::BGSProcedureSitSleepExecState::ActivateTarget.address() + 0x02B4);
			spdlog::info("Fixes: MultipleMarkersReservation installed");
		}
	}
}