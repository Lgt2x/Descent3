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

 * $Logfile: /DescentIII/Main/sdlcontroller/sdlcontroller.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:58:14 $
 * $Author: kevinb $
 *
 * Linux controller routines
 *
 * $Log: lnxcontroller.cpp,v $
 * Revision 1.1.1.1  2003/08/26 03:58:14  kevinb
 * initial 1.5 import
 *
 *
 * 7     9/23/99 12:10p Jeff
 * changes to reflect new wincontroller
 *
 * 6     8/18/99 8:52p Jeff
 * make up to date with windows
 *
 * 5     7/14/99 9:10p Jeff
 * added comment header
 *
 * $NoKeywords: $
 */

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <plog/Log.h>
#include <sys/types.h>

#include "controller.h"
#include "ddio.h"
#include "pserror.h"
#include "joystick.h"
#include "inffile.h"
#include "sdlcontroller.h"
#include "log.h"

// Sorry! This is needed for the semi-hacky mouselook support
#include "descent.h"
#include "player.h"
#include "object.h"
#include "pilot.h"
#include "multi.h"
#include "game.h"
// End of hacky includes

#define JOY_DEADZONE 0.20f
#define MOUSE_DEADZONE 0.00f

static float WinControllerTimer = 0.0f;
static float g_accum_frame_time = 0.0f;

sdlgameController::sdlgameController(int num_funcs, ct_function *funcs) : gameController(num_funcs, funcs) {
  this->init_controllers();

  // Initial function assignation, from default keys and controller mapping
  for (int i = 0; i < num_funcs; i++) {
    this->assign_function(&funcs[i]);
  }

  m_Suspended = 0;
  m_frame_timer_ms = -1;
  m_frame_time = 1.0f;
  g_accum_frame_time = 0.0f;

  sdlgameController::flush();
}

//	these functions suspend or resume any controller reading.  this is really only useful for
//	preemptive controller polling, but they should be used to activate and deactivate controller
//	reading.
void sdlgameController::suspend() { m_Suspended = 1; }
void sdlgameController::resume() {
  m_Suspended = 0;
  m_frame_timer_ms = -1;
  m_frame_time = 1.0f;
}

#define CONTROLLER_POLLING_TIME 50
#define MOUSE_POLLING_TIME (1.0f / 20.0f)

//	this functions polls the controllers if needed.  some systems may not need to implement
//	this function.
void sdlgameController::poll() {
  int64_t cur_frame_timer_ms;

  if (m_Suspended)
    return;

  cur_frame_timer_ms = timer_GetMSTime();
  if (m_frame_timer_ms == -1) {
    // don't poll this frame.
    m_frame_timer_ms = cur_frame_timer_ms;
    g_accum_frame_time = 0.0f;
    return;
  }

  m_frame_time = ((float)(cur_frame_timer_ms - m_frame_timer_ms) / 1000.0f);
  m_frame_timer_ms = cur_frame_timer_ms;
  g_accum_frame_time += m_frame_time;

  if (g_accum_frame_time >= MOUSE_POLLING_TIME) {
    g_accum_frame_time = 0.0f;
  }

  for (t_controller &ctrl : m_ControlList) {
    if (ctrl.type == CTID::JOYSTICK) {
      extctl_getpos(ctrl.id);
    } else if (ctrl.type == CTID::MOUSE) {
      mouse_geteval();
    }
  }
}

void sdlgameController::set_controller_deadzone(uint8_t ctl, float deadzone) {
  deadzone = std::clamp(deadzone, 0.0f, 0.9f);
  m_ControlList[ctl].deadzone = deadzone;
}

char Ctltext_AxisBindings[][16] = {"", "X-axis", "Y-axis", "Z-axis", "R-axis", "U-axis", "V-axis"};

static char Ctltext_BtnBindings[][16] = {
    "",      "btn1",  "btn2",  "btn3",  "btn4",  "btn5",  "btn6",  "btn7",  "btn8",  "btn9",  "btn10",
    "btn11", "btn12", "btn13", "btn14", "btn15", "btn16", "btn17", "btn18", "btn19", "btn20", "btn21",
    "btn22", "btn23", "btn24", "btn25", "btn26", "btn27", "btn28", "btn29", "btn30", "btn31", "btn32"};

char Ctltext_PovBindings[][16] = {"", "pov-U", "pov-R", "pov-D", "pov-L"};

#define NUM_AXISBINDSTRINGS std::size(Ctltext_AxisBindings)
#define NUM_BTNBINDSTRINGS std::size(Ctltext_BtnBindings)

// retrieves binding text for desired function, binding, etc.
const char *sdlgameController::get_binding_text(ct_type type, uint8_t ctrl, uint8_t bind) {
  static char binding_text[32];
  const char *str;

  if (ctrl == NULL_CONTROLLER) {
    return nullptr;
  }

  switch (type) {
    int pov_n;
  case ctAxis: {
    ASSERT(bind < NUM_AXISBINDSTRINGS);
    str = Ctltext_AxisBindings[bind];
    if (ctrl > 2) {
      snprintf(binding_text, sizeof(binding_text), "J%d:%s", (ctrl - 2) + 1, str);
    } else {
      return str;
    }
    break;
  }

  case ctMouseAxis: {
    str = ddio_MouseGetAxisText(((int8_t)bind) - 1);
    return str;
  }

  case ctButton: {
    ASSERT(bind < NUM_BTNBINDSTRINGS);
    str = Ctltext_BtnBindings[bind];
    if (ctrl > 2) {
      snprintf(binding_text, sizeof(binding_text), "J%d:%s", (ctrl - 2) + 1, str);
    } else {
      return str;
    }
    break;
  }

  case ctMouseButton: {
    str = ddio_MouseGetBtnText(((int8_t)bind) - 1);
    return str;
  }

  case ctPOV:
  case ctPOV2:
  case ctPOV3:
  case ctPOV4: {
    uint16_t povpos = bind;

    if (type == ctPOV)
      pov_n = 0;
    else
      pov_n = (type - ctPOV2) + 1;

    if (povpos == JOYPOV_UP)
      str = Ctltext_PovBindings[1];
    else if (povpos == JOYPOV_DOWN)
      str = Ctltext_PovBindings[3];
    else if (povpos == JOYPOV_LEFT)
      str = Ctltext_PovBindings[4];
    else if (povpos == JOYPOV_RIGHT)
      str = Ctltext_PovBindings[2];
    else
      str = Ctltext_PovBindings[0];
    if ((ctrl - 2) > 0) {
      if (pov_n) {
        snprintf(binding_text, sizeof(binding_text), "J%d:%s%d", (ctrl - 2) + 1, str, pov_n);
      } else {
        snprintf(binding_text, sizeof(binding_text), "J%d:%s", (ctrl - 2) + 1, str);
      }
    } else {
      if (pov_n) {
        snprintf(binding_text, sizeof(binding_text), "%s%d", str, pov_n);
      } else {
        return str;
      }
    }
    break;
  }
  case ctAnalogTrigger:
    snprintf(binding_text, sizeof(binding_text), "J%d:trig%d", (ctrl - 2) + 1, bind);
    break;

  case ctKey:
    break;

  default:
    if (type == ctNone) {
      Int3();
    }
    binding_text[0] = 0;
  }

  return binding_text;
}

//	flushes all controller information
void sdlgameController::flush() {
  bool old_mse = m_MouseActive, old_joy = m_JoyActive;

  ddio_KeyFlush();
  ddio_MouseQueueFlush();

  // does real flush
  mask_controllers(false, false);
  mask_controllers(old_joy, old_mse);
}

//	returns the value of a requested controller type. make sure you flush the controller before polling.
ct_config_data sdlgameController::get_controller_value(ct_type type_req) {
  //	will return the current value of a requested control type.
  ct_config_data val = MAKE_CONFIG_DATA(INVALID_CONTROLLER_INFO, NULL_BINDING);

  switch (type_req) {
    int pov_n;

  case ctNone:
    break;
  case ctKey:
    val = makeword(0, ddio_KeyInKey());
    break;

  case ctButton:
    for (size_t i = 2; i < m_ControlList.size(); i++) {
      for (uint8_t j = 0; j < m_ControlList[i].info.num_btns; j++) {
        if (m_ExtCtlStates[m_ControlList[i].id].btn_pos[j].btnpresses) {
          val = MAKE_CONFIG_DATA(CONTROLLER_CTL_INFO(i, NULL_CONTROLLER), CONTROLLER_CTL_VALUE(j + 1, NULL_BINDING));
          return val;
        }
      }
    }
    break;

  case ctMouseButton:
    for (int j = 0; j < CT_MAX_BUTTONS; j++) {
      if (ddio_MouseBtnUpCount(j)) {
        //	mprintf(0, "MseBtn %d down\n", j);
        val = MAKE_CONFIG_DATA(CONTROLLER_CTL_INFO(1, NULL_CONTROLLER), CONTROLLER_CTL_VALUE(j + 1, NULL_BINDING));
        return val;
      }
    }
    break;

  case ctAxis:
    for (size_t controllerId = 2; controllerId < m_ControlList.size(); controllerId++) {
      for (uint8_t axis = 0; axis < m_ControlList[controllerId].info.num_axis; axis++) {
        if (!joy_AxisIsTrigger(controllerId - 2, axis)) {
          get_controller_axis_value(controllerId, axis, &val);
        }
      }
    }
    break;
  case ctAnalogTrigger:
    for (size_t controllerId = 2; controllerId < m_ControlList.size(); controllerId++) {
      for (uint8_t axis = 0; axis < m_ControlList[controllerId].info.num_axis; axis++) {
        if (joy_AxisIsTrigger(controllerId - 2, axis)) {
          get_controller_trigger_value(controllerId, axis, &val);
        }
      }
    }
    break;
  case ctMouseAxis: {
    float pos = 0.0f;
    int ctl = CONTROLLER_CTL_INFO(1, NULL_CONTROLLER), id = 1;
    ASSERT(m_ControlList[id].type == CTID::MOUSE);
    for (int axis = 0; axis < m_ControlList[id].info.num_axis; axis++) {
      pos = get_axis_value(id, CT_X_AXIS, ctAnalog);
      if (std::abs(pos) >= 0.90f) {
        val = MAKE_CONFIG_DATA(ctl, CONTROLLER_CTL_VALUE(CT_V_AXIS, NULL_BINDING));
      }
    }
  } break;

  case ctPOV:
  case ctPOV2:
  case ctPOV3:
  case ctPOV4:
    if (type_req == ctPOV) {
      pov_n = 0;
    } else {
      pov_n = (type_req - ctPOV2) + 1;
    }
    for (size_t i = 2; i < m_ControlList.size(); i++) {
      if (m_ControlList[i].info.num_povs <= pov_n) {
        break;
      }

      std::array pov_positions{JOYPOV_RIGHT, JOYPOV_LEFT, JOYPOV_DOWN, JOYPOV_UP};
      for (auto &pov_pos : pov_positions) {
        float pos = get_pov_value(i, ctDigital, pov_n, pov_pos);
        if (pos)
          val = makeword(CONTROLLER_CTL_INFO(i, -1), CONTROLLER_CTL_VALUE(pov_pos, 0));
      }
    }
    break;
  }

  return val;
}

void sdlgameController::get_controller_axis_value(int controllerId, uint8_t axis, ct_config_data *val) {
  float limit = (m_ControlList[controllerId].sens[axis] > 1.5f)   ? 0.95f
                : (m_ControlList[controllerId].sens[axis] > 1.0f) ? 0.80f
                                                                  : (m_ControlList[controllerId].sens[axis] / 2);
  float pos = get_axis_value(controllerId, axis, ctAnalog);
  if (std::abs(pos) > limit) {
    *val =
        MAKE_CONFIG_DATA(CONTROLLER_CTL_INFO(controllerId, NULL_CONTROLLER), CONTROLLER_CTL_VALUE(axis, NULL_BINDING));
  }
}

void sdlgameController::get_controller_trigger_value(int controllerId, uint8_t axis, ct_config_data *val) {
  float limit = (m_ControlList[controllerId].sens[axis - 1] > 1.5f) ? 0.5f
                : (m_ControlList[controllerId].sens[axis - 1] > 1.0f)
                    ? 0.3f
                    : (m_ControlList[controllerId].sens[axis - 1] / 4);
  float pos = get_axis_value(controllerId, axis, ctAnalog);
  if (pos > limit) {
    *val =
        MAKE_CONFIG_DATA(CONTROLLER_CTL_INFO(controllerId, NULL_CONTROLLER), CONTROLLER_CTL_VALUE(axis, NULL_BINDING));
  }
}

//	sets the configuration of a function (type must be of an array == CTLBINDS_PER_FUNC)
void sdlgameController::set_controller_function(int id, const ct_type *type, ct_config_data value,
                                                const uint8_t *flags) {
  ct_element elem;

  if (id >= m_ElementList.size()) {
    LOG_ERROR << "Controller function out of bounds for id " << id << " >= " << m_ElementList.size();
    return;
  }

  // auto assign keyboard controller if type is key.
  if (type[0] == ctKey)
    elem.ctl_id[0] = CONTROLLER_CTL1_INFO(0);
  else
    elem.ctl_id[0] = CONTROLLER_CTL1_INFO(CONTROLLER_INFO(value));

  if (type[1] == ctKey)
    elem.ctl_id[1] = CONTROLLER_CTL2_INFO(0);
  else
    elem.ctl_id[1] = CONTROLLER_CTL2_INFO(CONTROLLER_INFO(value));

  elem.ctype[0] = type[0];
  elem.ctype[1] = type[1];
  elem.format = m_ElementList[id].format;
  elem.value[0] = CONTROLLER_CTL1_VALUE(CONTROLLER_VALUE(value));
  elem.value[1] = CONTROLLER_CTL2_VALUE(CONTROLLER_VALUE(value));
  elem.flags[0] = flags[0];
  elem.flags[1] = flags[1];
  elem.enabled = m_ElementList[id].enabled;

  //	if controller doesn't exist, set it to invalid.
  if (elem.ctl_id[0] > m_ControlList.size())
    elem.ctl_id[0] = NULL_LNXCONTROLLER;
  if (elem.ctl_id[1] >= m_ControlList.size())
    elem.ctl_id[1] = NULL_LNXCONTROLLER;

  assign_element(id, &elem);
}

//	returns information about a requested function (type must be of an array == CTLBINDS_PER_FUNC)
void sdlgameController::get_controller_function(int id, ct_type *type, ct_config_data *value, uint8_t *flags) {
  type[0] = m_ElementList[id].ctype[0];
  type[1] = m_ElementList[id].ctype[1];
  *value = makeword(CONTROLLER_CTL_INFO(m_ElementList[id].ctl_id[0], m_ElementList[id].ctl_id[1]),
                    CONTROLLER_CTL_VALUE(m_ElementList[id].value[0], m_ElementList[id].value[1]));
  flags[0] = m_ElementList[id].flags[0];
  flags[1] = m_ElementList[id].flags[1];
}

//	temporarily enables or disables a function
void sdlgameController::enable_function(int id, bool enable) { m_ElementList[id].enabled = enable; }

//	all systems need to implement this function.  this returns information about the controller
bool sdlgameController::get_packet(int id, ct_packet *packet, ct_format alt_format) {
  float val = 0.0f;

  ASSERT(id < m_ElementList.size());

  packet->format = (alt_format != ctNoFormat) ? alt_format : m_ElementList[id].format;
  alt_format = packet->format;

  WinControllerTimer = timer_GetTime();
  packet->flags = 0;

  if (!m_ElementList[id].enabled) {
    goto skip_packet_read;
  }

  //	check if the element's controller is valid.
  for (uint8_t i = 0; i < CTLBINDS_PER_FUNC; i++) {
    uint8_t value = m_ElementList[id].value[i];
    int8_t controller = m_ElementList[id].ctl_id[i];

    if (controller == -1 || m_ControlList[controller].type == CTID::INVALID) {
      continue;
    }
    switch (m_ElementList[id].ctype[i]) {
    case ctKey:
      if (value) {
        val = get_key_value(value, alt_format);
        if (KEY_STATE(value))
          packet->flags |= CTPK_ELEMENTACTIVE;
      }
      break;

    case ctMouseAxis:
      packet->flags |= CTPK_MOUSE;
    case ctAnalogTrigger:
      val = get_trigger_value(controller, value, alt_format);
    case ctAxis:
      val = get_axis_value(controller, value, alt_format, (m_ElementList[id].flags[i] & CTFNF_INVERT) ? true : false);
      if (m_ElementList[id].flags[i] & CTFNF_INVERT) {
        if (alt_format == ctDigital) {
          val = (std::abs(val) < SDL_FLT_EPSILON) ? 1.0f : 0.0f;
        } else if (alt_format == ctAnalog) {
          val = -val;
        }
      }
      break;
    case ctMouseButton:
      packet->flags |= CTPK_MOUSE;
    case ctButton:
      val = get_button_value(controller, alt_format, value);
      break;

    case ctPOV:
      val = get_pov_value(controller, alt_format, 0, value);
      break;
    case ctPOV2:
    case ctPOV3:
    case ctPOV4:
      val = get_pov_value(controller, alt_format, (m_ElementList[id].ctype[i] - ctPOV2) + 1, value);
      break;

    default:
      Int3();
      val = 0.0f;
    }

    if (std::abs(val) > SDL_FLT_EPSILON)
      break;
  }

skip_packet_read:
  if (std::abs(val) > SDL_FLT_EPSILON)
    packet->flags |= CTPK_ELEMENTACTIVE;

  packet->value = val;

  return true;
}

// gets sensitivity of axis item
float sdlgameController::get_axis_sensitivity(ct_type axis_type, uint8_t axis) {
  axis--;
  ASSERT(axis < CT_NUM_AXES);

  switch (axis_type) {
  case ctMouseAxis:
    return m_ControlList[1].sens[axis];

  case ctAxis:
    return m_ControlList[2].sens[axis];

  default:
    Int3();
  }

  return 0.0f;
}

// sets sensitivity of axis item
void sdlgameController::set_axis_sensitivity(ct_type axis_type, uint8_t axis, float val) {
  axis--;
  ASSERT(axis < CT_NUM_AXES);

  switch (axis_type) {
  case ctMouseAxis:
    m_ControlList[1].sens[axis] = val;
    break;
  case ctAxis:
    for (int i = 2; i < m_ControlList.size(); i++)
      m_ControlList[i].sens[axis] = val;
    break;
  default:
    Int3();
  }
}

// assigns an individual function
int sdlgameController::assign_function(ct_function *func) {
  // Given the type of function we want to map, find a fitting t_controller element
  ct_element elem;

  for (int i = 0; i < CTLBINDS_PER_FUNC; i++) {
    elem.ctl_id[i] = NULL_LNXCONTROLLER;

    switch (func->ctype[i]) {
    case ctNone:
      break;
    case ctKey:
      elem.ctl_id[i] = 0; // Keyboard has id 0
      break;
    case ctAnalogTrigger:
    case ctAxis:
      elem.ctl_id[i] = get_axis_controller(func->value[i]);
      break;
    case ctButton:
      elem.ctl_id[i] = get_button_controller(func->value[i]);
      break;
    case ctMouseAxis:
      elem.ctl_id[i] = 1; // Mouse has id 0
      break;
    case ctMouseButton:
      //	find a free mouse button.
      if ((m_ControlList[1].btnmask & (1 << (func->value[i] - 1))) &&
          ((func->value[i] - 1) < m_ControlList[1].info.num_btns)) {
        elem.ctl_id[i] = 1;
      }
      break;
    case ctPOV:
    case ctPOV2:
    case ctPOV3:
    case ctPOV4: {
      int pov_n;
      if (func->ctype[i] == ctPOV)
        pov_n = 0;
      else
        pov_n = (func->ctype[i] - ctPOV2) + 1;

      elem.ctl_id[i] = get_pov_controller(pov_n);
      break;
    }
    }

    elem.ctype[i] = func->ctype[i];
    elem.value[i] = func->value[i];
  }

  elem.format = func->format;
  elem.flags[0] = func->flags[0];
  elem.flags[1] = func->flags[1];
  elem.enabled = true;

  assign_element(func->id, &elem);

  return func->id;
}

int sdlgameController::get_mouse_raw_values(int *x, int *y) {
  if (m_Suspended)
    return 0;

  *x = m_MseState.mx;
  *y = m_MseState.my;

  return m_MseState.btnmask;
}

unsigned int sdlgameController::get_joy_buttons() {
  if (m_Suspended) {
    return 0;
  }

  if (m_ControlList.size() > 2) {
    return m_ExtCtlStates[2].pos.buttons;
  }

  return 0;
}

gameController *CreateController(int num_funcs, ct_function *funcs) { return new sdlgameController(num_funcs, funcs); }

void DestroyController(gameController *ctl) { delete ctl; }

// activates or deactivates mouse and or controller
void sdlgameController::mask_controllers(bool joystick, bool mouse) {
  m_JoyActive = joystick;
  m_MouseActive = mouse;

  if (!m_MouseActive) {
    m_MseState.x = 0;
    m_MseState.y = 0;
    m_MseState.mx = 0;
    m_MseState.my = 0;
    m_MseState.btnmask = 0;
  }

  if (!m_JoyActive) {
    for (size_t ctl = 0; ctl < m_ControlList.size(); ctl++) {
      if (m_ControlList[ctl].type != CTID::JOYSTICK) {
        continue;
      }
      
      uint8_t dev = m_ControlList[ctl].id;
      for (uint8_t axis = 0; axis < m_ControlList[ctl].info.num_axis; axis++) {
        m_ExtCtlStates[dev].pos.axis[axis] = m_ControlList[ctl].normalizer[axis];
      }

      for (uint8_t pov = 0; pov < m_ControlList[ctl].info.num_povs; pov++) {
        m_ExtCtlStates[dev].pos.pov[pov] = JOYPOV_CENTER;
        m_ExtCtlStates[dev].pov_pos[pov].last_pov = JOYPOV_CENTER;

        for (uint8_t i = 0; i < JOYPOV_DIR; i++) {
          m_ExtCtlStates[dev].pov_pos[pov].povstarts[i] = 0.0f;
          m_ExtCtlStates[dev].pov_pos[pov].povtimes[i] = 0.0f;
          m_ExtCtlStates[dev].pov_pos[pov].povpresses[i] = 0;
        }
      }

      m_ExtCtlStates[dev].pos.buttons = 0;
      for (uint8_t i = 0; i < m_ControlList[ctl].info.num_btns; i++) {
        m_ExtCtlStates[dev].btn_pos[i].btnpresses = 0;
        m_ExtCtlStates[dev].btn_pos[i].btntimes = 0.0f;
        m_ExtCtlStates[dev].btn_pos[i].btnstarts = 0.0f;
      }
    }
  }
}

//	---------------------------------------------------------------------------
//	controller functions
void sdlgameController::extctl_getpos(int id) {
  if (!m_JoyActive) {
    return;
  }

  float timer_val = timer_GetTime();

  m_ExtCtlStates[id].pos = joy_GetRawPos((tJoystick_id)id);
  const tJoyPos &pos = m_ExtCtlStates[id].pos;

  for (size_t i = 0; i < pos.pov.size(); i++) {
    m_ExtCtlStates[id].pov_pos[i].last_pov = m_ExtCtlStates[id].pos.pov[i];

    //	when pov changes position and new position is not in center, then set a new start time.
    int pov_index = m_ExtCtlStates[id].pos.pov[i] / (JOYPOV_MAXVAL / JOYPOV_DIR);
    int last_pov_index = m_ExtCtlStates[id].pov_pos[i].last_pov / (JOYPOV_MAXVAL / JOYPOV_DIR);

    if (m_ExtCtlStates[id].pos.pov[i] != m_ExtCtlStates[id].pov_pos[i].last_pov) {
      if (m_ExtCtlStates[id].pos.pov[i] != JOYPOV_CENTER)
        m_ExtCtlStates[id].pov_pos[i].povstarts[pov_index] = timer_val;
      if (m_ExtCtlStates[id].pov_pos[i].last_pov != JOYPOV_CENTER)
        m_ExtCtlStates[id].pov_pos[i].povtimes[last_pov_index] =
            timer_val - m_ExtCtlStates[id].pov_pos[i].povstarts[last_pov_index];
      m_ExtCtlStates[id].pov_pos[i].povpresses[pov_index]++;
    }

    if (m_ExtCtlStates[id].pos.pov[i] != JOYPOV_CENTER) {
      m_ExtCtlStates[id].pov_pos[i].povtimes[pov_index] =
          timer_val - m_ExtCtlStates[id].pov_pos[i].povstarts[pov_index];
    }
  }

  //	handle buttons
  for (int i = 0; i < CT_MAX_BUTTONS; i++) {
    //	case if we read time before doing this again.
    if ((m_ExtCtlStates[id].pos.buttons & (1 << i)) &&
        (std::abs(m_ExtCtlStates[id].btn_pos[i].btnstarts) < SDL_FLT_EPSILON))
      m_ExtCtlStates[id].btn_pos[i].btnstarts = timer_val;
    if ((m_ExtCtlStates[id].pos.buttons & (1 << i)) && !(m_ExtCtlStates[id].pos.buttons & (1 << i))) {
      m_ExtCtlStates[id].btn_pos[i].btnpresses++;
      m_ExtCtlStates[id].btn_pos[i].btnstarts = timer_val;
      //	mprintf(0, "Start time for %d = %f\n", i, timer_val);
    }

    if (m_ExtCtlStates[id].pos.buttons & (1 << i)) // if button is down
      m_ExtCtlStates[id].btn_pos[i].btntimes = timer_val - m_ExtCtlStates[id].btn_pos[i].btnstarts;
    else if (m_ExtCtlStates[id].pos.buttons & (1 << i)) // if button is up and last pass it was down.
      m_ExtCtlStates[id].btn_pos[i].btntimes = timer_val - m_ExtCtlStates[id].btn_pos[i].btnstarts;
  }
}

void sdlgameController::mouse_geteval() {
  int x, y, dx, dy; //,z;
  unsigned btnmask;

  if (!m_MouseActive) {
    return;
  }

  if (std::abs(g_accum_frame_time) > SDL_FLT_EPSILON)
    return;

  btnmask = (unsigned)ddio_MouseGetState(&x, &y, &dx, &dy);

  m_MseState.x = dx;
  m_MseState.y = dy;
  m_MseState.mx = x;
  m_MseState.my = y;

  m_MseState.btnmask = btnmask;
}

//	enumerate all controllers on system
bool sdlgameController::init_controllers() {
  //	Add keyboard controller
  m_ControlList.push_back(t_controller{});
  m_ControlList.back().type = CTID::KEYBOARD;
  m_ControlList.back().id = 0;
  m_ExtCtlStates.emplace_back(t_extctlstate{});

  //	add mouse controller
  int left, top, right, bottom, zmin, zmax; // btns, axes,
  int nbtns, naxis, btnmask;
  ddio_MouseGetLimits(&left, &top, &right, &bottom, &zmin, &zmax);
  btnmask = ddio_MouseGetCaps(&nbtns, &naxis);

  // we will try to support a mouse with 3 axis and N_MSEBTNS buttons.
  m_ControlList.push_back(t_controller{});
  m_ControlList.back().type = CTID::MOUSE;
  m_ControlList.back().id = 1;
  m_ControlList.back().info.num_btns = nbtns;
  m_ControlList.back().info.num_axis = naxis;
  m_ControlList.back().sens.resize(naxis);
  m_ControlList.back().btnmask = btnmask;
  m_ExtCtlStates.emplace_back(t_extctlstate{});

  // normalizer is the "available area" in dots - this is the max we expect ANY mouse to EVER travel in 1.0s
  // if a mouse is faster than (normalizer / frame_time), rot speed will be clamped to 1 rev/sec
  m_ControlList.back().normalizer.resize(naxis, 10000.0f);
  m_ControlList.back().sens.resize(naxis, 1.0f);
  m_ControlList.back().sensmod.resize(naxis, 1.0f);

  for (uint32_t joyId = 0; joyId < joy_GetCount(); joyId++) {
    //	check if device is plugged in.
    if (!joy_IsValid(joyId)) {
      LOG_WARNING << "Ignoring invalid joystick #" << joyId;
      continue;
    }
    tJoyInfo jc = joy_GetJoyInfo((tJoystick_id)joyId);
    m_ControlList.push_back(t_controller{});
    m_ControlList.back().type = CTID::JOYSTICK;
    m_ControlList.back().id = joyId + 2;
    m_ControlList.back().info = jc;
    m_ControlList.back().btnmask = 0;

    int minV = -32768, maxV = 32768;
    m_ControlList.back().normalizer.resize(jc.num_axis, (maxV - minV) / 2.0f);
    m_ControlList.back().sens.resize(jc.num_axis, 1.0f);
    m_ControlList.back().sensmod.resize(jc.num_axis, 1.0f);
    m_ControlList.back().deadzone = JOY_DEADZONE;

    // okay, now search for a "****.ctl" file in the current directory
    this->parse_ctl_file(m_ControlList.size() - 1, jc.name.c_str());

    m_ExtCtlStates.emplace_back(t_extctlstate{});
    m_ExtCtlStates.back().pov_pos.resize(jc.num_povs);
    m_ExtCtlStates.back().btn_pos.resize(jc.num_btns);
    m_ExtCtlStates.back().pos.axis.resize(jc.num_axis);
    m_ExtCtlStates.back().pos.pov.resize(jc.num_povs);
  }

  sdlgameController::flush();

  return true;
}

//	returns the controller with a pov hat
uint8_t sdlgameController::get_pov_controller(uint8_t pov) {
  //	start from controller 2 because 0, and 1 are reserved for keyboard and mouse
  // uint16_t pov_flag = CTF_POV << (pov);

  // TODO: Use mapping
  // for (int i = 2; i < m_ControlList.size(); i++)
  //   if ((m_ControlList[i].flags & pov_flag) && m_ControlList[i].type != CTID::INVALID)
  //     return i;

  return NULL_LNXCONTROLLER;
}

uint8_t sdlgameController::get_button_controller(uint8_t btn) {
  //	buttons range from 1-CT_MAX_BUTTONS
  ASSERT(btn <= CT_MAX_BUTTONS);
  if (btn == NULL_BINDING)
    return NULL_LNXCONTROLLER;

  // TODO: Use gamepad mapping
  // //	start from controller 2 because 0, and 1 are reserved for keyboard and mouse
  // for (int i = 2; i < m_ControlList.size(); i++)
  //   //@@		if (((unsigned)btn < m_ControlList[i].buttons) && !(m_ControlList[i].btnmask & mask) &&
  //   //(m_ControlList[i].id
  //   //!= CTID::INVALID)) {
  //   if (((unsigned)btn < m_ControlList[i].num_buttons) && (m_ControlList[i].type != CTID::INVALID)) {
  //     //@@			m_ControlList[i].btnmask |= mask;
  //     return i;
  //   }

  return NULL_LNXCONTROLLER;
}

uint8_t sdlgameController::get_axis_controller(uint8_t axis) {
  //	start from controller 2 because 0, and 1 are reserved for keyboard and mouse
  if (axis == NULL_BINDING)
    return NULL_LNXCONTROLLER;

  // TODO: Use mapping to find the axis
  // int axis_mask = (1 << (axis - 1));

  // for (int i=0; i < m_ControlList.size(); i++) {
  //   if ((m_ControlList[i+2].flags & axis_mask) && m_ControlList[i+2].type != CTID::INVALID &&
  //       !(m_ControlList[i+2].axis_is_trigger & axis_mask)) {
  //     return i;
  //   }
  return NULL_LNXCONTROLLER;
}

void sdlgameController::assign_element(int id, ct_element *elem) {
  //	assign element, check to see if valid.
  m_ElementList[id].format = elem->format;
  m_ElementList[id].flags[0] = elem->flags[0];
  m_ElementList[id].flags[1] = elem->flags[1];
  m_ElementList[id].enabled = elem->enabled;

  //	look through each controller and validate each element
  for (int i = 0; i < CTLBINDS_PER_FUNC; i++) {
    m_ElementList[id].ctl_id[i] = elem->ctl_id[i];
    m_ElementList[id].value[i] = elem->value[i];
    m_ElementList[id].ctype[i] = elem->ctype[i];

    if (m_ElementList[id].ctl_id[i] != NULL_LNXCONTROLLER) {
      // this function shouldn't do any error checking!!!!  keep same controller values and bindings unless
      // bindings are truly bogus.
      switch (m_ElementList[id].ctype[i]) {
      case ctMouseButton:
      case ctButton:
        if (elem->value[i] > CT_MAX_BUTTONS) {
          m_ElementList[id].ctl_id[i] = NULL_LNXCONTROLLER;
          m_ElementList[id].value[i] = NULL_BINDING;
        }
        break;
      case ctMouseAxis:
      case ctAxis:
      case ctPOV:
      case ctPOV2:
      case ctPOV3:
      case ctPOV4:
      case ctAnalogTrigger:
      case ctKey:
        break;
      default:
        m_ElementList[id].value[i] = NULL_BINDING;
        m_ElementList[id].ctl_id[i] = NULL_LNXCONTROLLER;
      }
    } else {
      m_ElementList[id].value[i] = NULL_BINDING;
    }
  }
}

float sdlgameController::get_button_value(uint8_t controller, ct_format format, uint8_t button) {
  float val = 0.0f;

  if (controller <= NULL_LNXCONTROLLER || static_cast<size_t>(controller) >= m_ControlList.size()) {
    return 0.0f;
  }
  if (m_ControlList[controller].type == CTID::INVALID) {
    return 0.0f;
  }

  if (button == NULL_BINDING) {
    return val;
  }

  //	buttons are idenitifed as 0=none, 1 = button 1, etc.  so if we have a valid button, then
  //	decrement counter.
  button--;

  // verify valid button.
  if ((unsigned)button >= m_ControlList[controller].info.num_btns)
    return val;

  switch (format) {
    //	note we take care of mouse controls and external controls here
  case ctDownCount:
    if (m_ControlList[controller].type == CTID::MOUSE) {
      val = (float)ddio_MouseBtnDownCount(button);
    } else {
      val = (float)m_ExtCtlStates[m_ControlList[controller].id].btn_pos[button].btnpresses;
      m_ExtCtlStates[m_ControlList[controller].id].btn_pos[button].btnpresses = 0;
    }
    break;

  case ctTime:
    if (m_ControlList[controller].type == CTID::MOUSE) {
      val = ddio_MouseBtnDownTime(button);
    } else {
      if (!(m_ExtCtlStates[m_ControlList[controller].id].pos.buttons & (1 << button))) {
        val = m_ExtCtlStates[m_ControlList[controller].id].btn_pos[button].btntimes;
        m_ExtCtlStates[m_ControlList[controller].id].btn_pos[button].btnstarts = 0.0f;
        m_ExtCtlStates[m_ControlList[controller].id].btn_pos[button].btntimes = 0.0f;
      } else {
        val = WinControllerTimer - m_ExtCtlStates[m_ControlList[controller].id].btn_pos[button].btnstarts;
        m_ExtCtlStates[m_ControlList[controller].id].btn_pos[button].btnstarts = WinControllerTimer;
        m_ExtCtlStates[m_ControlList[controller].id].btn_pos[button].btntimes = 0.0f;
      }
    }
    break;

  case ctDigital:
  case ctAnalog:
    if (m_ControlList[controller].type == CTID::MOUSE) {
      if (m_MseState.btnmask & (1 << button))
        val = 1.0f;
    } else if (m_ExtCtlStates[m_ControlList[controller].id].pos.buttons & (1 << button)) {
      val = 1.0f;
    }
    break;

  default:
    LOG_WARNING << "gameController::button unsupported format for function";
  }

  return val;
}

// rot: fraction of one full circle (1.0 == 360 degrees)
static constexpr angle rotationToFixAngle(double rot) {
  return static_cast<angle>(std::numeric_limits<angle>::max() * rot);
}

//	note controller is index into ControlList.
float sdlgameController::get_axis_value(uint8_t controller, uint8_t axis, ct_format format, bool invert) {
  if (controller <= NULL_LNXCONTROLLER || static_cast<size_t>(controller) >= m_ControlList.size()) {
    return 0.0f;
  }

  const t_controller &ctldev = m_ControlList[controller];
  if (ctldev.type == CTID::INVALID) {
    return 0.0f;
  }

  if (m_ControlList[controller].type == CTID::KEYBOARD) {
    LOG_ERROR << "Cannot get axis value from keyboard input";
    Int3();
    return 0.0f;
  }

  //	verify controller axis
  if (ctldev.info.num_axis <= axis) {
    return 0.0f;
  }

  //	get raw value
  float axisval = 0.0f;
  if (ctldev.type == CTID::MOUSE) {
    switch (axis) {
    case CT_X_AXIS:
      axisval = m_MseState.x;
      break;
    case CT_Y_AXIS:
      axisval = m_MseState.y;
      break;
    default:
      LOG_ERROR << "Invalid mouse axis " << axis;
      break;
    }
  } else {
    axisval = static_cast<float>(m_ExtCtlStates[ctldev.id].pos.axis.at(axis - 1));
  }

  // create normalizer
  axis--;
  float nullzone, normalizer;
  if (ctldev.type == CTID::MOUSE) {
    if (m_frame_time < 0.005f)
      m_frame_time = 0.005f; // to trap potential errors.
    normalizer = ctldev.normalizer[axis] * m_frame_time;
    nullzone = MOUSE_DEADZONE;

  } else {
    normalizer = ctldev.normalizer[axis];
    nullzone = (m_ControlList[controller].deadzone < 0.05f) ? 0.05f : m_ControlList[controller].deadzone;
  }

  float val = axisval / normalizer;
  val = val - ((ctldev.type == CTID::MOUSE) ? 0.0f : 1.0f); // joystick needs to be normalized to -1.0 to 1.0

  //	calculate adjusted value
  if (val > nullzone) {
    val = (val - nullzone) / (1.0f - nullzone);
  } else if (val < -nullzone) {
    val = (val + nullzone) / (1.0f - nullzone);
  } else {
    val = 0.0f;
  }
  val = ctldev.sensmod[axis] * ctldev.sens[axis] * val;
  val += 1.0f;

  val = std::clamp(val, 0.0f, 2.0f);

  // determine value based off requested format.
  if (format == ctDigital) {
    if (val < 0.5f)
      val = 0.0f;
    else
      val = 1.0f;
  } else if (format == ctAnalog) {
    val = val - 1.0f;
  } else {
    val = 0.0f;
    LOG_WARNING << "gameController::axis unsupported format for function.";
  }

  ct_packet key_slide1, key_bank;

  get_packet(ctfTOGGLE_SLIDEKEY, &key_slide1);
  get_packet(ctfTOGGLE_BANKKEY, &key_bank);

  if ((Current_pilot.mouselook_control && GAME_MODE == GetFunctionMode()) &&
      // Don't do mouse look if
      !(
          // either toggle is happening
          key_slide1.value || key_bank.value ||
          // mouselook isn't enabled in multiplayer
          (Game_mode & GM_MULTI && !(Netgame.flags & NF_ALLOW_MLOOK)) ||
          // we're in guided missile control
          Players[Player_num].guided_obj)) {
    axis++;

    if ((axis == CT_X_AXIS) && (ctldev.type == CTID::MOUSE) && (std::abs(val) > SDL_FLT_EPSILON)) {
      matrix orient;

      if (!(Players[Player_num].controller_bitflags & PCBF_HEADINGLEFT)) {
        if (val < 0) {
          val = 0.0f;
        }
      }
      if (!(Players[Player_num].controller_bitflags & PCBF_HEADINGRIGHT)) {
        if (val > 0) {
          val = 0.0f;
        }
      }

      if (invert) {
        val = -val;
      }

      vm_AnglesToMatrix(&orient, 0.0, rotationToFixAngle(val * m_frame_time), 0.0);

      Objects[Players[Player_num].objnum].orient = Objects[Players[Player_num].objnum].orient * orient;

      vm_Orthogonalize(&Objects[Players[Player_num].objnum].orient);
      ObjSetOrient(&Objects[Players[Player_num].objnum], &Objects[Players[Player_num].objnum].orient);
      return 0;
    }
    if ((axis == CT_Y_AXIS) && (ctldev.type == CTID::MOUSE) && (std::abs(val) > SDL_FLT_EPSILON)) {
      matrix orient;

      if (!(Players[Player_num].controller_bitflags & PCBF_PITCHUP)) {
        val = std::max(val, 0.0f);
      }
      if (!(Players[Player_num].controller_bitflags & PCBF_PITCHDOWN)) {
        val = std::min(val, 0.0f);
      }

      if (invert) {
        val = -val;
      }

      vm_AnglesToMatrix(&orient, rotationToFixAngle(val * m_frame_time), 0.0, 0.0);

      Objects[Players[Player_num].objnum].orient = Objects[Players[Player_num].objnum].orient * orient;

      vm_Orthogonalize(&Objects[Players[Player_num].objnum].orient);
      ObjSetOrient(&Objects[Players[Player_num].objnum], &Objects[Players[Player_num].objnum].orient);
      return 0;
    }
  }

  return val;
}

float sdlgameController::get_trigger_value(uint8_t controller, uint8_t axis, ct_format format) {
  float normalizer, axisval = 0, nullzone; //, senszone;

  if (controller <= NULL_LNXCONTROLLER || static_cast<size_t>(controller) >= m_ControlList.size()) {
    return 0.0f;
  }

  const t_controller &ctldev = m_ControlList[controller];
  if (ctldev.type == CTID::INVALID) {
    return 0.0f;
  }

  //	verify controller axis
  if (ctldev.info.num_axis <= axis) {
    return 0.0f;
  }

  //	get raw value
  axisval = m_ExtCtlStates[ctldev.id].pos.axis.at(axis);

  // create normalizer
  axis--;
  normalizer = ctldev.normalizer[axis];
  nullzone = (m_ControlList[controller].deadzone < 0.05f) ? 0.05f : m_ControlList[controller].deadzone;

  float val = axisval / normalizer;
  val = val - ((ctldev.type == CTID::MOUSE) ? 0.0f : 1.0f); // joystick needs to be normalized to -1.0 to 1.0

  //	calculate adjusted value
  if (val > nullzone) {
    val = (val - nullzone) / (1.0f - nullzone);
  } else if (val < -nullzone) {
    val = (val + nullzone) / (1.0f - nullzone);
  } else {
    val = 0.0f;
  }
  val = ctldev.sensmod[axis] * ctldev.sens[axis] * val;
  val = val + 1.0f;

  val = std::clamp(val, 0.0f, 2.0f);

  // determine value based off requested format.
  if (format == ctDigital) {
    if (val < 0.5f)
      val = 0.0f;
    else
      val = 1.0f;
  } else if (format == ctAnalog) {
    val = val - 1.0f;
  } else {
    val = 0.0f;
    LOG_WARNING << "unsupported format for function sdlgameController::get_trigger_value.";
  }

  return val;
}
//	do some pov stuff
float sdlgameController::get_pov_value(uint8_t controller, ct_format format, uint8_t pov_number, uint8_t pov) {
  float val = 0.0f;

  if (controller <= NULL_LNXCONTROLLER || static_cast<size_t>(controller) >= m_ControlList.size()) {
    return val;
  }
  if (m_ControlList[controller].type == CTID::INVALID) {
    return val;
  }
  if (m_ControlList[controller].type == CTID::KEYBOARD) {
    Int3();
    return 0.0f;
  }
  if (m_ControlList[controller].info.num_povs <= pov_number) {
    return val;
  }

  int pov_index = pov / (JOYPOV_MAXVAL / JOYPOV_DIR);
  int cur_pov_index = m_ExtCtlStates[m_ControlList[controller].id].pos.pov[pov_number] / (JOYPOV_MAXVAL / JOYPOV_DIR);

  switch (format) {
    //	note we take care of mouse controls and external controls here
  case ctDownCount:
    if (pov_index == JOYPOV_DIR)
      val = 0.0f;
    else {
      val = m_ExtCtlStates[m_ControlList[controller].id].pov_pos[pov_number].povpresses[pov_index];
      m_ExtCtlStates[m_ControlList[controller].id].pov_pos[pov_number].povpresses[pov_index] = 0;
    }
    break;

  case ctAnalog:
  case ctDigital: {
    if (m_ExtCtlStates[m_ControlList[controller].id].pos.pov[pov_number] == JOYPOV_CENTER)
      val = 0.0f;
    else if ((cur_pov_index == 0 || cur_pov_index == 1 || cur_pov_index == 7) && (pov == JOYPOV_UP))
      val = 1.0f;
    else if ((cur_pov_index == 1 || cur_pov_index == 2 || cur_pov_index == 3) && (pov == JOYPOV_RIGHT))
      val = 1.0f;
    else if ((cur_pov_index == 3 || cur_pov_index == 4 || cur_pov_index == 5) && (pov == JOYPOV_DOWN))
      val = 1.0f;
    else if ((cur_pov_index == 5 || cur_pov_index == 6 || cur_pov_index == 7) && (pov == JOYPOV_LEFT))
      val = 1.0f;
    break;
  }

  case ctTime:
    if (cur_pov_index == pov_index) {
      val = WinControllerTimer - m_ExtCtlStates[m_ControlList[controller].id].pov_pos[pov_number].povstarts[pov_index];
      m_ExtCtlStates[m_ControlList[controller].id].pov_pos[pov_number].povstarts[pov_index] = WinControllerTimer;
      m_ExtCtlStates[m_ControlList[controller].id].pov_pos[pov_number].povtimes[pov_index] = 0.0f;
    } else {
      val = m_ExtCtlStates[m_ControlList[controller].id].pov_pos[pov_number].povtimes[pov_index];
      m_ExtCtlStates[m_ControlList[controller].id].pov_pos[pov_number].povstarts[pov_index] = 0.0f;
      m_ExtCtlStates[m_ControlList[controller].id].pov_pos[pov_number].povtimes[pov_index] = 0.0f;
    }
    break;

  default:
    LOG_WARNING << "gameController::pov unsupported format for function";
  }

  return val;
}

//	get keyboard info
float sdlgameController::get_key_value(int key, ct_format format) {
  float val = 0.0f;

  ASSERT(key < DDIO_MAX_KEYS);

  switch (format) {
    //	note we take care of mouse controls and external controls here
  case ctDigital:
    if (KEY_STATE(key))
      val = 1.0f;
    break;

  case ctDownCount:
    val = (float)ddio_KeyDownCount(key);
    break;

  case ctTime:
    val = ddio_KeyDownTime(key);
    break;

  default:
    LOG_WARNING << "gameController::key unsupported format for function";
  }

  return val;
}

//	CTL file parser
enum CTLCMD { NAME = 0, AXIS = 1, SX = 3, DEAD = 2, SZ = 5, SY = 4, SU = 7, SR = 6, SV = 8, N_CTLCMDS = 9 };

const char *CTLCommands[N_CTLCMDS] = {"name",     "axis",     "deadzone", "sensmodx", "sensmody",
                                      "sensmodz", "sensmodr", "sensmodu", "sensmodv"};

int CTLLex(const char *command) {
  for (int i = 0; i < N_CTLCMDS; i++) {
    if (strcmp(CTLCommands[i], command) == 0)
      return i;
  }

  return INFFILE_ERROR;
}

// okay, now search for a '****.ctl' file in the Base_directories
void sdlgameController::parse_ctl_file(int devnum, const char *ctlname) {
  for (auto base_directories_iterator = Base_directories.rbegin(); base_directories_iterator != Base_directories.rend();
       ++base_directories_iterator) {
    // parse each file until we find a name match, no name match, just return
    ddio_DoForeachFile(
        *base_directories_iterator, std::regex(".*\\.ctl"),
        [this, &devnum, &ctlname](const std::filesystem::path &path) {
          InfFile file;
          bool found_name = false;

          if (file.Open(path.filename(), "[controller settings]", CTLLex)) {
            // parse each line, setting the appropriate values, etc.
            while (file.ReadLine()) {
              int cmd;
              char operand[128];

              while ((cmd = file.ParseLine(operand, INFFILE_LINELEN)) > INFFILE_ERROR) {
                // we want to assert that the name command comes before any other to verify
                // this is the file we really want to change.
                switch (cmd) {
                case CTLCMD::NAME:
                  if (strcmp(ctlname, operand) != 0)
                    goto cancel_file_parse;
                  found_name = true;
                  break;

                case CTLCMD::DEAD: // deadzone
                  if (!found_name)
                    goto cancel_file_parse;
                  else {
                    m_ControlList[devnum].deadzone = atof(operand);
                  }
                  break;

                case CTLCMD::AXIS: // allowable axis.
                                   // format of command is "+Z-R"
                                   //	this would add a Z axis to the controller.  -R would remove the Rudder.
                                   // you can do this for X,Y,Z,R,U,V.
                  if (!found_name)
                    goto cancel_file_parse;
                  else {
                    int slen = strlen(operand);
                    for (int i = 0; i <= slen; i += 2) {
                      m_ControlList[devnum].info.num_axis = 0;
                      if ((i + 1) <= slen) {
                        if (operand[i] == '+') {
                          m_ControlList[devnum].info.num_axis += 1;
                        } else if (operand[i] == '-') {
                          m_ControlList[devnum].info.num_axis -= 1;
                        } else {
                          goto cancel_file_parse;
                        }
                      } else {
                        break; // this should break out of the axis search but continue with the file
                      }
                    }
                  }
                  break;

                case CTLCMD::SX: // allow modification of global sensitivity modifiers
                case CTLCMD::SY:
                case CTLCMD::SZ:
                case CTLCMD::SR:
                case CTLCMD::SU:
                case CTLCMD::SV: {
                  int idx = (cmd - CTLCMD::SX);
                  m_ControlList[devnum].sensmod[idx] = atof(operand);
                  break;
                }
                }
              }
            }
          cancel_file_parse:
            file.Close();
          }
        });
  }
}
