#include "BrAIn.h"

using namespace std;
using namespace springai;
using namespace brainSpace;

brainSpace::BrAIn::BrAIn(springai::AICallback* cb)
{
	ai = new AIClasses();
	ai->callback = cb;
	ai->utility = new Utility(ai);
	ai->utility->Log(ALL, MISC, "Utility loaded...");
	ai->knowledge = new Knowledge(ai);
	ai->utility->Log(ALL, MISC, "Knowledge loaded...");
	ai->math = new BrainMath( ai );

	teamId = (ai->callback != NULL ? ai->callback->GetTeamId() : -1);

	decision = new Decision(ai);
	ai->utility->Log(ALL, MISC, "Decision loaded...");
	//ai->utility->ChatMsg("Hello world i am team: %d",teamId);
	srand(0);
}
brainSpace::BrAIn::~BrAIn()
{
	delete(ai->utility);
	delete(ai->knowledge);
	delete(ai->math);
	delete(decision);
	delete(ai);
	
}

///Passes the events received on to the Decision class
///@see Decision
///return 0 on success, and something else if an error happend.
int brainSpace::BrAIn::HandleEvent(int topic, const void* data) {

	//ai->utility->Log(ALL,EVENT,"Recived an event with topic: %i", topic);

	//if(topic > 50) ai->utility->Log(ALL,EVENT,"wierd crap");
	switch (topic) {
		case EVENT_INIT:
			//ai->utility->ChatMsg("THE BEGINNING! LOG_LEVEL: %i, LOG_TYPE: %i, EVENT: %i", LOG_LEVEL, LOG_TYPE);
			ai->utility->Log(ALL, CHAT|DECISION, "Logging started...");
			//do i really need to do anything here? (i allready have a teamId and a callback)
			break;
		case EVENT_RELEASE:
			ai->utility->Log(DEBUG,CHAT,"THE END!");
			//i am no longer needed
			//delete(decision);
			break;
		case EVENT_UPDATE:
			{
				//ai->utility->Log(DEBUG,EVENT,"update");
				struct SUpdateEvent* evt = (struct SUpdateEvent*) data;
				int frame = evt->frame;
				ai->frame = frame;
				decision->Update(frame);
				break;
			}
		case EVENT_MESSAGE:
			{
				ai->utility->Log(DEBUG,EVENT,"message");
				//struct SMessageEvent* evt = (struct SMessageEvent*) data;
				//why are you talking to me? im a bot!
				break;
			}
		case EVENT_UNIT_CREATED:
			{
				ai->utility->Log(DEBUG,EVENT,"unit created");
				struct SUnitCreatedEvent* evt = (struct SUnitCreatedEvent*) data;
				int unitId = evt->unit;
				int builder = evt->builder;
				
				decision->UnitCreated(unitId, builder);

				// TODO: wrapp events and commands too

				//std::string unitDefName = ai->callback->GetFriendlyUnits()[0]->GetDef()->GetName();

				//ai->utility->ChatMsg("Hello Engine (from brAIn), first friendly untis def name is: " + unitDefName);

				break;
			}
		case EVENT_UNIT_FINISHED:
			{
				ai->utility->Log(DEBUG,EVENT,"Unit finished");
				struct SUnitFinishedEvent* evt = (struct SUnitFinishedEvent*) data;
				int unitId = evt->unit;
				decision->UnitFinished(unitId);
				break;
			}
		case EVENT_UNIT_IDLE:
			{
				ai->utility->Log(DEBUG,EVENT,"UNIT IDLE");
				struct SUnitIdleEvent* evt = (struct SUnitIdleEvent*)data;
				decision->UnitIdle( evt->unit );
				//get back to work ya lazy git!
				break;
			}
		case EVENT_UNIT_MOVE_FAILED:
			ai->utility->Log(ALL,MISC,"I GOT A MOVE FAILED!!!!! WARNING, THIS SHALL NEVER HAPPEN!!!!!");
			ai->utility->Log(IMPORTANT,EVENT,"I GOT A MOVE FAILED!!!!! WARNING, THIS SHALL NEVER HAPPEN!!!!!");
			break;
		case EVENT_UNIT_DAMAGED:
			{
				ai->utility->Log(DEBUG,EVENT,"unit damaged");
				struct SUnitDamagedEvent* evt = (struct SUnitDamagedEvent*)data;
				decision->UnitDamaged( evt->unit, evt->attacker );

				break;
			}
		case EVENT_UNIT_DESTROYED:
			{
				ai->utility->Log(DEBUG,EVENT,"unit destroyed");
				struct SUnitDestroyedEvent* evt = (struct SUnitDestroyedEvent*) data;
				int unitId = evt->unit;
				int attackerId = evt->attacker;
				decision->UnitDestroyed(unitId, attackerId);
				break;
			}
		case EVENT_UNIT_GIVEN:
			ai->utility->Log(DEBUG,EVENT,"unit given");
			break;
		case EVENT_UNIT_CAPTURED:
			ai->utility->Log(DEBUG,EVENT,"unit captured");
			break;
		case EVENT_ENEMY_ENTER_LOS:
			{
				//ai->knowledge->selfInfo->armyInfo->Print();

				ai->utility->Log(DEBUG,EVENT,"enemy enter los");
				//there he is, get him!
				struct SEnemyEnterLOSEvent* evt = (struct SEnemyEnterLOSEvent*)data;
				decision->EnemyEnterLOS(evt->enemy);
				break;
			}
		case EVENT_ENEMY_LEAVE_LOS:
			ai->utility->Log(DEBUG,EVENT,"enemy leave los");
			//Where did he go?
			break;
		case EVENT_ENEMY_ENTER_RADAR:
			ai->utility->Log(DEBUG,EVENT,"enemy enter radar");
			break;
		case EVENT_ENEMY_LEAVE_RADAR:
			ai->utility->Log(DEBUG,EVENT,"enemy leave radar");
			break;
		case EVENT_ENEMY_DAMAGED:
			{
			ai->utility->Log(DEBUG,EVENT,"enemy damaged");
			struct SEnemyDamagedEvent* evt = (struct SEnemyDamagedEvent*)data;
			decision->EnemyDamaged( evt->attacker, evt->enemy );
			break;
			}
		case EVENT_ENEMY_DESTROYED:
			{
				//gotcha bitch!
				ai->utility->Log(DEBUG,EVENT,"enemy destroyed");

				struct SEnemyDestroyedEvent* evt = (struct SEnemyDestroyedEvent*)data;
				decision->EnemyDestroyed(evt->enemy, evt->attacker);
				break;
			}
		case EVENT_WEAPON_FIRED:
			ai->utility->Log(DEBUG,EVENT,"weapon fired");
			break;
		case EVENT_PLAYER_COMMAND:
			ai->utility->Log(DEBUG,EVENT,"player command");
			//are you telling ME what to do?
			break;
		case EVENT_SEISMIC_PING:
			ai->utility->Log(DEBUG,EVENT,"ping");
			//WTH was that?
			break;
		case EVENT_COMMAND_FINISHED:
			ai->utility->Log(DEBUG,EVENT,"command finished");
			break;
		case EVENT_LOAD:
			ai->utility->Log(DEBUG,EVENT,"load");
			break;
		case EVENT_SAVE:
			ai->utility->Log(DEBUG,EVENT,"save");
			break;
		default: {
			ai->utility->Log(DEBUG,EVENT,"recived an unhandled event with topic: %d",topic); 
			break;
				 }
	}
	//ai->utility->Log(ALL,EVENT,"Im done switching..."); 
	// signal: everything went OK
	return 0;
}
