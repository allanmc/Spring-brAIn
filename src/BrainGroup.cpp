
#include "BrainGroup.h"

using namespace std;
using namespace springai;
using namespace brainSpace;

BrainGroup::BrainGroup( AIClasses* aiClasses, int groupID )
{
	Idle = true;
	ai = aiClasses;
	GroupID = groupID;
	LastFrameGetPos = 0;
}

BrainGroup::~BrainGroup()
{
}


void BrainGroup::RemoveUnit( Unit *unit)
{
	for ( map<int, bool>::iterator it = Units.begin() ; it != Units.end() ; it++ )
	{
		if (it->first == unit->GetUnitId())
		{
			Units.erase(it);
			break;
		}
	}
}

int BrainGroup::GetSize()
{
	return Units.size();
}

bool BrainGroup::IsIdle()
{
	return Idle;
}



SAIFloat3 BrainGroup::GetPos()
{

	if ( LastFrameGetPos == ai->frame )
		return CachedPosition[0][0];

	KMedoids k( ai );
	vector<SAIFloat3> points;
	for ( map<int, bool>::iterator it = Units.begin() ; it != Units.end() ; it++ )
	{
		Unit* u = Unit::GetInstance( ai->callback, it->first );
		points.push_back( u->GetPos() );
		delete u;
	}
	k.AddPoints( points );
	CachedPosition = k.GetClusters( 1 );
	LastFrameGetPos = ai->frame;
	return CachedPosition[0][0];
}

int BrainGroup::GetGroupID()
{
	return GroupID;
}

std::vector<int> BrainGroup::GetUnits()
{
	vector<int> retVal;
	for ( map<int, bool>::iterator it = Units.begin() ;it != Units.end() ; it++)
	{
		retVal.push_back( it->first );
	}
	return retVal;
}

bool BrainGroup::Contains(int unitID)
{
	return ( Units.find(unitID) == Units.end() ) ? false : true;
}