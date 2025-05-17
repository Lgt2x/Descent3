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
 */

#ifndef __LNXCONTROLLER_H_
#define __LNXCONTROLLER_H_

#include "controller.h"
#include "controls.h"
#include "joystick.h"
#include <cstdint>

#define NULL_LNXCONTROLLER ((int8_t)NULL_CONTROLLER)

//	rules for adding controllers
//		any nonstandard special controllers should be added to the below list
enum CTID { INVALID = -3, MOUSE = -2, KEYBOARD = -1, JOYSTICK = 0 };

class sdlgameController final : public gameController {
public:
  sdlgameController(int num_funcs, ct_function *funcs);

  //	these functions suspend or resume any controller reading.  this is really only useful for
  //	preemptive controller polling, but they should be used to activate and deactivate controller
  //	reading.
  void suspend() override;
  void resume() override;

  //	this functions polls the controllers if needed.  some systems may not need to implement
  //	this function.
  void poll() override;

  //	flushes all controller information
  void flush() override;

  //	returns the value of a requested controller type. make sure you flush the controller before polling.
  ct_config_data get_controller_value(ct_type type_req) override;

  // fill `val` with the axis and controller values if axis value is over a threshold. Used for control mapping.
  void get_controller_axis_value(int controllerId, unsigned int axis_ctf_flag, uint8_t axis_ct_flag,
                                 ct_config_data *val);

  // fill `val` with the axis and controller values if trigger value is over a threshold
  void get_controller_trigger_value(int controllerId, unsigned int axis_ctf_flag, uint8_t axis_ct_flag,
                                    ct_config_data *val);

  //	sets the configuration of a function (type must be of an array == CTLBINDS_PER_FUNC)
  void set_controller_function(int id, const ct_type *type, ct_config_data value, const uint8_t *flags) override;

  //	returns information about a requested function (type must be of an array == CTLBINDS_PER_FUNC)
  void get_controller_function(int id, ct_type *type, ct_config_data *value, uint8_t *flags) override;

  //	temporarily enables or disables a function
  void enable_function(int id, bool enable) override;

  //	all systems need to implement this function.  this returns information about the controller
  bool get_packet(int id, ct_packet *packet, ct_format alt_format = ctNoFormat) override;

  // gets sensitivity of axis item
  float get_axis_sensitivity(ct_type axis_type, uint8_t axis) override;

  // sets sensitivity of axis item
  void set_axis_sensitivity(ct_type axis_type, uint8_t axis, float val) override;

  // assigns an individual function
  int assign_function(ct_function *fn) override;

  // activates or deactivates mouse and or controller
  void mask_controllers(bool joystick, bool mouse) override;

  // get raw values for the controllers
  int get_mouse_raw_values(int *x, int *y) override;
  unsigned get_joy_raw_values(int *x, int *y) override;

  // retrieves binding text for desired function, binding, etc.
  const char *get_binding_text(ct_type type, uint8_t ctrl, uint8_t bind) override;

  // toggles use of deadzone for controllers
  void set_controller_deadzone(int ctl, float deadzone) override;

private:
  struct t_controller {
    int id = CTID::INVALID;
    uint16_t buttons = 0;
    uint32_t btnmask = 0;
    float normalizer[CT_NUM_AXES]{};
    float sens[CT_NUM_AXES]{};
    float sensmod[CT_NUM_AXES]{};
    float deadzone = 0;
  };

  struct ct_element {
    ct_format format{};
    int8_t ctl[CTLBINDS_PER_FUNC]{};
    uint8_t value[CTLBINDS_PER_FUNC]{};
    ct_type ctype[CTLBINDS_PER_FUNC]{};
    uint8_t flags[2]{};
    bool enabled = false;
  };

  static const int CT_MAX_BUTTONS = 32;
  struct t_extctlstate {
    int x = 0, y = 0, z = 0, r = 0, u = 0, v = 0;
    int pov[JOYPOV_NUM]{};
    int last_pov[JOYPOV_NUM]{};
    float povstarts[JOYPOV_NUM][JOYPOV_DIR]{};
    float povtimes[JOYPOV_NUM][JOYPOV_DIR]{};
    uint8_t povpresses[JOYPOV_NUM][JOYPOV_DIR]{};
    uint32_t buttons = 0;
    uint8_t btnpresses[CT_MAX_BUTTONS]{};
    float btnstarts[CT_MAX_BUTTONS]{};
    float btntimes[CT_MAX_BUTTONS]{};
  };

  struct t_msestate {
    int x = 0, y = 0;
    int mx = 0, my = 0;
    uint32_t btnmask = 0;
  } m_MseState;

  bool init_controllers();

  //	sets up an elements information structure
  void assign_element(int id, ct_element *elem);

  //	this returns an index into the control list.
  int8_t get_axis_controller(uint8_t axis);

  //	returns controller with specified button
  int8_t get_button_controller(uint8_t btn);

  //	returns the controller with a pov hat
  int8_t get_pov_controller(uint8_t pov);

  //	note controller is index into ControlList.
  float get_axis_value(int8_t controller, uint8_t axis, ct_format format, bool invert = false);

  // get value of analog button/trigger
  float get_trigger_value(int8_t controller, uint8_t axis, ct_format format);

  //	get value of button in  seconds, presses, etc.
  float get_button_value(int8_t controller, ct_format format, uint8_t button);

  //	get value of pov (using JOYPOV values)
  float get_pov_value(int8_t controller, ct_format format, uint8_t pov_number, uint8_t pov);

  //	get keyboard info
  float get_key_value(int key, ct_format format);

  void parse_ctl_file(int devnum, const char *ctlname);

  int m_Suspended = 0;                             // is controller polling suspended?
  bool m_JoyActive = false, m_MouseActive = false; // enables or disables mouse, joystick control

  std::vector<t_controller> m_ControlList; // Available controllers and their config. The first 2 are respectively keyboard and mouse
  std::vector<t_extctlstate> m_ExtCtlStates; // State
  std::array<ct_element, NUM_CONTROLLER_FUNCTIONS> m_ElementList; // Bindings of gameplay

  //	thread info.
  int64_t m_frame_timer_ms = 0;
  float m_frame_time = 0;

  //	note id is id value from controller in control list.
  void extctl_getpos(int id);

  //	this gets timings for mouse buttons
  void mouse_geteval();
};

#endif
