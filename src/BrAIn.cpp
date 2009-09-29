#include "BrAIn.h"

using namespace brainSpace;


brainSpace::BrAIn::BrAIn(springai::AICallback* cb)
{
	ai = new AIClasses();
	ai->callback = cb;
	ai->utility = new Utility(ai);
	ai->knowledge = new Knowledge(ai);

	teamId = (ai->callback != NULL ? ai->callback->GetTeamId() : -1);

	decision = new Decision(ai);
	//ai->utility->ChatMsg("Hello world i am team: %d",teamId);
	srand(0);
}
brainSpace::BrAIn::~BrAIn() {}

int brainSpace::BrAIn::HandleEvent(int topic, const void* data) {

	//ai->utility->ChatMsg("Recived an event with topic: %i", topic);

	//if(topic > 50) ai->utility->ChatMsg("wierd crap");
	switch (topic) {
		case EVENT_INIT:
			ai->utility->ChatMsg("THE BEGINNING!");
			ai->utility->Log(ALL, CHAT|DECISION, "Trying something...");
			ai->utility->Log(ALL, CHAT|DECISION, "Congrats...");
			//do i really need to do anything here? (i allready have a teamId and a callback)
			break;
		case EVENT_RELEASE:
			//ai->utility->ChatMsg("THE END!");
			//i am no longer needed
			//delete(decision);
			break;
		case EVENT_UPDATE:
			{
				//UtilityInstance->ChatMsg("update");
				struct SUpdateEvent* evt = (struct SUpdateEvent*) data;
				int frame = evt->frame;
				decision->Update(frame);
				break;
			}
		case EVENT_MESSAGE:
			{
				//UtilityInstance->ChatMsg("message");
				struct SMessageEvent* evt = (struct SMessageEvent*) data;
				//why are you talking to me? im a bot!
				break;
			}
		case EVENT_UNIT_CREATED:
			{
				//UtilityInstance->ChatMsg("unit created");
				struct SUnitCreatedEvent* evt = (struct SUnitCreatedEvent*) data;
				int unitId = evt->unit;

				// TODO: wrapp events and commands too

				std::string unitDefName = ai->callback->GetFriendlyUnits()[0]->GetDef()->GetName();

				//ai->utility->ChatMsg("Hello Engine (from brAIn), first friendly untis def name is: " + unitDefName);

				break;
			}
		case EVENT_UNIT_FINISHED:
			{
				//ai->utility->ChatMsg("Unit finished");
				struct SUnitFinishedEvent* evt = (struct SUnitFinishedEvent*) data;
				int unitId = evt->unit;
				decision->UnitFinished(unitId);
				break;
			}
		case EVENT_UNIT_IDLE:
			{
				//ai->utility->ChatMsg("UNIT IDLE");
				struct SUnitIdleEvent* evt = (struct SUnitIdleEvent*)data;
				decision->UnitIdle( evt->unit );
				//get back to work ya lazy git!
				break;
			}
		case EVENT_UNIT_MOVE_FAILED:
			//UtilityInstance->ChatMsg("move failed");
			break;
		case EVENT_UNIT_DAMAGED:
			{
				struct SUnitDamagedEvent* evt = (struct SUnitDamagedEvent*)data;
				decision->UnitDamaged( evt->unit, evt->attacker );

				break;
			}
		case EVENT_UNIT_DESTROYED:
			{
				//ai->utility->ChatMsg("unit destroyed");
				struct SUnitDestroyedEvent* evt = (struct SUnitDestroyedEvent*) data;
				int unitId = evt->unit;
				int attackerId = evt->attacker;
				decision->UnitDestroyed(unitId, attackerId);
				break;
			}
		case EVENT_UNIT_GIVEN:
			//UtilityInstance->ChatMsg("unit given");
			break;
		case EVENT_UNIT_CAPTURED:
			//UtilityInstance->ChatMsg("unit captured");
			break;
		case EVENT_ENEMY_ENTER_LOS:
			{
				//ai->knowledge->selfInfo->armyInfo->Print();

				//ai->utility->ChatMsg("enemy enter los");
				//there he is, get him!
				struct SEnemyEnterLOSEvent* evt = (struct SEnemyEnterLOSEvent*)data;
				decision->EnemyEnterLOS(evt->enemy);
				break;
			}
		case EVENT_ENEMY_LEAVE_LOS:
			//ai->utility->ChatMsg("enemy leave los");
			//Where did he go?
			break;
		case EVENT_ENEMY_ENTER_RADAR:
			//ai->utility->ChatMsg("enemy enter radar");
			break;
		case EVENT_ENEMY_LEAVE_RADAR:
			//ai->utility->ChatMsg("enemy leave radar");
			break;
		case EVENT_ENEMY_DAMAGED:
			//ai->utility->ChatMsg("enemy damaged");
			break;
		case EVENT_ENEMY_DESTROYED:
			{
				//gotcha bitch!
				struct SEnemyDestroyedEvent* evt = (struct SEnemyDestroyedEvent*)data;
				decision->EnemyDestroyed(evt->enemy, evt->attacker);
				break;
			}
		case EVENT_WEAPON_FIRED:
			//ai->utility->ChatMsg("weapon fired");
			break;
		case EVENT_PLAYER_COMMAND:
			//ai->utility->ChatMsg("player command");
			//are you telling ME what to do?
			break;
		case EVENT_SEISMIC_PING:
			//ai->utility->ChatMsg("ping");
			//WTH was that?
			break;
		case EVENT_COMMAND_FINISHED:
			//UtilityInstance->ChatMsg("command finished");
			break;
		case EVENT_LOAD:
			//ai->utility->ChatMsg("load");
			break;
		case EVENT_SAVE:
			//ai->utility->ChatMsg("save");
			break;
		default: {
			//ai->utility->ChatMsg("recived an unhandled event with topic: %d",topic); 
			break;
				 }
	}
	//UtilityInstance->ChatMsg("Im done switching..."); 
	// signal: everything went OK
	return 0;
}
