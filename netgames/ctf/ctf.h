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

#ifndef __DMFC_APP_H_
#define __DMFC_APP_H_

#include <stddef.h>

#include "osiris_share.h"
#include "d3events.h"
#include "gamedll_header.h"
#include "idmfc.h"
#include "module.h"
#include "multi_external.h"
#include "object_external_struct.h"
#include "pstypes.h"

//	If a player dies, becomes an observer or disconnects, it's possible that they
//	have a flag or more.  This function will handle setting certain variables, timer
//	and other various settings related to a flag spew
void HandlePlayerSpew(int pnum);
//	AddFlagToPlayer
//	gives a player a flag
bool AddFlagToPlayer(int pnum, int team, int flagobjnum = -1);
//	RemoveFlagFromPlayer
//	removes a flag from a player
bool RemoveFlagFromPlayer(int pnum, int team);

void OnInterval(void);
void OnHUDInterval(void);
void OnKeypress(int key);
void OnServerGameCreated(void);
void OnClientLevelStart(void);
void OnClientLevelEnd(void);
void OnGameStateRequest(int player_num);
void OnPlayerConnect(int player_num);
void OnClientPlayerKilled(object *killer_obj, int victim_pnum);
void OnServerCollide(object *me_obj, object *it_obj);
void OnClientCollide(ubyte *data);
void OnPLRInterval(void);
void OnPLRInit(void);
bool OnCanChangeTeam(int pnum, int newteam);
void OnSaveStatsToFile(void);
void OnLevelEndSaveStatsToFile(void);
void OnDisconnectSaveStatsToFile(void);
void OnPrintScores(int level);
void OnPlayerEntersObserver(int pnum, object *piggy);
void OnClientPlayerDisconnect(int player_num);
void OnPlayerChangeTeam(int player_num, int newteam, bool announce, bool spew_onrespawn);

extern IDMFC *DMFCBase;

// These next two function prototypes MUST appear in the extern "C" block if called
// from a CPP file.
#ifdef __cplusplus
extern "C" {
#endif
DLLEXPORT void DLLFUNCCALL DLLGameInit(int *api_func, ubyte *all_ok, int num_teams_to_use);
DLLEXPORT void DLLFUNCCALL DLLGameCall(int eventnum, dllinfo *data);
DLLEXPORT void DLLFUNCCALL DLLGameClose();
DLLEXPORT void DLLFUNCCALL DLLGetGameInfo(tDLLOptions *options);
DLLEXPORT int DLLFUNCCALL GetGOScriptID(const char *name, ubyte isdoor);
DLLEXPORT void DLLFUNCCALLPTR CreateInstance(int id);
DLLEXPORT void DLLFUNCCALL DestroyInstance(int id, void *ptr);
DLLEXPORT short DLLFUNCCALL CallInstanceEvent(int id, void *ptr, int event, tOSIRISEventInfo *data);
DLLEXPORT int DLLFUNCCALL SaveRestoreState(void *file_ptr, ubyte saving_state);
#ifdef __cplusplus
}
#endif


// The main entry point where the game calls the dll
void DLLFUNCCALL DLLGameCall(int eventnum, dllinfo *data) {
  if ((eventnum < EVT_CLIENT_INTERVAL) && (DMFCBase->GetLocalRole() != LR_SERVER)) {
    return;
  }

  DMFCBase->TranslateEvent(eventnum, data);
}

//	GetGOScriptID
//	Purpose:
//		Given the name of the object (from it's pagename), this function will search through it's
//	list of General Object Scripts for a script with a matching name (to see if there is a script
//	for that type/id of object within this DLL).  If a matching scriptname is found, a UNIQUE ID
//	is to be returned back to Descent 3.  This ID will be used from here on out for all future
//	interaction with the DLL.  Since doors are not part of the generic object's, it's possible
//	for a door to have the same name as a generic object (OBJ_POWERUP, OBJ_BUILDING, OBJ_CLUTTER
//	or OBJ_ROBOT), therefore, a 1 is passed in for isdoor if the given object name refers to a
//	door, else it is a 0.  The return value is the unique identifier, else -1 if the script
//	does not exist in the DLL.
int DLLFUNCCALL GetGOScriptID(const char *name, ubyte isdoor) { return -1; }

//	CreateInstance
//	Purpose:
//		Given an ID from a call to GetGOScriptID(), this function will create a new instance for that
//	particular script (by allocating and initializing memory, etc.).  A pointer to this instance
//	is to be returned back to Descent 3.  This pointer will be passed around, along with the ID
//	for CallInstanceEvent() and DestroyInstance().  Return NULL if there was an error.
void DLLFUNCCALLPTR CreateInstance(int id) { return NULL; }

//	DestroyInstance
//	Purpose:
//		Given an ID, and a pointer to a particular instance of a script, this function will delete and
//	destruct all information associated with that script, so it will no longer exist.
void DLLFUNCCALL DestroyInstance(int id, void *ptr) {}

//	CallInstanceEvent
//	Purpose:
//		Given an ID, a pointer to a script instance, an event and a pointer to the struct of
//	information about the event, this function will translate who this event belongs to and
//	passes the event to that instance of the script to be handled.  Return a combination of
//	CONTINUE_CHAIN and CONTINUE_DEFAULT, to give instructions on what to do based on the
//	event. CONTINUE_CHAIN means to continue through the chain of scripts (custom script, level
//	script, mission script, and finally default script).  If CONTINUE_CHAIN is not specified,
//	than the chain is broken and those scripts of lower priority will never get the event.  Return
//	CONTINUE_DEFAULT in order to tell D3 if you want process the normal action that is built into
//	the game for that event.  This only pertains to certain events.  If the chain continues
//	after this script, than the CONTINUE_DEFAULT setting will be overridden by lower priority
//	scripts return value.
short DLLFUNCCALL CallInstanceEvent(int id, void *ptr, int event, tOSIRISEventInfo *data) {
  return CONTINUE_CHAIN | CONTINUE_DEFAULT;
}

//	SaveRestoreState
//	Purpose:
//		This function is called when Descent 3 is saving or restoring the game state.  In this function
//	you should save/restore any global data that you want preserved through load/save (which includes
//	demos).  You must be very careful with this function, corrupting the file (reading or writing too
//	much or too little) may be hazardous to the game (possibly making it impossible to restore the
//	state).  It would be best to use version information to keep older versions of saved states still
//	able to be used.  IT IS VERY IMPORTANT WHEN SAVING THE STATE TO RETURN THE NUMBER OF _BYTES_ WROTE
//	TO THE FILE.  When restoring the data, the return value is ignored.  saving_state is 1 when you should
//	write data to the file_ptr, 0 when you should read in the data.
int DLLFUNCCALL SaveRestoreState(void *file_ptr, ubyte saving_state) { return 0; }


#endif
