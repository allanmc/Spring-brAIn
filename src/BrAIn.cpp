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
#include "Utility.h"
using namespace brainSpace;


brainSpace::BrAIn::BrAIn(springai::AICallback* callback):
		callback(callback),
		teamId(callback != NULL ? callback->GetTeamId() : -1)
		{
			Utility::GetInstance()->ChatMsg("Hello world i am team: %d",teamId);
			decision = new Decision(callback);
			UtilityInstance = Utility::GetInstance( callback );
		}

brainSpace::BrAIn::~BrAIn() {}

int brainSpace::BrAIn::HandleEvent(int topic, const void* data) {
	static char c[200];
	SNPRINTF(c, 200, "recived an event with topic: %i", topic);
	SSendTextMessageCommand cmd;
	cmd.text = c;
	cmd.zone = 0;
	callback->GetEngine()->HandleCommand(0, -1, COMMAND_SEND_TEXT_MESSAGE, &cmd);

	if(topic > 50) Utility::GetInstance()->ChatMsg("wierd crap");
	switch (topic) {
		case EVENT_INIT:
			Utility::GetInstance()->ChatMsg("init");
			//do i really need to do anything here? (i allready have a teamId and a callback)
			break;
		case EVENT_RELEASE:
			Utility::GetInstance()->ChatMsg("release");
			//i am no longer needed
			delete(decision);
			break;
		case EVENT_UPDATE:
		{
			Utility::GetInstance()->ChatMsg("update");
			struct SUpdateEvent* evt = (struct SUpdateEvent*) data;
			int frame = evt->frame;
			decision->Update(frame);
			break;
		}
		case EVENT_MESSAGE:
		{
			Utility::GetInstance()->ChatMsg("message");
			struct SMessageEvent* evt = (struct SMessageEvent*) data;
			//why are you talking to me? im a bot!
			break;
		}
		case EVENT_UNIT_CREATED:
		{
			Utility::GetInstance()->ChatMsg("unit created");
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
			Utility::GetInstance()->ChatMsg("Unit finished");
			struct SUnitFinishedEvent* evt = (struct SUnitFinishedEvent*) data;
			int unitId = evt->unit;
			decision->UnitFinished(unitId);
			break;
		}
		case EVENT_UNIT_IDLE:
			Utility::GetInstance()->ChatMsg("unit idle");
			//get back to work ya lazy git!
			break;
		case EVENT_UNIT_MOVE_FAILED:
			Utility::GetInstance()->ChatMsg("move failed");
			break;
		case EVENT_UNIT_DAMAGED:
			Utility::GetInstance()->ChatMsg("unit damaged");
			break;
		case EVENT_UNIT_DESTROYED:
		{
			Utility::GetInstance()->ChatMsg("unit destroyed");
			struct SUnitDestroyedEvent* evt = (struct SUnitDestroyedEvent*) data;
			int unitId = evt->unit;
			int attackerId = evt->attacker;
			decision->UnitDestroyed(unitId, attackerId);
			break;
		}
		case EVENT_UNIT_GIVEN:
			Utility::GetInstance()->ChatMsg("unit given");
			break;
		case EVENT_UNIT_CAPTURED:
			Utility::GetInstance()->ChatMsg("unit captured");
			break;
		case EVENT_ENEMY_ENTER_LOS:
		{
			Utility::GetInstance()->ChatMsg("enemy enter los");
			//there he is, get him!
			struct SEnemyEnterLOSEvent* evt = (struct SEnemyEnterLOSEvent*)data;
			decision->EnemyEnterLOS(evt->enemy);
			break;
		}
		case EVENT_ENEMY_LEAVE_LOS:
			Utility::GetInstance()->ChatMsg("enemy leave los");
			//Where did he go?
			break;
		case EVENT_ENEMY_ENTER_RADAR:
			Utility::GetInstance()->ChatMsg("enemy enter radar");
			break;
		case EVENT_ENEMY_LEAVE_RADAR:
			Utility::GetInstance()->ChatMsg("enemy leave radar");
			break;
		case EVENT_ENEMY_DAMAGED:
			Utility::GetInstance()->ChatMsg("enemy damaged");
			break;
		case EVENT_ENEMY_DESTROYED:
		{
			Utility::GetInstance()->ChatMsg("enemy destroyed");
			//gotcha bitch!
			struct SEnemyDestroyedEvent* evt = (struct SEnemyDestroyedEvent*)data;
			decision->EnemyDestroyed(evt->enemy, evt->attacker);
			break;
		}
		case EVENT_WEAPON_FIRED:
			Utility::GetInstance()->ChatMsg("weapon fired");
			break;
		case EVENT_PLAYER_COMMAND:
			Utility::GetInstance()->ChatMsg("player command");
			//are you telling ME what to do?
			break;
		case EVENT_SEISMIC_PING:
			Utility::GetInstance()->ChatMsg("ping");
			//WTH was that?
			break;
		case EVENT_COMMAND_FINISHED:
			Utility::GetInstance()->ChatMsg("command finished");
			break;
		case EVENT_LOAD:
			Utility::GetInstance()->ChatMsg("load");
			break;
		case EVENT_SAVE:
			Utility::GetInstance()->ChatMsg("save");
			break;
		default: {
			UtilityInstance->ChatMsg("recived an unhandled event with topic: %d",topic); 
			break;
		}
	}
	Utility::GetInstance()->ChatMsg("Im done switching..."); 
	// signal: everything went OK
	return 0;
}