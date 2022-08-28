#include "../tomb4/pch.h"
#include "controller.h"

long XI_ReadController(long& x, long& y)
{
	XINPUT_STATE state;
	long pressed;

	pressed = 0;

	for (int i = 0; i < 4; i++)
	{
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		if (XInputGetState(i, &state) != ERROR_SUCCESS)
			continue;

		pressed |= state.Gamepad.wButtons;

		if (state.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
			pressed |= 0x10000;

		if (state.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
			pressed |= 0x20000;

		x = state.Gamepad.sThumbLX >> 11;
		y = state.Gamepad.sThumbLY >> 11;
		break;
	}

	return pressed;
}
