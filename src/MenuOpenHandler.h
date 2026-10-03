#pragma once

namespace Gotobed
{
	class MenuOpenHandler: public RE::MenuOpenHandler
	{
	public:
		bool CanProcessHook(RE::InputEvent* a_event);
		bool ProcessButtonHook(RE::ButtonEvent* a_event);
		bool OnSleepButtonDown();
		bool OnServeTimeButtonDown();

		static void InstallHooks();
	};
}