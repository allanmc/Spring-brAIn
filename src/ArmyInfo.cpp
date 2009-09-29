#include "ArmyInfo.h"


using namespace brainSpace;
using namespace springai;

ArmyInfo::ArmyInfo( AIClasses* aiClasses )
{
	ai = aiClasses;
	CBoundingBox bbox;
	bbox.topLeft = (SAIFloat3){0,0,0};
	bbox.bottomRight = (SAIFloat3){	ai->callback->GetMap()->GetWidth()*8,
									0,
									ai->callback->GetMap()->GetHeight()*8};
	quadTree = new QuadTree( ai, bbox );
}

ArmyInfo::~ArmyInfo()
{
}

void ArmyInfo::Print()
{
	quadTree->Print();
}

vector<Unit*> ArmyInfo::RangeQuery(float topLeftX, float topLeftZ, float bottomRightX, float bottomRightZ)
{
	return quadTree->RangeQuery(topLeftX, topLeftZ, bottomRightX, bottomRightZ);
}

vector<Unit*> ArmyInfo::RangeQuery(SAIFloat3 topLeft, SAIFloat3 bottomRight)
{
	return quadTree->RangeQuery(topLeft, bottomRight);
}

vector<Unit*> ArmyInfo::RangeQuery(CBoundingBox bbox)
{
	return quadTree->RangeQuery(bbox);
}

void ArmyInfo::AddUnit(Unit* unit)
{
	//ai->utility->ChatMsg("Adding army unit...");
	quadTree->InsertUnit(unit->GetUnitId(), unit->GetPos());

	unitCount++;
}

void ArmyInfo::RemoveUnit(Unit* unit)
{
	if (unitCount==0)
	{
		return;
	}
	quadTree->RemoveUnit( unit->GetUnitId() );
	//remove unit from quadtree, using pos

	unitCount--;
}

void ArmyInfo::UpdateUnit(Unit* unit)
{
	SAIFloat3 new_pos = unit->GetPos();
	quadTree->UpdateUnit( unit->GetUnitId(), unit->GetPos() );
	//Update quadtree, using old_pos and new_pos
}

SAIFloat3 ArmyInfo::GetUnitPos( int unitID )
{
	return quadTree->GetLastUnitPos(unitID);
}