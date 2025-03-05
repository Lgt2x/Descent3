/*
* Descent 3
* Copyright (C) 2024 Parallax Software
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.

--- HISTORICAL COMMENTS FOLLOW ---

 * $Logfile: /DescentIII/Main/ddio_lnx/lnxjoy.cpp $
 * $Revision: 1.3 $
 * $Date: 2001/02/07 09:16:45 $
 * $Author: icculus $
 *
 * Linux joystick routines
 *
 * $Log: sdljoy.cpp,v $
 * Revision 1.3  2001/02/07 09:16:45  icculus
 * More robust debugging information.
 *
 * Revision 1.2  2000/06/29 22:15:25  hercules
 * Fixed hat motion
 *
 * Revision 1.1  2000/06/29 09:53:00  hercules
 * Use SDL joystick support (hats off to you! :)
 *
 * Revision 1.3  2000/06/24 01:15:15  icculus
 * patched to compile.
 *
 * Revision 1.2  2000/05/29 05:21:09  icculus
 * Changed a fprintf(stderr, ...) to an mprintf()...
 *
 * Revision 1.1.1.1  2000/04/18 00:00:33  icculus
 * initial checkin
 *
 *
 * 9     8/22/99 5:55p Jeff
 * fixed assert
 *
 * 8     8/19/99 3:46p Jeff
 * removed mprintfs
 *
 * 7     8/19/99 3:22p Jeff
 * added support for joystick driver version pre 1.0
 *
 * 6     8/18/99 9:47p Jeff
 * joystick support! for kernel 2..2+ clients...need to handle before that
 * still.
 *
 * 5     8/17/99 2:32p Jeff
 * fixed joy_GetPos
 *
 * 4     7/14/99 9:06p Jeff
 * added comment header
 *
 * $NoKeywords: $
 */

#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <array>
#include <vector>

#include <SDL3/SDL.h>

// rcg06182000 need this for specific joystick stuff.
#include "args.h"
#include "joystick.h"
#include "log.h"

//	---------------------------------------------------------------------------
//	globals

static int specificJoy = -1;
struct Joystick_t {
  SDL_Joystick *handle;
  tJoyInfo caps;
};

static std::vector<Joystick_t> Joysticks;

static int joyGetNumDevs(void);

//		closes connection with controller.
static void joy_CloseStick(tJoystick_id joy);

//	initializes a joystick
//		if server_adr is valid, a link is opened to another machine with a controller.
static bool joy_InitStick(tJoystick_id joy);

//	---------------------------------------------------------------------------
//	functions

//	joystick system initialization
bool joy_Init() {
  //	reinitialize joystick if already initialized.
  joy_Close();
  if (!SDL_InitSubSystem(SDL_INIT_GAMEPAD)) {
    LOG_ERROR << "Could not initialize Joystick";
    return false;
  }

  // rcg06182000 add support for specific joydev.
  int rc = FindArgChar("-joystick", 'j');
  specificJoy = -1;
  if ((rc > 0) && (GameArgs[rc + 1] != NULL)) {
    specificJoy = atoi(GameArgs[rc + 1]);
  }

  Joysticks.resize(joyGetNumDevs());

  // rcg06182000 specific joystick support.
  if (specificJoy >= 0) {
    joy_InitStick((tJoystick_id)specificJoy);
  } // if
  else {
    //	initialize joystick list
    for (int i = 0; i < Joysticks.size(); i++) {
      joy_InitStick((tJoystick_id)i);
    }
  } // else
  return true;
}

void joy_Close() {
  //	initialize joystick list
  for (int i = 0; i < Joysticks.size(); i++) {
    joy_CloseStick((tJoystick_id)i);
  }
  SDL_QuitSubSystem(SDL_INIT_GAMEPAD);
}

//	initializes a joystick
static bool joy_InitStick(tJoystick_id joy) {
  //	close down already open joystick.
  joy_CloseStick(joy);

  SDL_Joystick *stick = SDL_OpenJoystick(joy);
  Joysticks.at(joy).handle = stick;
  if (stick) {
    tJoyInfo caps;

    memset(&caps, 0, (sizeof(caps)));
    strncpy(caps.name, SDL_GetJoystickNameForID(joy), sizeof(caps.name) - 1);
    caps.num_btns = SDL_GetNumJoystickButtons(stick);
    int axes = SDL_GetNumJoystickAxes(stick);

    std::array<uint16_t, 6> axis_flags{JOYFLAG_XVALID, JOYFLAG_YVALID, JOYFLAG_ZVALID,
                                       JOYFLAG_RVALID, JOYFLAG_UVALID, JOYFLAG_VVALID};
    for (int axis = 0; axis < SDL_GetNumJoystickAxes(stick); axis++) {
      caps.axes_mask |= axis_flags[axis];

      if (SDL_IsGamepad(joy)) {
        SDL_Gamepad * controller = SDL_OpenGamepad(joy);

        int binding_count = 0;
        SDL_GamepadBinding **bindings = SDL_GetGamepadBindings(controller, &binding_count);
        for (int i = 0; i < binding_count; i++) {
          if (bindings[i]->input_type == SDL_GAMEPAD_BINDTYPE_AXIS && bindings[i]->output_type == SDL_GAMEPAD_BINDTYPE_AXIS) {
              LOG_DEBUG << "Axis input is " << bindings[i]->input.axis.axis << " min " << bindings[i]->input.axis.axis_min << " max " << bindings[i]->input.axis.axis_max;
              LOG_DEBUG << "Axis output is " << bindings[i]->output.axis.axis << " min " << bindings[i]->output.axis.axis_min << " max " << bindings[i]->output.axis.axis_max;
              if (bindings[i]->output.axis.axis == SDL_GAMEPAD_AXIS_LEFT_TRIGGER) {
                caps.trigger_axis_mask |= axis_flags[axis];
              }
              if (bindings[i]->output.axis.axis == SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) {
                caps.trigger_axis_mask |= axis_flags[axis];
              }
            }
        }
        SDL_free(bindings);
      }

      else {
        // Joystick is not recognized as a game controller.
        // We try to guess whether it is a one-directional analog trigger based on its initial value.

        int16_t initialVal = 0;
        SDL_GetJoystickAxisInitialState(stick, axis, &initialVal);
        LOG_DEBUG << "Initial axis " << axis << " value is " << initialVal;

        if (initialVal < -32768 * 0.75) {
          // Axis is an analog button/trigger, because it's initial value
          // is at the start of the range and not in the middle
          LOG_DEBUG << "Axis " << axis << " is an analog button";
          caps.trigger_axis_mask |= axis_flags[axis];
        } else {
          LOG_DEBUG << "Axis " << axis << " is a bidirectional axis";
        }
      }
    }

    int hats = SDL_GetNumJoystickHats(stick);
    switch (hats) {
    default:
      // Fall through to 4 hats
    case 4:
      caps.axes_mask |= JOYFLAG_POV4VALID;
    case 3:
      caps.axes_mask |= JOYFLAG_POV3VALID;
    case 2:
      caps.axes_mask |= JOYFLAG_POV2VALID;
    case 1:
      caps.axes_mask |= JOYFLAG_POVVALID;
    case 0:
      break;
    }
    Joysticks.at(joy).caps = caps;

    LOG_DEBUG.printf("JOYSTICK: Initialized stick named [%s].", caps.name);
    LOG_DEBUG.printf("JOYSTICK: (%d) axes, (%d) hats, and (%d) buttons.", axes, hats, caps.num_btns);
  }

  return (Joysticks.at(joy).handle != NULL);
}

//  closes connection with controller.
static void joy_CloseStick(tJoystick_id joy) {
  SDL_CloseJoystick(Joysticks.at(joy).handle);
  Joysticks[joy].handle = nullptr;
}

//	returns true if joystick valid
bool joy_IsValid(tJoystick_id joy) {
  if (specificJoy >= 0) {
    if (joy != specificJoy) {
      return false;
    }
  }
  return joy < Joysticks.size() && (Joysticks.at(joy).handle != NULL);
}

//	retreive information about joystick.
void joy_GetJoyInfo(tJoystick_id joy, tJoyInfo *info) { memcpy(info, &Joysticks.at(joy).caps, sizeof(tJoyInfo)); }

//	retreive uncalibrated position of joystick
#define LNX_JOYAXIS_RANGE 65535
void joy_GetRawPos(tJoystick_id joy, tJoyPos *pos) {
  joy_GetPos(joy, pos);

  pos->x = (pos->x + 32767);
  pos->y = (pos->y + 32767);
  pos->z = (pos->z + 32767);
  pos->r = (pos->r + 32767);
  pos->u = (pos->u + 32767);
  pos->v = (pos->v + 32767);
}

static inline uint32_t map_hat(Uint8 value) {
  uint32_t mapped = 0;

  switch (value) {
  case SDL_HAT_CENTERED:
    mapped = JOYPOV_CENTER;
    break;
  case SDL_HAT_UP:
    mapped = 0x00;
    break;
  case SDL_HAT_UP | SDL_HAT_RIGHT:
    mapped = 0x20;
    break;
  case SDL_HAT_RIGHT:
    mapped = 0x40;
    break;
  case SDL_HAT_RIGHT | SDL_HAT_DOWN:
    mapped = 0x60;
    break;
  case SDL_HAT_DOWN:
    mapped = 0x80;
    break;
  case SDL_HAT_DOWN | SDL_HAT_LEFT:
    mapped = 0xA0;
    break;
  case SDL_HAT_LEFT:
    mapped = 0xC0;
    break;
  case SDL_HAT_LEFT | SDL_HAT_UP:
    mapped = 0xE0;
    break;
  }
  return mapped;
}

//	returns the state of a stick, remote or otherwise
void joy_GetPos(tJoystick_id joy, tJoyPos *pos) {
  SDL_Joystick *stick;
  int i;

  memset(pos, 0, (sizeof(*pos)));

  //	retrieve joystick info from the net, or locally.
  stick = Joysticks[joy].handle;
  if (stick) {
    uint32_t mask;

    mask = Joysticks[joy].caps.axes_mask;
    if (mask & JOYFLAG_XVALID) {
      pos->x = SDL_GetJoystickAxis(stick, 0);
    }
    if (mask & JOYFLAG_YVALID) {
      pos->y = SDL_GetJoystickAxis(stick, 1);
    }
    if (mask & JOYFLAG_ZVALID) {
      pos->z = SDL_GetJoystickAxis(stick, 2);
    }
    if (mask & JOYFLAG_RVALID) {
      pos->r = SDL_GetJoystickAxis(stick, 3);
    }
    if (mask & JOYFLAG_UVALID) {
      pos->u = SDL_GetJoystickAxis(stick, 4);
    }
    if (mask & JOYFLAG_VVALID) {
      pos->v = SDL_GetJoystickAxis(stick, 5);
    }
    for (i = 0; i < JOYPOV_NUM; ++i) {
      if (mask & (JOYFLAG_POVVALID << i)) {
        pos->pov[i] = map_hat(SDL_GetJoystickHat(stick, i));
      }
    }
    for (i = Joysticks[joy].caps.num_btns; i >= 0; --i) {
      if (SDL_GetJoystickButton(stick, i)) {
        pos->buttons |= (1 << i);
      }
    }
  }
}

static int joyGetNumDevs(void) {
  int found = 0;

  int joyCount = 0;
  SDL_JoystickID *joysticks = SDL_GetJoysticks(&joyCount);

  if ((specificJoy >= 0) && (specificJoy < joyCount)) {
    LOG_INFO.printf("Found requested joystick #%d", specificJoy);
    found = 1;
  } else {
    LOG_WARNING.printf("Could not find requested joystick #%d", specificJoy);
    specificJoy = -1;
  }
  if (specificJoy < 0) {
    found = joyCount;
  }

  LOG_INFO.printf("Joystick: Found %d joysticks.", found);
  SDL_free(joysticks);
  return found;
}

void ddio_InternalJoyFrame(void) {
  // All the work is done already in SDL_PumpEvents()
}
