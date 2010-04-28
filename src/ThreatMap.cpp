#include "ThreatMap.h"
#include "Knowledge.h"

using namespace std;
using namespace springai;
using namespace brainSpace;


ThreatMap::ThreatMap( AIClasses* aiClasses ) : BrainMap(aiClasses, 64)
{
	OurThreat = new float[MapWidth*MapHeight];
	ai = aiClasses;
	ReferenceUnitDefAir = NULL;
	ReferenceUnitDefGround = NULL;
	for ( int i = 0 ; i < MapWidth*MapHeight ; i++ )
	{
		OurThreat[i] = 0;
	}

	OverrideCache = NULL;
}


ThreatMap::~ThreatMap()
{
	delete[] OurThreat;
	EnemyUnitInfoCache.clear();
	FriendlyUnitInfoCache.clear();
}

///updates the map depending on all friendly units DPS and range, also draws the content od the cells
void ThreatMap::Update()
{
	map<int, UnitInformationContainer> enemyUnits = ai->knowledge->enemyInfo->armyInfo->GetUnits();
	map<int, UnitInformationContainer> friendlyUnits = ai->knowledge->selfInfo->armyInfo->GetUnits();

	//Reset();
	if ( ReferenceUnitDefAir == NULL || ReferenceUnitDefGround )
	{
		ReferenceUnitDefAir = ai->utility->GetUnitDef( "armkam" );
		ReferenceUnitDefGround = ai->utility->GetUnitDef( "armham" );
	}


	for ( int i = 0 ; i < MapWidth*MapHeight ; i++ )
	{
		MapArray[i] = 0;
		OurThreat[i] = 0;
	}

	FriendlyUnitInfoCache.clear();
	EnemyUnitInfoCache.clear();


	//Enemy threat vs. friendly units
	CalcThreatForEnemy = true;
	for ( map<int, UnitInformationContainer>::iterator it = enemyUnits.begin() ; it != enemyUnits.end() ; it++ )
	{
		bool insert = true;
		if(it->second.def == NULL || (it->second.pos.x == 0 && it->second.pos.z == 0) )
		{
			continue;
		}
		Unit* u = Unit::GetInstance( ai->callback, it->first );
		//We are using the OverrideCache to move some enemy units
		if ( OverrideCache != NULL && !OverrideCacheFriendly )
		{
			for ( int i = 0 ; i < OverrideCache->units.size() ; i++ )
			{
				if ( u->GetUnitId() == OverrideCache->units[i] )
				{
					Unit* cacheUnit = Unit::GetInstance( ai->callback, OverrideCache->units[i] );
					ai->utility->ChatMsg("TM: Moving enemy units (OverrideCache)");
					insert = false;
					UnitInformationContainer tmp = it->second;
					int res = ai->knowledge->mapInfo->threatMap->GetMapData()->MapResolution;
					tmp.pos.x = OverrideCache->xCell*res+0.5f*res;
					tmp.pos.z = OverrideCache->yCell*res+0.5f*res;
					InsertUnit( cacheUnit, tmp );
					delete cacheUnit;
					break;
				}
			}
		}
		if ( insert )
			InsertUnit( u, it->second );
		delete u;
	}

	//Friendly threat vs. enemy units
	CalcThreatForEnemy = false;
	for ( map<int, UnitInformationContainer>::iterator it = friendlyUnits.begin() ; it != friendlyUnits.end() ; it++ )
	{
		bool insert = true;
		if(it->second.def == NULL || (it->second.pos.x == 0 && it->second.pos.z == 0) )
		{
			continue;
		}
		Unit* u = Unit::GetInstance( ai->callback, it->first );
		//We are using the OverrideCache to move some friendly units
		if ( OverrideCache != NULL && OverrideCacheFriendly )
		{
			for ( int i = 0 ; i < OverrideCache->units.size() ; i++ )
			{
				if ( u->GetUnitId() == OverrideCache->units[i] )
				{
					insert = false;
					UnitInformationContainer tmp = it->second;
					int res = ai->knowledge->mapInfo->threatMap->GetMapData()->MapResolution;
					tmp.pos.x = OverrideCache->xCell*res+0.5f*res;
					tmp.pos.z = OverrideCache->yCell*res+0.5f*res;
					//ai->utility->ChatMsg("TM: Moving friendly units to (%f,%f,%f)", tmp.pos.x, tmp.pos.y, tmp.pos.z );
					InsertUnit( u, tmp );
					break;
				}
			}
		}
		if ( insert )
			InsertUnit( u, it->second );
		delete u;
	}
	FigureID = 0;
	/*
	for ( int i = 0 ; i < MapWidth ; i++ )
	{
		for ( int j = 0 ; j < MapHeight ; j++ )
		{
			if ( MapArray[ j*MapWidth + i ] != 0 )
			{
				//ai->utility->ChatMsg("Threat: (%d, %d): %f", i, j, MapArray[j*MapWidth+i] );
				SAIFloat3 start, end;
				start.x = i*Resolution;
				start.y = 80.0f;
				start.z = j*Resolution;
				end.x = (i+1)*Resolution;
				end.y = 80.0f;
				end.z = j*Resolution;
				int id = ai->utility->DrawLine( start, end, false, Resolution*4, FigureID );
				if(FigureID == 0) FigureID = id;
			}
		}
	}
	*/
}

///udates the map with the information from a single enemy uit
void ThreatMap::InsertUnit(Unit *u, UnitInformationContainer c )
{
	vector<WeaponMount*> weaponMounts = c.def->GetWeaponMounts();
	//ai->utility->ChatMsg("Inserting %s with %d weapons at (%f,%f)", c.def->GetName(), weaponMounts.size(), c.pos.x, c.pos.z );
	for ( unsigned int i = 0 ; i < weaponMounts.size() ; i++ )
	{
		CurrentWeaponDefCanHitAir = ( CanHit( weaponMounts[i], true ) );
		CurrentWeaponDefCanHitGround = ( CanHit( weaponMounts[i], false ) );
		CurrentWeaponDef = weaponMounts[i]->GetWeaponDef();
		if ( strcmp ( "arm_disintegrator", CurrentWeaponDef->GetName() ) == 0 )
			continue;
		float range = CurrentWeaponDef->GetRange();
		//EffectCircle(c.pos, 0, -1 );
		int currentIndexX = floorf(c.pos.x/Resolution );
		int currentIndexZ = floorf(c.pos.z/Resolution);
		for ( int j = -1 ; j <= 1 ; j++ )
		{
			for ( int k = -1 ; k <= 1 ; k++ )
			{
				int tmpX = currentIndexX+j;
				int tmpZ = currentIndexZ+k;

				if ( tmpX < 0 || tmpX >= MapWidth ) continue;
				if ( tmpZ < 0 || tmpZ >= MapHeight ) continue;
				SAIFloat3 tmpPos;
				tmpPos.x = tmpX*Resolution + 0.5f*Resolution;
				tmpPos.z = tmpZ*Resolution + 0.5f*Resolution;

				if ( ai->utility->EuclideanDistance( tmpPos, u->GetPos() ) < range )
					EffectCell( tmpZ*MapWidth + tmpX, -1 );
			}
		}
		delete CurrentWeaponDef;
	}
}

/**
* Calucales DPS for a given vector of weapons. Either calculated for a specified armortype, 
* or if no armortype is given, the maximum possible DPS for any armortype is returned
**/
float ThreatMap::CalculateDPS( int armorType, int numberOfUnitsWithThatArmorType, bool airUnits )
{
	//ai->utility->ChatMsg("TM: CalcDPS. AirUnits: %d. WeaponDefCanHitAir: %d. WeaponDefCanHitGround: %d", airUnits, CurrentWeaponDefCanHitAir, CurrentWeaponDefCanHitGround );
	//if ( ( airUnits && !CurrentWeaponDefCanHitAir ) || ( !airUnits && !CurrentWeaponDefCanHitGround ) )
	//	return 0;
	float val = 0.0f;
	Damage* d = CurrentWeaponDef->GetDamage();
	val = d->GetTypes().at(armorType);
	float reloadFactor = 1/CurrentWeaponDef->GetReload();
	val *= reloadFactor;

	//if ( ( airUnits && !CurrentWeaponDefCanHitAir ) || ( !airUnits && !CurrentWeaponDefCanHitGround ) )
		//ai->utility->ChatMsg("TM: CalcDPS Val %f", val );
	delete d;
	return val;
}

///Adds threat to a given tile based on the enemy units present in that tile
void ThreatMap::EffectCell(int index, float value )
{
	value = 0.0f;
	int tmpZ = index/MapWidth;
	int tmpX = index % MapWidth;


	//ai->utility->ChatMsg("TM: EffectCell (%d,%d)", tmpX, tmpZ );
	//We are to calculate threat for the given weapondef against another collection of units given by OverrideCache
	if ( OverrideCache != NULL && OverrideCache->xCell == tmpX && OverrideCache->yCell == tmpZ && ( CalcThreatForEnemy == OverrideCacheFriendly ) )
	{
		for ( map<int, pair<int, bool> >::iterator it = OverrideCache->armorTypesInCell.begin() ; it != OverrideCache->armorTypesInCell.end() ; it++ )
		{
			value += CalculateDPS( it->first, it->second.first, it->second.second );
		}
		//ai->utility->ChatMsg("TM: Total Value: %f", value );
	}
	else	//If we are to calculate threat for the given weapondef against the units that are actually there
	{
		//Key: armortype. Val: number of units with that armortype, bool for air units/ground units.
		bool cacheFound = false;
		map<int, pair<int, bool> > armorTypesInCell;

		if ( CalcThreatForEnemy )//Calc threat for enemy units against friendly units
		{
			for ( int i = 0 ; i < FriendlyUnitInfoCache.size() ; i++ )
			{
				if ( FriendlyUnitInfoCache[i]->xCell == tmpX && FriendlyUnitInfoCache[i]->yCell == tmpZ )
				{
					armorTypesInCell = FriendlyUnitInfoCache[i]->armorTypesInCell;
					cacheFound = true;
					break;
				}
			}
		}
		else //Calc threat for friendly units against enemy units
		{
			for ( int i = 0 ; i < EnemyUnitInfoCache.size() ; i++ )
			{
				if ( EnemyUnitInfoCache[i]->xCell == tmpX && EnemyUnitInfoCache[i]->yCell == tmpZ )
				{
					armorTypesInCell = EnemyUnitInfoCache[i]->armorTypesInCell;
					cacheFound = true;
				}
			}
		}
		if ( !cacheFound )
		{
			vector<Unit*> tmpUnits;
			if ( CalcThreatForEnemy )
				tmpUnits = ai->knowledge->selfInfo->armyInfo->RangeQuery( tmpX*Resolution, tmpZ*Resolution, tmpX*Resolution+Resolution, tmpZ*Resolution + Resolution );
			else tmpUnits = ai->knowledge->enemyInfo->armyInfo->RangeQuery( tmpX*Resolution, tmpZ*Resolution, tmpX*Resolution+Resolution, tmpZ*Resolution + Resolution );
			//Find out if there are any null-defs. If yes, remove them
			//For each distinct armortype in the group, count how many units got it
			//NOTE: this info isn't actually used yet
			UnitInfoCache* cache = MakeUnitInfoCache(tmpUnits, tmpX, tmpZ );
			if ( CalcThreatForEnemy )
				FriendlyUnitInfoCache.push_back( cache );
			else
				EnemyUnitInfoCache.push_back( cache );

		}
		//Find out if there are any units in the specified area
		//In case no, don't add any threat
		//In case yes, calculate the DPS
		if ( armorTypesInCell.size() > 0 )
		{
			for ( map<int, pair<int, bool> >::iterator it = armorTypesInCell.begin() ; it != armorTypesInCell.end() ; it++ )
			{
				value += CalculateDPS( it->first, it->second.first, it->second.second );
			}
		}
		else return;
	}
	//if ( value > 0 )
	//	ai->utility->ChatMsg("Adding threat: %f. For enemy: %d. At (%d,%d)", value, CalcThreatForEnemy, tmpX, tmpZ );
	if ( CalcThreatForEnemy )
		MapArray[index] += value;
	else OurThreat[index] += value;
}

///@return the DPS at a given point
Superiority ThreatMap::GetSuperiorityAtPos( SAIFloat3 pos, bool debug)
{
	float enemyDamage = GetDamageAtPos( pos, true );
	float ourDamage = GetDamageAtPos( pos, false );
	//ai->utility->ChatMsg("TM: GetSuperiority: EnemyDam: %f. OurDam: %f. (%f,%f)", enemyDamage, ourDamage, pos.x, pos.z );

	if ( enemyDamage == 0.0f )
	{
		//if ( ourDamage > EQUALITY_LIMIT )
		//	return SUPERIOR;
		//else
		//{
			return NO_ENEMY_THREAT;
		//}
	}
	else if ( ourDamage == 0.0f )
	{
		//ai->utility->ChatMsg("TM: OurDamage zero!");
		//if ( enemyDamage > EQUALITY_LIMIT )
			return INFERIOR;
	}
	else
	{
		//ai->utility->ChatMsg("TM: None zero!");
		float d = ourDamage/enemyDamage;
		if ( d > 1.25f && ourDamage > EQUALITY_LIMIT )
			return SUPERIOR;
		if ( d < 0.75f && enemyDamage > EQUALITY_LIMIT )
			return INFERIOR;
	}
	return EQUAL;
}

bool ThreatMap::CanHit( WeaponMount* mount, bool air )
{
	if ( air )
		return ai->utility->CanHit( mount, ReferenceUnitDefAir );
	return ai->utility->CanHit( mount, ReferenceUnitDefGround );
}


float ThreatMap::GetDamageAtPos( SAIFloat3 pos, bool enemy )
{
	float totalThreat = 0.0f;
	int tiles = 0;
	for ( int i = -1 ; i <= 1 ; i++ )
	{
		float deltaThreat = 0.0f;
		for ( int j = -1 ; j <= 1 ; j++ )
		{
			SAIFloat3 newPos = pos;
			newPos.x += i*Resolution;
			if ( newPos.x < 0 || newPos.x > MapWidth*Resolution )
				continue;

			newPos.z += j*Resolution;
			if ( newPos.z < 0 || newPos.z > MapHeight*Resolution )
				continue;

			tiles++;
			int index = (int)(floorf(newPos.z/Resolution)*MapWidth + floorf(newPos.x/Resolution));

			if ( enemy )
				deltaThreat = MapArray[index];
			else deltaThreat = OurThreat[index];
			totalThreat += deltaThreat;
		}
	}
	return totalThreat/(float)tiles;
}

Superiority ThreatMap::GetImaginarySuperiorityAtPos(SAIFloat3 pos, brainSpace::MilitaryUnitGroup *group, bool enemyUnits, bool debug )
{	
	float* backUp = new float[MapWidth*MapHeight];

	for ( int j = 0 ; j < MapWidth*MapHeight ; j++ )
	{
		if ( enemyUnits )
		{
			backUp[j] = MapArray[j];
		}
		else backUp[j] = OurThreat[j];
	}

	vector<int> uIDs = group->GetUnits();
	vector<Unit*> groupUnits;
	for ( int i = 0 ; i < uIDs.size() ; i++ )
		groupUnits.push_back( Unit::GetInstance( ai->callback, uIDs[i] ) );

	int x = pos.x/Resolution;
	int z = pos.z/Resolution;
	float TLX = x*Resolution;
	float TLZ = z*Resolution;
	float BRX = x*Resolution+Resolution;
	float BRZ = z*Resolution+Resolution;

	vector<Unit*> unitsInTile;

	if ( enemyUnits )
	{
		unitsInTile = ai->knowledge->enemyInfo->armyInfo->RangeQuery( TLX, TLZ, BRX, BRZ );
	}
	else
	{
		unitsInTile = ai->knowledge->selfInfo->armyInfo->RangeQuery( TLX, TLZ, BRX, BRZ );
	}

	for ( int i = 0 ; i < unitsInTile.size() ; i++ )
	{
		for ( int j = 0 ; j < group->GetSize() ; j++ )
		{
			if ( unitsInTile[i]->GetUnitId() == group->GetUnits()[j] )
			{
				//ai->utility->ChatMsg("TM: Same unit!! Erasing index %d", i);
				unitsInTile.erase( unitsInTile.begin() + i );
				i--;
				//ai->utility->ChatMsg("TM: New index %d. New size; %d", i, unitsInTile.size() );
				break;
			}
		}
	}


	for ( int i = 0 ; i < groupUnits.size() ; i++ )
		unitsInTile.push_back( groupUnits[i] );

	bool makeNewCache = true;
	if ( OverrideCache != NULL )
	{
		if ( OverrideCache->xCell == x && OverrideCache->yCell == z )
		{
			if ( unitsInTile.size() == OverrideCache->units.size() )
			{
				for ( int i = 0 ; i < unitsInTile.size() ; i++ )
				{
					bool found = false;
					for ( int j = 0 ; j < OverrideCache->units.size() ; j++ )
					{
						if ( unitsInTile[i]->GetUnitId() == OverrideCache->units[j] )
						{
							found = true;
							break;
						}
					}
					if ( !found )
					{
						break;//one of the units in the tile is not in the OverrideCache so we must make a new one, break from the loop for optimisation
					}
					if ( found && i == OverrideCache->units.size()-1 )
					{
						//we found all units, so recycle the cache
						makeNewCache = false;
					}
				}
			}
		}
	}
	if ( makeNewCache )
	{
		//ai->utility->ChatMsg("Making new cache for %d and %d", x, z);
		if ( OverrideCache != NULL )
			delete OverrideCache;
		OverrideCache = MakeUnitInfoCache( unitsInTile, x, z );
		OverrideCacheFriendly = !enemyUnits;

		//ai->utility->ChatMsg("TM: TMTile (%d,%d). GroupPos (%f,%f). TMPos (%f,%f). Total units in tile: %d", x, z, group->GetPos().x, group->GetPos().z, pos.x, pos.z,  unitsInTile.size() );
		Update();
	}

	Superiority s = GetSuperiorityAtPos(pos, debug);
	for ( int j = 0 ; j < MapWidth*MapHeight ; j++ )
	{
		if ( enemyUnits )
		{
			MapArray[j] = backUp[j];
		}
		else OurThreat[j] = backUp[j];
	}
	delete[] backUp;
	for ( int i = 0 ; i < groupUnits.size() ; i++ )
		delete groupUnits[i];
	return s;
}

UnitInfoCache* ThreatMap::MakeUnitInfoCache( std::vector<springai::Unit*> units, int x, int z )
{
	map<int, pair<int, bool> > armorTypesInCell;

	for ( int i = 0 ; i < units.size() ; i++ )
	{
		UnitDef* d = units[i]->GetDef();
		if ( d == NULL )
		{
			ai->utility->ChatMsg("TM: NULLDEF");
			if ( CalcThreatForEnemy )
				ai->knowledge->selfInfo->armyInfo->RemoveUnit(units[i]->GetUnitId());
			else ai->knowledge->enemyInfo->armyInfo->RemoveUnit(units[i]->GetUnitId());
			delete d;
			continue;
		}
		int newCount = armorTypesInCell[d->GetArmorType()].first;
		if ( newCount == 0 )
		{
			armorTypesInCell[d->GetArmorType()].second = d->IsAbleToFly();
		}
		else
		{
			if ( d->IsAbleToFly() != armorTypesInCell[d->GetArmorType()].second )
				ai->utility->ChatMsg("Error! Some units with the same armor type can fly, and some cannot!");
		}
		newCount++;
		armorTypesInCell[d->GetArmorType()].first = newCount;
		delete d;
	}
	std::vector<int> a;
	for ( int i = 0 ; i < units.size() ; i++ )
	{
		a.push_back( units[i]->GetUnitId() );
	}
	return new UnitInfoCache( a, armorTypesInCell, x, z );
}