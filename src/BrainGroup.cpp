#include "BrainGroup.h"


using namespace brainSpace;
using namespace springai;
BrainGroup::BrainGroup( AICallback* callback )
{
	Idle = true;
	Callback = callback;
}

BrainGroup::~BrainGroup()
{
}

void BrainGroup::AddUnit( Unit *unit)
{
	Units.push_back( unit );
}

void BrainGroup::RemoveUnit( Unit *unit)
{
	for ( int i = 0 ; i < Units.size() ; i++ )
	{
		if ( Units.at(i)->GetUnitId() == unit->GetUnitId() )
		{
			Units.erase( Units.begin() + i );
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

void BrainGroup::AssignBuildOrder( SBuildUnitCommand order )
{
	Idle = false;
	Callback->GetEngine()->HandleCommand( 0, -1, COMMAND_UNIT_BUILD, &order );
}