#include "BattlesInfo.h"


using namespace std;
using namespace springai;
using namespace brainSpace;

BattlesInfo::BattlesInfo( AIClasses* aiClasses )
{
	ai = aiClasses;
	LastUpdateFrame = ai->frame;
}

BattlesInfo::~BattlesInfo()
{
	ai->utility->Log(ALL,MISC, "Start deleting battlesinfo");
	for ( list<Battle*>::iterator iter = CurrentBattles.begin() ; iter != CurrentBattles.end() ; iter++ )
	{
		delete *iter;
		*iter = NULL;
	}
	CurrentBattles.clear();
	ai->utility->Log(ALL,MISC, "middle deleting battlesinfo");
	//OldBattles
	for ( list<Battle*>::iterator iter = OldBattles.begin() ; iter != OldBattles.end() ; iter++ )
	{
		delete *iter;
		*iter = NULL;
	}
	OldBattles.clear();
	ai->utility->Log(ALL,MISC, "done deleting battlesinfo");
}


//1a: Find the battle which the friendly unit is a part of
//1b: Add the attacker to the battle
//2a: The friendly unit was not a part of any ongoing battle, so try and add it to the nearest battle
//2b: If that battle did not already contain the attacker, add it to the battle
//3a: No battles were taking place, so add both units to a new Battle object
void BattlesInfo::UnitDamaged( Unit* friendlyUnit, Unit* attackingUnit )
{
	SomeoneDamaged( friendlyUnit, attackingUnit );
}


void BattlesInfo::UnitDestroyed( Unit* deadFriendlyUnit, Unit* attackingUnit  )
{
	Battle* b = FindBattleContaining( deadFriendlyUnit );
	if ( b != NULL )
	{
		b->UnitDied( deadFriendlyUnit, false );
	}
	CleanupAfterSomeoneDied( deadFriendlyUnit );
}

void BattlesInfo::EnemyDestroyed( Unit* deadEnemyUnit, Unit* attackingUnit )
{
	Battle* b = FindBattleContaining( deadEnemyUnit );
	if ( b != NULL )
	{	
		b->UnitDied( deadEnemyUnit, true );
	}
	CleanupAfterSomeoneDied( deadEnemyUnit );
	
}

void BattlesInfo::EnemyDamaged( Unit* attacker, Unit* enemy )
{
	SomeoneDamaged( attacker, enemy );
}

void BattlesInfo::Update ( int frame )
{
	LastUpdateFrame = frame;
	/** Erase any inactive battles and copy them to the OldBattles list **/
	for ( list<Battle*>::iterator iter = CurrentBattles.begin() ; iter != CurrentBattles.end() ; iter++ )
	{
		/** If no activity in current battle, oldify it **/
		if ((*iter)->GetLastFrameOfActivity() + BATTLE_TIMEOUT < LastUpdateFrame )
		{
			(*iter)->Oldify();
			OldBattles.push_back( *iter );
			iter = CurrentBattles.erase( iter );
			ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "Inactive battle erased" );
		}
		else //Else update the battle object
		{
			(*iter)->Update();
		}
	}

	if ( frame % 600 == 0 )
	{
		ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "\nPRINTING UPDATED BATTLE INFO: frame %d\n", frame );
		ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "\n\nNum active battles %d\n==============", CurrentBattles.size() );
		int i = 0;
		for ( list<Battle*>::iterator iter = CurrentBattles.begin() ; iter != CurrentBattles.end() ; iter++, i++ )
		{
			ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "\n\nBattle number %d\n--------", i );
			(*iter)->ToString();
		}

		i = 0;
		ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "\n\nNum old battles %d\n==============", OldBattles.size() );
		for ( list<Battle*>::iterator iter = OldBattles.begin() ; iter != OldBattles.end() ; iter++, i++ )
		{
			ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "Battle number %d", i );
			(*iter)->ToString();
		}
		ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "\nDONE PRINTING UPDATED BATTLE INFO\n" );
	}
}

Battle* BattlesInfo::FindBattleContaining( Unit* u )
{
	for ( list<Battle*>::iterator iter = CurrentBattles.begin() ; iter != CurrentBattles.end() ; iter++ )
		if ( (*iter)->Contains( u ) )
			return *iter;
	return NULL;
}

Battle* BattlesInfo::FindNearestBattle( SAIFloat3 pos )
{
	//Old lars idea
	//for ( list<Battle*>::iterator iter = CurrentBattles.begin() ; iter != CurrentBattles.end() ; iter++ )
	//	if ( ai->utility->EuclideanDistance( (*iter)->GetCenter(), pos ) < (*iter)->GetRadius()+RADIUS_BUFFER )
	//	{
	//		return *iter;
	//	}
	//	return NULL;
	
	//jeppes idea, new battle would intersect with current battle, then use current battle
	for ( list<Battle*>::iterator iter = CurrentBattles.begin() ; iter != CurrentBattles.end() ; iter++ )
	{
		if ( ai->math->CircleIntersetCircle((*iter)->GetCenter(), (*iter)->GetRadius(), pos, INITIAL_BATTLE_RADIUS ))
		{
			return *iter;
		}
	}
	return NULL;
}


void BattlesInfo::SomeoneDamaged( Unit* our, Unit* their )
{
	Battle* b = FindBattleContaining( our );

	if ( b != NULL )
	{
		b->SomeoneDamaged();
		if ( !b->Contains( their ) )
			b->UnitEnteredBattle( their, true );
		return;
	}
	else
	{
		b = FindNearestBattle( our->GetPos() );
		if ( b != NULL )
		{
			b->UnitEnteredBattle( our, false );
			if ( !b->Contains( their ) )
				b->UnitEnteredBattle( their, true );
			return;
		}
		//The unit is not a part of any battles, and there are no battles nearby so make a new Battle object.

		ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "New battle. Frame %d. Pos (%f, %f)", ai->frame, our->GetPos().x, our->GetPos().z );
		b = new Battle( ai, our->GetPos() );
		b->UnitEnteredBattle( our, false );
		b->UnitEnteredBattle( their, true );
		CurrentBattles.push_back( b );
	}
}

void BattlesInfo::CleanupAfterSomeoneDied( Unit* unitToCleanup )
{
	for ( list<Battle*>::iterator iter = CurrentBattles.begin() ; iter != CurrentBattles.end() ; iter++ )
	{
		if ( (*iter)->Contains( unitToCleanup ) )
		{
			(*iter)->RemoveUnit( unitToCleanup );
		}
	}
}


int BattlesInfo::NumberOfBattlesInArea( int timePeriod, CBoundingBox box )
{
	int count = 0;
	for ( list<Battle*>::iterator iter = CurrentBattles.begin() ; iter != CurrentBattles.end() ; iter++ )
	{
		if ( (*iter)->GetLastFrameOfActivity() > ai->frame-timePeriod )
		{
			CBoundingBox circleBoundingbox;
			SAIFloat3 circleCenter = (*iter)->GetCenter();
			float circleRadius = (*iter)->GetRadius();

			circleBoundingbox.topLeft.x = circleCenter.x - circleRadius;
			circleBoundingbox.topLeft.z = circleCenter.z - circleRadius;
			circleBoundingbox.bottomRight.x = circleCenter.x + circleRadius;
			circleBoundingbox.bottomRight.z = circleCenter.z + circleRadius;

			if ( !circleBoundingbox.Intersects( box ) )
				continue;

			SAIFloat3 temp;
			if ( box.bottomRight.x < circleCenter.x )//To the right
				temp.x = box.bottomRight.x;
			else if ( box.topLeft.x > circleCenter.x )
				temp.x = box.topLeft.x;
			else
			{
				count++;
				continue;
			}

			if ( box.bottomRight.z < circleCenter.z )
				temp.z = box.bottomRight.z;
			else if ( box.topLeft.z > circleCenter.z )
				temp.z = box.topLeft.z;
			else
			{
				count++;
				continue;
			}

			if ( ai->utility->EuclideanDistance( circleCenter, temp ) < circleRadius )
				count++;
		}
	}

	for ( list<Battle*>::iterator iter = OldBattles.begin() ; iter != OldBattles.end() ; iter++ )
	{
		if ( (*iter)->GetLastFrameOfActivity() > ai->frame-timePeriod )
		{
			CBoundingBox circleBoundingbox;
			SAIFloat3 circleCenter = (*iter)->GetCenter();
			float circleRadius = (*iter)->GetRadius();

			circleBoundingbox.topLeft.x = circleCenter.x - circleRadius;
			circleBoundingbox.topLeft.z = circleCenter.z - circleRadius;
			circleBoundingbox.bottomRight.x = circleCenter.x + circleRadius;
			circleBoundingbox.bottomRight.z = circleCenter.z + circleRadius;

			if ( !circleBoundingbox.Intersects( box ) )
				continue;

			SAIFloat3 temp;
			if ( box.bottomRight.x < circleCenter.x )//To the right
				temp.x = box.bottomRight.x;
			else if ( box.topLeft.x > circleCenter.x )
				temp.x = box.topLeft.x;
			else
			{
				count++;
				continue;
			}

			if ( box.bottomRight.z < circleCenter.z )
				temp.z = box.bottomRight.z;
			else if ( box.topLeft.z > circleCenter.z )
				temp.z = box.topLeft.z;
			else
			{
				count++;
				continue;
			}

			if ( ai->utility->EuclideanDistance( circleCenter, temp ) < circleRadius )
				count++;
		}
	}

	return count;
}