#include "BrainGroup.h"

using namespace brainSpace;
using namespace springai;
BrainGroup::BrainGroup( string name )
{
	GroupName = name;
	Idle = true;
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