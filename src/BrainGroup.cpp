
#include "BrainGroup.h"

using namespace std;
using namespace springai;
using namespace brainSpace;

BrainGroup::BrainGroup( AIClasses* aiClasses, int groupID )
{
	Idle = true;
	ai = aiClasses;
	GroupID = groupID;
}

BrainGroup::~BrainGroup()
{
}

void BrainGroup::AddUnit( Unit *unit)
{
	if (ai->commander==NULL && unit->GetDef()->IsCommander())
	{
		ai->commander = unit;
	}
	Units[unit] = true;
}

void BrainGroup::RemoveUnit( Unit *unit)
{
	for ( map<Unit*, bool>::iterator it = Units.begin() ; it != Units.end() ; it++ )
	{
		if (it->first->GetUnitId() == unit->GetUnitId())
		{
			delete it->first;
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

	KMedoids k( ai );
	vector<SAIFloat3> points;
	for ( map<Unit*, bool>::iterator it = Units.begin() ; it != Units.end() ; it++ )
	{
		points.push_back( it->first->GetPos() );
	}
	k.AddPoints( points );
	vector<vector<SAIFloat3> > clusters = k.GetClusters( 1 );
	
	return clusters[0][0];
}

int BrainGroup::GetGroupID()
{
	return GroupID;
}