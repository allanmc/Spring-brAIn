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
	unitCount = 0;
	aggressive = 0;
	defensive = 0;
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

	if (quadTree->RemoveUnit( unit->GetUnitId()))
	{
		unitCount--;

		if(unit->GetDef()->GetSpeed() > 0)
		{
			aggressive--;
		}
		else
		{
			defensive--;
		}
	}	
}

void ArmyInfo::UpdateUnit(Unit* unit)
{
	SAIFloat3 new_pos = unit->GetPos();
	int i = quadTree->UpdateUnit( unit->GetUnitId(), unit->GetPos() );
	UnitDef* unitDef = unit->GetDef();
	if (unitDef->GetUnitDefId() != -1) {
		knownUnitDefs[unit->GetUnitId()] = unitDef;
	}
	unitCount += i;

	if(i != 0)
	{
		if(unitDef->GetSpeed() > 0)
		{
			aggressive += i;
		}
		else
		{
			defensive += i;
		}
	}
}

UnitDef* ArmyInfo::GetUnitDef(int unitID)
{
	ai->utility->Log( DEBUG, KNOWLEDGE, "SVEND!!!!" );
	UnitDef* foundDef = NULL;
	map<int,UnitDef*>::iterator iter = knownUnitDefs.find(unitID);
	ai->utility->Log( DEBUG, KNOWLEDGE, "SVEND2!!!!" );
	if (iter!=knownUnitDefs.end())
	{
		ai->utility->Log( DEBUG, KNOWLEDGE, "SVEND3!!!!" );
		foundDef = iter->second;
		ai->utility->Log( DEBUG, KNOWLEDGE, "SVEND4!!!!" );
	}
	ai->utility->Log( DEBUG, KNOWLEDGE, "SVEND5!!!!" );
	return foundDef;
}

SAIFloat3 ArmyInfo::GetUnitPos( int unitID )
{
	return quadTree->GetLastUnitPos(unitID);
}

int ArmyInfo::CountDefensive()
{
	return defensive;
}

int ArmyInfo::CountAggressive()
{
	return aggressive;
}
