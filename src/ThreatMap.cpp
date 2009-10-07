#include "ThreatMap.h"


ThreatMap::ThreatMap( AIClasses* aiClasses )
{
	ai = aiClasses;
	ThreatMapResolution = 9;
	CellHeight = ai->callback->GetMap()->GetHeight()/ThreatMapResolution;
	CellWidth = ai->callback->GetMap()->GetWidth()/ThreatMapResolution;
	ThreatMapWidth = (ai->callback->GetMap()->GetWidth())/CellWidth;
	ThreatMapHeight = (ai->callback->GetMap()->GetWidth())/CellHeight;
	ThreatArray = new float[ThreatMapWidth*ThreatMapHeight];
}


ThreatMap::~ThreatMap()
{
}

void ThreatMap::Update()
{
	ai->utility->Log( DEBUG, KNOWLEDGE, "Piksved" );
	map<int, SAIFloat3> enemyUnits = ai->knowledge->enemyInfo->armyInfo->GetEnemyUnits();
	for ( map<int, SAIFloat3>::iterator it = enemyUnits.begin() ; it != enemyUnits.end() ; it++ )
	{
		ai->utility->Log( DEBUG, KNOWLEDGE, "Allan lugter" );
		InsertUnit( Unit::GetInstance( ai->callback, it->first ) );
	}

}

void ThreatMap::InsertUnit(Unit *u)
{
	vector<WeaponMount*> weaponMounts = u->GetDef()->GetWeaponMounts();
	for ( int i = 0 ; i < weaponMounts.size() ; i++ )
	{
		WeaponDef* def = weaponMounts.at(i)->GetWeaponDef();
		float range = def->GetRange();
		float reload = def->GetReload();
		Damage* damage = def->GetDamage();
		vector<float> types = damage->GetTypes();
		float craterBoost = damage->GetCraterBoost();
		float craterMult = damage->GetCraterMult();
		float impulseBoost = damage->GetImpulseBoost();
		float impulseFactor = damage->GetImpulseFactor();
		float dynDamageExp = def->GetDynDamageExp();
		float dynDamageMin = def->GetDynDamageMin();
		float dynDamageRange = def->GetDynDamageRange();

		UnitDef* uDef = u->GetDef();
		if ( uDef->GetUnitDefId() != -1 )
			ai->utility->Log( DEBUG, KNOWLEDGE, "Unit %s", uDef->GetHumanName() ); 
		ai->utility->Log( DEBUG, KNOWLEDGE, "range: %f reload %f crater boost: %f craterMult: %f impulseBoost %f impulseFactor %f dynDamageExp %f dynDamageMin %f dynDamageRange %f", range, reload, craterBoost, craterMult, impulseBoost, impulseFactor, dynDamageExp, dynDamageMin, dynDamageRange );
		
		for ( int i = 0 ; i < types.size() ; i++ )
			ai->utility->Log( DEBUG, KNOWLEDGE, "types index %d %f", i, types.at(i) );
		for ( int x = 0 ; x < ThreatMapWidth ; x++ )
		{
			for ( int z = 0 ; z < ThreatMapHeight ; z++ )
			{
				//vector<Unit*> units = ai->knowledge->enemyInfo->armyInfo->RangeQuery( x*CellWidth, z*CellHeight, (x+1)*CellWidth, (z+1)*CellHeight );

			}
		}
	}
}