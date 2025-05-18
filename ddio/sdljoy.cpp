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

#include "joystick.h"

#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_joystick.h>
#include <SDL3/SDL_stdinc.h>
#include <cstdlib>
#include <cstdint>
#include <plog/Log.h>
#include <vector>

#include <SDL3/SDL.h>

#include "args.h"
#include "log.h"

//	---------------------------------------------------------------------------
//	globals

static int16_t specificJoy = -1;

struct GamepadBindings {
  std::vector<uint32_t> axis_bindings;
  std::vector<uint32_t> pov_bindings;
  std::vector<uint32_t> button_bindings;
};

struct Joystick_t {
  SDL_JoystickID id;
  SDL_Joystick *handle;
  tJoyInfo info;
  GamepadBindings bindings;
};

static std::vector<Joystick_t> Joysticks;

namespace {

inline uint8_t map_hat(uint8_t value) {
  switch (value) {
  case SDL_HAT_CENTERED:
    return JOYPOV_CENTER;
    break;
  case SDL_HAT_UP:
    return JOYPOV_CENTER;
    break;
  case SDL_HAT_UP | SDL_HAT_RIGHT:
    return 0x20;
    break;
  case SDL_HAT_RIGHT:
    return JOYPOV_RIGHT;
    break;
  case SDL_HAT_RIGHT | SDL_HAT_DOWN:
    return 0x60;
    break;
  case SDL_HAT_DOWN:
    return 0x80;
    break;
  case SDL_HAT_DOWN | SDL_HAT_LEFT:
    return 0xA0;
    break;
  case SDL_HAT_LEFT:
    return JOYPOV_LEFT;
    break;
  case SDL_HAT_LEFT | SDL_HAT_UP:
    return 0xE0;
    break;
  default:
    return JOYPOV_CENTER;
  }
}

/**
 * Closes connection with controller.
 */
static void joy_CloseStick(tJoystick_id joy) {
  SDL_CloseJoystick(Joysticks.at(joy).handle);
  Joysticks[joy].handle = nullptr;
}

/**
 * Initialize the `Joysticks` vector with SDL joysticks ids, taking the `-joystick` argument into account
 */
bool joy_InitStick(tJoystick_id joy) {
  //	close down already open joystick.
  ::joy_CloseStick(joy);

  SDL_JoystickID SDL_joyId = Joysticks.at(joy).id;

  SDL_Joystick *stick = SDL_OpenJoystick(SDL_joyId);
  Joysticks.at(joy).handle = stick;
  if (!stick) {
    LOG_WARNING << "Could not open joystick #" << joy << " : " << SDL_GetError();
    return false;
  }

  tJoyInfo &caps = Joysticks.at(joy).info;
  caps.name = SDL_GetJoystickNameForID(SDL_joyId);
  caps.num_btns = SDL_GetNumJoystickButtons(stick);
  caps.num_axis = SDL_GetNumJoystickAxes(stick);
  caps.num_povs = SDL_GetNumJoystickHats(stick);

  GamepadBindings &bind = Joysticks.at(joy).bindings;
  bind.button_bindings.resize(caps.num_btns, SDL_GAMEPAD_BUTTON_INVALID);
  bind.axis_bindings.resize(caps.num_axis, SDL_GAMEPAD_AXIS_INVALID);
  bind.pov_bindings.resize(caps.num_axis, SDL_GAMEPAD_AXIS_INVALID);

  if (SDL_IsGamepad(joy)) {
    SDL_Gamepad *controller = SDL_OpenGamepad(SDL_joyId);
    int binding_count = 0;
    SDL_GamepadBinding **bindings = SDL_GetGamepadBindings(controller, &binding_count);
    for (int i = 0; i < binding_count; i++) {
      if (bindings[i]->input_type == SDL_GAMEPAD_BINDTYPE_AXIS &&
          bindings[i]->output_type == SDL_GAMEPAD_BINDTYPE_AXIS) {
        bind.axis_bindings.at(bindings[i]->input.axis.axis) = bindings[i]->output.axis.axis;
      } else if (bindings[i]->input_type == SDL_GAMEPAD_BINDTYPE_HAT &&
                 bindings[i]->output_type == SDL_GAMEPAD_BINDTYPE_AXIS) {
        bind.pov_bindings.at(bindings[i]->input.hat.hat) = bindings[i]->output.axis.axis;
      } else if (bindings[i]->input_type == SDL_GAMEPAD_BINDTYPE_BUTTON &&
                 bindings[i]->output_type == SDL_GAMEPAD_BINDTYPE_BUTTON) {
        bind.button_bindings.at(bindings[i]->input.button) = bindings[i]->output.button;
      }
    }
    SDL_free(bindings);
  }

  LOG_DEBUG.printf("JOYSTICK: Initialized stick named [%s].", caps.name.c_str());
  LOG_DEBUG.printf("JOYSTICK: (%d) axes, (%d) hats, and (%d) buttons.", caps.num_btns, caps.num_povs, caps.num_axis);

  return (Joysticks.at(joy).handle != nullptr);
}


/**
 * Allocate the `Jotsticks` strucutre, storing joystick information
*/
int joyCreateStructures() {
  int32_t joyCount = 0;
  SDL_JoystickID *joystickIds = SDL_GetJoysticks(&joyCount);

  if ((specificJoy >= 0)) {
    if (specificJoy < joyCount) {
      LOG_INFO.printf("Found requested joystick #%d", specificJoy);
      Joysticks.resize(1);
    } else {
      LOG_WARNING.printf("Could not find the requested joystick #%d, only found %d joysticks", specificJoy, joyCount);
      return 0;
    }
  } else {
    Joysticks.resize(joyCount);
  }

  // Set the values for SDL joystick Ids
  for (int32_t id = 0; id < joyCount; id++) {
    if (specificJoy == -1 || id == specificJoy) {
      Joysticks.at(id).id = joystickIds[id];
    }
  }

  LOG_INFO.printf("Joystick: Found %d joysticks.", Joysticks.size());
  SDL_free(joystickIds);

  return Joysticks.size();
}

} // namespace

//	---------------------------------------------------------------------------

//	joystick system initialization
bool joy_Init() {
  //	reinitialize joystick if already initialized.
  joy_Close();
  if (!SDL_InitSubSystem(SDL_INIT_GAMEPAD)) {
    LOG_ERROR << "Could not initialize SDL Gamepad subsystem";
    return false;
  }

  int rc = FindArgChar("-joystick", 'j');
  specificJoy = -1;
  if (rc > 0) {
    specificJoy = atoi(GameArgs[rc + 1]);
  }

  ::joyCreateStructures();

  bool success = true;
  for (tJoystick_id joyId = 0; joyId < static_cast<tJoystick_id>(Joysticks.size()); joyId++) {
    success &= ::joy_InitStick(joyId);
  }
  return success;
}

void joy_Close() {
  for (tJoystick_id joyId = 0; joyId < static_cast<tJoystick_id>(Joysticks.size()); joyId++) {
    joy_CloseStick(joyId);
  }
  SDL_QuitSubSystem(SDL_INIT_GAMEPAD);
}

//	returns true if joystick valid
bool joy_IsValid(tJoystick_id joy) {
  if (specificJoy >= 0) {
    if (joy != specificJoy) {
      return false;
    }
  }
  return joy < static_cast<tJoystick_id>(Joysticks.size()) && (Joysticks.at(joy).handle != nullptr);
}

uint32_t joy_GetCount() {
  return Joysticks.size();
}

bool joy_AxisIsTrigger(tJoystick_id joy, uint8_t axisId) {
  auto bind = Joysticks.at(joy).bindings.axis_bindings.at(axisId);
  return (bind == SDL_GAMEPAD_AXIS_LEFT_TRIGGER || bind == SDL_GAMEPAD_AXIS_RIGHT_TRIGGER);
}

//	retreive information about joystick.
tJoyInfo joy_GetJoyInfo(tJoystick_id joy) { return Joysticks.at(joy).info; }

//	retreive uncalibrated position of joystick
tJoyPos joy_GetRawPos(tJoystick_id joy) {
  tJoyPos pos = joy_GetPos(joy);

  for (uint8_t i = 0; i < Joysticks[joy].info.num_axis; ++i) {
    pos.axis[i] += 32767;
  }

  return pos;
}

//	returns the state of a stick
tJoyPos joy_GetPos(tJoystick_id joy) {
  tJoyPos pos;
  SDL_Joystick *stick;

  //	retrieve joystick info from the net, or locally.
  stick = Joysticks[joy].handle;
  if (!stick) {
    return pos;
  }

  for (uint8_t i = 0; i < Joysticks[joy].info.num_axis; ++i) {
      pos.axis[i] = SDL_GetJoystickAxis(stick, i);
  }

  for (uint8_t i = 0; i < Joysticks[joy].info.num_povs; ++i) {
    pos.pov[i] = ::map_hat(SDL_GetJoystickHat(stick, i));
  }

  for (uint8_t i = Joysticks[joy].info.num_btns; i >= 0; --i) {
    if (SDL_GetJoystickButton(stick, i)) {
      pos.buttons |= (1 << i);
    }
  }

  return pos;
}
