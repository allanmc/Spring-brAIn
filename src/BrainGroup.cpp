
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
	Units[unit] = true;
}

void BrainGroup::RemoveUnit( Unit *unit)
{
	if ( Units.find( unit ) != Units.end() )
	{
		Units.erase( unit );
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

//Skal gøres mere intelligent. Evt tage gennemsnitligt centrum for alle units. 
//Det giver dog problemer hvis der er 1 unit der er kommet langt væk fra gruppen.
SAIFloat3 BrainGroup::GetPos()
{
	return Units.begin()->first->GetPos();
}

int BrainGroup::GetGroupID()
{
	return GroupID;
}