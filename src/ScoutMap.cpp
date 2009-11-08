#include "ScoutMap.h"
#include "Knowledge.h"

using namespace std;
using namespace springai;
using namespace brainSpace;

ScoutMap::ScoutMap( AIClasses* aiClasses):BrainMap(aiClasses)
{
	ai->utility->Log( ALL, SLOPEMAP, "ScoutMap resolution %d. Width: %d. Height: %d", Resolution, MapWidth, MapHeight );
}

ScoutMap::~ScoutMap()
{
}

///updates the map depending on all friendly units LOS
void ScoutMap::Update()
{
	map<int, struct UnitInformationContainer> units = ai->knowledge->selfInfo->armyInfo->GetUnits();
	map<int, struct UnitInformationContainer>::iterator it;
	ai->utility->Log( ALL, MISC, "getunits done, size: %i",units.size());
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

///sets the current timestamp of a cell
void ScoutMap::EffectCell(int index, float value)
{
	MapArray[index] = value;
}
