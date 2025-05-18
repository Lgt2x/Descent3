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

 * $Logfile: /DescentIII/Main/Lib/joystick.h $
 * $Revision: 11 $
 * $Date: 10/21/99 9:27p $
 * $Author: Jeff $
 *
 *	Joystick interface.
 *
 * $Log: /DescentIII/Main/Lib/joystick.h $
 *
 * 11    10/21/99 9:27p Jeff
 * B.A. Macintosh code merge
 *
 * 10    7/28/99 3:19p Kevin
 * Mac Stuff
 *
 * 9     7/26/99 11:59a Samir
 * add code to get name of joystick
 *
 * 8     7/16/99 11:14a Samir
 * multiple hat support and improved direct input support.
 *
 * 7     4/09/99 12:02p Samir
 * joystick changes (Win32 DirectInput support)
 *
 * 6     6/18/98 4:49p Samir
 * modified for better mouse support?
 *
 * 5     6/02/98 4:37p Samir
 * multiple joysticks supported.
 *
 * 4     6/01/98 4:27p Samir
 * pov may return multiple positions.
 *
 * 3     12/05/97 12:49p Samir
 * New POV constants.
 *
 * 2     12/03/97 7:33p Samir
 * Newer joystick library.
 *
 * 1     11/24/97 3:27p Samir
 * Initial revision
 *
 * $NoKeywords: $
 */

#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <cstdint>
#include <string>
#include <vector>

#define JOYPOV_NUM 16
#define JOYAXIS_NUM 16
#define JOYBUTTON_NUM 32

typedef int32_t tJoystick_id;

#define JOYPOV_DIR 8
#define JOYPOV_MAXVAL 0x100
#define JOYPOV_UP 0
#define JOYPOV_RIGHT 0x40
#define JOYPOV_DOWN 0x80
#define JOYPOV_LEFT 0xc0
#define JOYPOV_CENTER 0xff

struct tJoyInfo {
  std::string name;
  uint8_t num_axis = 0;
  uint8_t num_btns = 0;
  uint8_t num_povs = 0;
};

struct tJoyPos {
  uint64_t buttons;
  std::vector<int32_t> axis;
  std::vector<uint8_t> pov;
};

// joystick system initialization
// return true if joysticks have been initialized properly
bool joy_Init();
void joy_Close();

// retreive information about joystick.
tJoyInfo joy_GetJoyInfo(tJoystick_id joy);

// retreive position of joystick
tJoyPos joy_GetPos(tJoystick_id joy);

// retreive uncalibrated position of joystick
tJoyPos joy_GetRawPos(tJoystick_id joy);

// returns true if joystick is valid
bool joy_IsValid(tJoystick_id joy);

bool joy_AxisIsTrigger(tJoystick_id joy, uint8_t axisId);

// return the joystick count
uint32_t joy_GetCount();

// DAJ Added to support InSprocket
#endif
