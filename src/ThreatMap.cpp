#include "ThreatMap.h"
#include "Knowledge.h"

using namespace std;
using namespace springai;
using namespace brainSpace;


ThreatMap::ThreatMap( AIClasses* aiClasses ) : BrainMap(aiClasses)
{
	ArmorType = -1;
}


ThreatMap::~ThreatMap()
{
}

///updates the map depending on all friendly units DPS and range, also draws the content od the cells
void ThreatMap::Update()
{
	bool printArray = false;
	map<int, struct UnitInformationContainer> enemyUnits = ai->knowledge->enemyInfo->armyInfo->GetUnits();
	Reset();

	for ( map<int, struct UnitInformationContainer>::iterator it = enemyUnits.begin() ; it != enemyUnits.end() ; it++ )
	{
		InsertUnit( Unit::GetInstance( ai->callback, it->first ), it->second );
	}

	DrawGrid();
	ai->utility->RemoveGraphics(FigureID);
	FigureID = 0;
	for ( int i = 0 ; i < MapWidth ; i++ )
		for ( int j = 0 ; j < MapHeight ; j++ )
		{
			if ( MapArray[ j*MapWidth + i ] != 0 )
			{
				SAIFloat3 start, end;
				start.x = i*Resolution;
				start.y = 100;
				start.z = j*Resolution + 0.5*Resolution;

				end.x = (i+1)*Resolution;
				end.y = 100;
				end.z = j*Resolution + 0.5*Resolution;

				
				int id = ai->utility->DrawLine( start, end, false, Resolution*2, FigureID );
				if(FigureID == 0) FigureID = id;
				printArray = true;
			}
		}

		if ( printArray )
			for ( int i = 0 ; i < MapHeight*MapWidth ; i++ )
			{
				ai->utility->Log( ALL, KNOWLEDGE, "ThreatArray %d, %f", i, MapArray[i]);
			}
}

///udates the map with the information from a single enemy uit
void ThreatMap::InsertUnit(Unit *u, struct UnitInformationContainer c )
{
	if ( ArmorType == -1 )
		ArmorType = c.def->GetArmorType();


	vector<WeaponMount*> weaponMounts = c.def->GetWeaponMounts();
	for ( int i = 0 ; i < weaponMounts.size() ; i++ )
	{
		WeaponDef* def = weaponMounts.at(i)->GetWeaponDef();
		EffectCircle(c.pos, def->GetRange(), CalculateDPS( def ));
	}
}

///calucales the DPS for a given weapon 
int ThreatMap::CalculateDPS( WeaponDef* w )
{
	return ( w->GetDamage()->GetTypes().at(ArmorType) / w->GetReload() );
}

///Adds threat to a given cell
void ThreatMap::EffectCell(int index, float value)
{
	MapArray[index] += value;
}

///@return the DPS against the armor specified at a given point
float ThreatMap::GetThreatAtPos( SAIFloat3 pos, int armorType )
{
	if ( armorType == ArmorType )
		return MapArray[ (int)(floorf(pos.z/Resolution)*MapWidth + floorf(pos.x/Resolution)) ];
	return 0;
}