#include "stdafx.h"
#ifndef JETHA_UTILITYTYPES
#define JETHA_UTILITYTYPES

#include <vector>

struct vec2u
{
	unsigned int x;
	unsigned int y;
};

struct vec2f
{
	float x;
	float y;
};
#endif

#ifndef SDLINPUTTYPES
#define SDLINPUTTYPES
namespace SledgeInputDeviceType {
	enum InputDeviceType_ID {
		IDT_DEVICE,
		IDT_PAD,
		IDT_JOY
	};
}

namespace SledgeInputDevice {
	enum InputDevice_ID {
		ID_DEVICE,
		ID_PAD_0,
		ID_PAD_1,
		ID_PAD_2,
		ID_PAD_3,
		ID_JOY_0,
		ID_JOY_1,
		ID_JOY_2,
		ID_JOY_3,
		ID_TOTAL
	};
	const static char* DeviceName[] = {
		"device",
		"pad0",
		"pad1",
		"pad2",
		"pad3",
		"joy0",
		"joy1",
		"joy2",
		"joy3"
	};
	const static SledgeInputDeviceType::InputDeviceType_ID DeviceType[] = {		
		SledgeInputDeviceType::IDT_DEVICE,
		SledgeInputDeviceType::IDT_PAD,
		SledgeInputDeviceType::IDT_PAD,
		SledgeInputDeviceType::IDT_PAD,
		SledgeInputDeviceType::IDT_PAD,
		SledgeInputDeviceType::IDT_JOY,
		SledgeInputDeviceType::IDT_JOY,
		SledgeInputDeviceType::IDT_JOY,
		SledgeInputDeviceType::IDT_JOY
	};
}

namespace SledgeDeviceSensor {
	enum DeviceSensor_ID {
		IDS_KEYBOARD,
		IDS_POINTER,
		IDS_MOUSE_LEFT,
		IDS_MOUSE_MIDDLE,
		IDS_MOUSE_RIGHT,
		IDS_TOTAL
	};
	const static char* SensorName[] = {
		"keyboard",
		"pointer",
		"mouseLeft",
		"mouseMiddle",
		"mouseRight"
	};
}

namespace SledgePadSensorAxes
{
	enum {
		PS_STICK_LEFT,
		PS_STICK_RIGHT,
		PS_TRIGGERS,
		PS_BUTTONS,
		PS_TOTAL
	};
	const static char* SensorName[] = {
		"stickLeft",
		"stickRight",
		"triggers",
		"buttons"
	};
}

namespace SledgeJoySensors
{
	enum{
		JS_STICK,
		JS_BUTTONS,
		JS_TOTAL
	};
	const static char* SensorName[] = {
		"stick",
		"buttons"
	};
}

namespace SledgePadSensorButtons
{
	enum {
		PSB_BUTTON_A,
		PSB_BUTTON_B,
		PSB_BUTTON_X,
		PSB_BUTTON_Y,
		PSB_BUTTON_START,
		PSB_BUTTON_BACK,

		PSB_BUTTON_LEFTSTICK,
		PSB_BUTTON_RIGHTSTICK,

		PSB_BUTTON_LEFTSHOULDER,
		PSB_BUTTON_RIGHTSHOULDER,

		PSB_BUTTON_DPAD_UP,
		PSB_BUTTON_DPAD_DOWN,
		PSB_BUTTON_DPAD_LEFT,
		PSB_BUTTON_DPAD_RIGHT
	};
	const static char* ButtonName[] = {
		"btnA",
		"btnB",
		"btnX",
		"btnY",
		"start",
		"back",
		"stickLeft",
		"stickRight",
		"shoulderLeft",
		"shoulderRight",
		"dpadUp",
		"dpadDown",
		"dpadLeft",
		"dpadRight"
	};
}

namespace JethaSDLControllerAxis
{
	const static char* AxisName[] = {
		"Left X",
		"Left Y",
		"Right X",
		"Right Y",
		"Left Trigger",
		"Right Trigger"
	};
}

typedef struct buttonState
{
	bool state[SDL_CONTROLLER_BUTTON_MAX];
} buttonState;

typedef struct pingpongState
{
	buttonState pp[2];
};

typedef struct keybState
{
	bool state[SDL_NUM_SCANCODES];
};

typedef struct pingpongState_keyb
{
	keybState pp[2];
};


struct NormalizedController
{
	vec2f stick_left;
	vec2f stick_right;
	vec2f triggers;
	//buttonState lastButtonState;
};

struct NormalizedJoystick
{
	std::vector<vec2f> sticks;
	std::vector<bool> buttons;
};

struct SledgeDevice
{
	SledgeInputDevice::InputDevice_ID device_id;
	char*				name;
};

struct SledgeController
{
	SledgeInputDevice::InputDevice_ID device_id;

	SDL_GameController* controller;
	int					index;
	int					index_controller;
	char*				name;

	bool				connected;

	vec2f				stick_left;
	vec2f				stick_right;
	vec2f				triggers;
	std::vector<bool>	buttons;
};

struct SledgeJoystick
{
	SledgeInputDevice::InputDevice_ID device_id;

	SDL_Joystick*		joystick;
	int					index;
	int					index_joystick;
	char*				name;

	bool				connected;

	std::vector<vec2f>	sticks;
	std::vector<bool>	buttons;
};

#endif