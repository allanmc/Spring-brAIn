#include "ArmyInfo.h"


using namespace std;
using namespace springai;
using namespace brainSpace;

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
	aggressiveDps = 0;
	defensiveDps = 0;
}

ArmyInfo::~ArmyInfo()
{
	delete quadTree;
	quadTree = NULL;
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
	ai->utility->Log(ALL, MISC, "Adding army unit...%s - %d", unit->GetDef()->GetHumanName(), unit->GetUnitId() );
	//quadTree->InsertUnit(unit->GetUnitId(), unit->GetPos());

	//unitCount++;
	//Using updateunit instead :)
	
	UpdateUnit(unit);
}

void ArmyInfo::RemoveUnit(int unit)
{
	//if (unitCount==0)
	//{
	//	return;
	//}
	bool removed = quadTree->RemoveUnit( unit);
	if (removed)
	{
		Unit* u = Unit::GetInstance(ai->callback, unit);
		unitCount--;
		UnitDef *unitDef = u->GetDef();
		if(unitDef != NULL)
		{

			if(unitDef->GetSpeed() > 0)
			{
				aggressive--;
				aggressiveDps -= ai->utility->GetDpsFromUnitDef(unitDef);
			}
			else
			{
				defensive--;
				defensiveDps -= ai->utility->GetDpsFromUnitDef(unitDef);
			}
		}
		delete unitDef;
	}	
}

//updates the position of the unit in the QuadTree.
void ArmyInfo::UpdateUnit(Unit* unit)
{
	if(unit == NULL)
		return;
	SAIFloat3 new_pos = unit->GetPos();
	int i = quadTree->UpdateUnit( unit->GetUnitId(), unit->GetPos() );
	UnitDef* unitDef = unit->GetDef();
	unitCount += i;
	if (unitDef == NULL) {
		return;//we dont know this unit?!
	}
	knownUnitDefs[unit->GetUnitId()] = unitDef;
	

	if(i != 0)
	{
		if(unitDef->GetSpeed() > 0)
		{
			aggressive += i;
			aggressiveDps += ai->utility->GetDpsFromUnitDef(unitDef);
		}
		else
		{
			defensive += i;
			defensiveDps += ai->utility->GetDpsFromUnitDef(unitDef);
		}
	}
}

///@return the UnitDef of a unit in the army, or NULL if we have never seen the unit
UnitDef* ArmyInfo::GetUnitDef(int unitID)
{
	UnitDef* foundDef = NULL;
	map<int,UnitDef*>::iterator iter = knownUnitDefs.find(unitID);
	if (iter!=knownUnitDefs.end() && iter->second != NULL)
	{
		ai->utility->Log(ALL, MISC, "GetUnitDef: we found a def, %s", iter->second->GetName());
		foundDef = iter->second;
	}
	return foundDef;
}

///@return the last known position of the unit
SAIFloat3 ArmyInfo::GetUnitPos( int unitID )
{
	return quadTree->GetLastUnitPos(unitID);
}

const map<int, struct UnitInformationContainer> ArmyInfo::GetUnits()
{
	return quadTree->GetUnits();
}

///@return the number of defensive units (stationary units with weapons)
int ArmyInfo::CountDefensive()
{
	return defensive;
}

///@return the number of aggressive units (mobile units with weapons)
int ArmyInfo::CountAggressive()
{
	return aggressive;
}

float ArmyInfo::GetAggresiveDps()
{
	return aggressiveDps;
}

float ArmyInfo::GetDefensiveDps()
{
	return defensiveDps;
}

int brainSpace::ArmyInfo::CountUnitsByName( const char* name )
{
	ai->utility->Log(ALL, MISC, "CountUnitsByName start ");
	map<int, UnitInformationContainer> i = quadTree->GetUnits();
	ai->utility->Log(ALL, MISC, "CountUnitsByName gotten units ");
	map<int, UnitInformationContainer>::iterator it = i.begin();
	ai->utility->Log(ALL, MISC, "CountUnitsByName gotten iterator ");

	int count = 0;
	while ( it != i.end() )
	{
		UnitDef *def = it->second.def;
		if(def == NULL)
		{
			ai->utility->Log(ALL, MISC, "CountUnitsByName: def is null");
			ai->utility->Log(ALL, MISC, "CountUnitsByName: unitid: %d, unitdef is maybe: %s", it->first, GetUnitDef(it->first)->GetName());
			//i.erase(it);
			it++;
			continue;
		}
		if ( strcmp(it->second.def->GetName(), name ) == 0 )
			count++;
		it++;
	}
	ai->utility->Log(ALL, MISC, "CountUnitsByName:return");
	return count;
}