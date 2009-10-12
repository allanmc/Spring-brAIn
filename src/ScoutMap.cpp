#include "ScoutMap.h"

using namespace std;
using namespace springai;
using namespace brainSpace;

ScoutMap::ScoutMap( AIClasses* aiClasses):BrainMap(aiClasses)
{
}

ScoutMap::~ScoutMap()
{
}

void ScoutMap::Update()
{
	map<int, struct UnitInformationContainer> units = ai->knowledge->selfInfo->armyInfo->GetUnits();
	map<int, struct UnitInformationContainer>::iterator it;
	for(it = units.begin();it != units.end(); it++)
	{
		UnitDef *def = it->second.def;
		float radius;
		if(def && def->GetUnitDefId() != -1)
		{
			radius = def->GetLosRadius();
		}
		else
		{
			continue;//can not use this unit for any thing!
		}
		SAIFloat3 pos = it->second.pos;
		EffectCircle(pos, radius, ai->frame);
	}
}

void ScoutMap::EffectCell(int index, float value)
{
	MapArray[index] = value;
}
