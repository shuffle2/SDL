/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2022 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
/* This driver supports the Nintendo Switch Joy-Cons pair controllers */
#include "../../SDL_internal.h"

#ifdef SDL_JOYSTICK_HIDAPI

#include "SDL_hints.h"
#include "SDL_joystick.h"
#include "SDL_gamecontroller.h"
#include "SDL_hidapijoystick_c.h"


static SDL_bool
HIDAPI_DriverCombined_IsSupportedDevice(const char *name, SDL_GameControllerType type, Uint16 vendor_id, Uint16 product_id, Uint16 version, int interface_number, int interface_class, int interface_subclass, int interface_protocol)
{
    /* This is always explicitly created for combined devices */
    return SDL_FALSE;
}

static const char *
HIDAPI_DriverCombined_GetDeviceName(const char *name, Uint16 vendor_id, Uint16 product_id)
{
    return NULL;
}

static SDL_bool
HIDAPI_DriverCombined_InitDevice(SDL_HIDAPI_Device *device)
{
    return HIDAPI_JoystickConnected(device, NULL);
}

static int
HIDAPI_DriverCombined_GetDevicePlayerIndex(SDL_HIDAPI_Device *device, SDL_JoystickID instance_id)
{
    return -1;
}

static void
HIDAPI_DriverCombined_SetDevicePlayerIndex(SDL_HIDAPI_Device *device, SDL_JoystickID instance_id, int player_index)
{
}

static SDL_bool
HIDAPI_DriverCombined_OpenJoystick(SDL_HIDAPI_Device *device, SDL_Joystick *joystick)
{
    int i;

    for (i = 0; i < device->num_children; ++i) {
        SDL_HIDAPI_Device *child = device->children[i];
        if (!child->driver->OpenJoystick(child, joystick)) {
            while (i-- > 0) {
                child = device->children[i];
                child->driver->CloseJoystick(child, joystick);
            }
            return SDL_FALSE;
        }
    }
    return SDL_TRUE;
}

static int
HIDAPI_DriverCombined_RumbleJoystick(SDL_HIDAPI_Device *device, SDL_Joystick *joystick, Uint16 low_frequency_rumble, Uint16 high_frequency_rumble)
{
    int i;
    int result = -1;

    for (i = 0; i < device->num_children; ++i) {
        SDL_HIDAPI_Device *child = device->children[i];
        if (child->driver->RumbleJoystick(child, joystick, low_frequency_rumble, high_frequency_rumble) == 0) {
            result = 0;
        }
    }
    return result;
}

static int
HIDAPI_DriverCombined_RumbleJoystickTriggers(SDL_HIDAPI_Device *device, SDL_Joystick *joystick, Uint16 left_rumble, Uint16 right_rumble)
{
    int i;
    int result = -1;

    for (i = 0; i < device->num_children; ++i) {
        SDL_HIDAPI_Device *child = device->children[i];
        if (child->driver->RumbleJoystickTriggers(child, joystick, left_rumble, right_rumble) == 0) {
            result = 0;
        }
    }
    return result;
}

static Uint32
HIDAPI_DriverCombined_GetJoystickCapabilities(SDL_HIDAPI_Device *device, SDL_Joystick *joystick)
{
    int i;
    Uint32 caps = 0;

    for (i = 0; i < device->num_children; ++i) {
        SDL_HIDAPI_Device *child = device->children[i];
        caps |= child->driver->GetJoystickCapabilities(child, joystick);
    }
    return caps;
}

static int
HIDAPI_DriverCombined_SetJoystickLED(SDL_HIDAPI_Device *device, SDL_Joystick *joystick, Uint8 red, Uint8 green, Uint8 blue)
{
    int i;
    int result = -1;

    for (i = 0; i < device->num_children; ++i) {
        SDL_HIDAPI_Device *child = device->children[i];
        if (child->driver->SetJoystickLED(child, joystick, red, green, blue) == 0) {
            result = 0;
        }
    }
    return result;
}

static int
HIDAPI_DriverCombined_SendJoystickEffect(SDL_HIDAPI_Device *device, SDL_Joystick *joystick, const void *data, int size)
{
    return SDL_Unsupported();
}

static int
HIDAPI_DriverCombined_SetJoystickSensorsEnabled(SDL_HIDAPI_Device *device, SDL_Joystick *joystick, SDL_bool enabled)
{
    int i;
    int result = -1;

    for (i = 0; i < device->num_children; ++i) {
        SDL_HIDAPI_Device *child = device->children[i];
        if (child->driver->SetJoystickSensorsEnabled(child, joystick, enabled) == 0) {
            result = 0;
        }
    }
    return result;
}

static SDL_bool
HIDAPI_DriverCombined_UpdateDevice(SDL_HIDAPI_Device *device)
{
    int i;
    int result = SDL_TRUE;

    for (i = 0; i < device->num_children; ++i) {
        SDL_HIDAPI_Device *child = device->children[i];
        if (!child->driver->UpdateDevice(child)) {
            result = SDL_FALSE;
        }
    }
    return result;
}

static void
HIDAPI_DriverCombined_CloseJoystick(SDL_HIDAPI_Device *device, SDL_Joystick *joystick)
{
    int i;

    for (i = 0; i < device->num_children; ++i) {
        SDL_HIDAPI_Device *child = device->children[i];
        child->driver->CloseJoystick(child, joystick);
    }
}

static void
HIDAPI_DriverCombined_FreeDevice(SDL_HIDAPI_Device *device)
{
}

SDL_HIDAPI_DeviceDriver SDL_HIDAPI_DriverCombined =
{
    SDL_HINT_JOYSTICK_HIDAPI,
    SDL_TRUE,
    SDL_TRUE,
    HIDAPI_DriverCombined_IsSupportedDevice,
    HIDAPI_DriverCombined_GetDeviceName,
    HIDAPI_DriverCombined_InitDevice,
    HIDAPI_DriverCombined_GetDevicePlayerIndex,
    HIDAPI_DriverCombined_SetDevicePlayerIndex,
    HIDAPI_DriverCombined_UpdateDevice,
    HIDAPI_DriverCombined_OpenJoystick,
    HIDAPI_DriverCombined_RumbleJoystick,
    HIDAPI_DriverCombined_RumbleJoystickTriggers,
    HIDAPI_DriverCombined_GetJoystickCapabilities,
    HIDAPI_DriverCombined_SetJoystickLED,
    HIDAPI_DriverCombined_SendJoystickEffect,
    HIDAPI_DriverCombined_SetJoystickSensorsEnabled,
    HIDAPI_DriverCombined_CloseJoystick,
    HIDAPI_DriverCombined_FreeDevice,
};

#endif /* SDL_JOYSTICK_HIDAPI */

/* vi: set ts=4 sw=4 expandtab: */
