/*
	Copyright (c) 2008 Robin Vobruba <hoijui.quaero@gmail.com>

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "BrAIn.h"



brainSpace::BrAIn::BrAIn(springai::AICallback* callback):
		callback(callback),
		teamId(callback != NULL ? callback->GetTeamId() : -1)
		{
			decision = new Decision(callback);
			UtilityInstance = Utility::GetInstance( callback );
		}

brainSpace::BrAIn::~BrAIn() {}

int brainSpace::BrAIn::HandleEvent(int topic, const void* data) {

	switch (topic) {
		case EVENT_INIT:
			//do i really need to do anything here? (i allready have a teamId and a callback)
			break;
		case EVENT_RELEASE:
			//i am no longer needed
			delete(decision);
			break;
		case EVENT_UPDATE:
		{
			struct SUpdateEvent* evt = (struct SUpdateEvent*) data;
			int frame = evt->frame;
			decision->Update(frame);
			break;
		}
		case EVENT_MESSAGE:
		{
			struct SMessageEvent* evt = (struct SMessageEvent*) data;
			//why are you talking to me? im a bot!
			break;
		}
		case EVENT_UNIT_CREATED:
		{
			struct SUnitCreatedEvent* evt = (struct SUnitCreatedEvent*) data;
			int unitId = evt->unit;
			
			// TODO: wrapp events and commands too
			
			std::string unitDefName = callback->GetFriendlyUnits()[0]->GetDef()->GetName();
			
			std::string msgText = "Hello Engine (from CppTestA), first friendly untis def name is: " + unitDefName;
			SSendTextMessageCommand cmd;
			cmd.text = msgText.c_str();
			cmd.zone = 0;
			int ret = callback->GetEngine()->HandleCommand(0, -1, COMMAND_SEND_TEXT_MESSAGE, &cmd);
			

			break;
		}
		case EVENT_UNIT_FINISHED:
		{
			struct SUnitFinishedEvent* evt = (struct SUnitFinishedEvent*) data;
			int unitId = evt->unit;
			decision->UnitFinished(unitId);
			break;
		}
		case EVENT_UNIT_IDLE:
			//get back to work ya lazy git!
			break;
		case EVENT_UNIT_MOVE_FAILED:
			break;
		case EVENT_UNIT_DAMAGED:
			break;
		case EVENT_UNIT_DESTROYED:
		{
			struct SUnitDestroyedEvent* evt = (struct SUnitDestroyedEvent*) data;
			int unitId = evt->unit;
			int attackerId = evt->attacker;
			decision->UnitDestroyed(unitId, attackerId);
			break;
		}
		case EVENT_UNIT_GIVEN:
			break;
		case EVENT_UNIT_CAPTURED:
			break;
		case EVENT_ENEMY_ENTER_LOS:
		{
			//there he is, get him!
			struct SEnemyEnterLOSEvent* evt = (struct SEnemyEnterLOSEvent*)data;
			decision->EnemyEnterLOS(evt->enemy);
			break;
		}
		case EVENT_ENEMY_LEAVE_LOS:
			//Where did he go?
			break;
		case EVENT_ENEMY_ENTER_RADAR:
			break;
		case EVENT_ENEMY_LEAVE_RADAR:
			break;
		case EVENT_ENEMY_DAMAGED:
			break;
		case EVENT_ENEMY_DESTROYED:
		{
			//gotcha bitch!
			struct SEnemyDestroyedEvent* evt = (struct SEnemyDestroyedEvent*)data;
			decision->EnemyDestroyed(evt->enemy, evt->attacker);
			break;
		}
		case EVENT_WEAPON_FIRED:
			break;
		case EVENT_PLAYER_COMMAND:
			//are you telling ME what to do?
			break;
		case EVENT_SEISMIC_PING:
			//WTH was that?
			break;
		case EVENT_COMMAND_FINISHED:
			break;
		case EVENT_LOAD:
			break;
		case EVENT_SAVE:
			break;
		default: {
			UtilityInstance->ChatMsg("recived an unhandled event with topic: %d",topic); 
			break;
		}
	}

	// signal: everything went OK
	return 0;
}