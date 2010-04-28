#ifndef _BRAINSPACE_THREATMAP_H
#define _BRAINSPACE_THREATMAP_H


#include "types.h"
#include "BrainMap.h"

#include "WeaponMount.h"
#include "WeaponDef.h"
#include "Damage.h"
#include "MilitaryUnitGroup.h"

#define EQUALITY_LIMIT 15.0f
namespace brainSpace {

	struct UnitInformationContainer;

	typedef struct UnitInfoCache
	{
		std::vector<int> units;
		//Key: armortype. Val: pair of: number of units with that type, flag for indicating air units (true), or ground units (false)
		std::map<int,std::pair<int, bool> > armorTypesInCell;
		int xCell, yCell;

		UnitInfoCache( std::vector<int> u, std::map<int,std::pair<int, bool> > m, int x, int y )
		{
			units = u;
			armorTypesInCell = m;
			xCell = x;
			yCell = y;
		}

		~UnitInfoCache()
		{
			units.clear();
			armorTypesInCell.clear();
		}
	}UnitInfoCache;


	enum Superiority
	{
		NO_ENEMY_THREAT, INFERIOR, EQUAL, SUPERIOR
	};

	///A map keeping information the estimated threat at a given cell.
	/**
	* The threat is calculated from enemies last know position, and the the value is in total DPS.
	*/
	class ThreatMap : public BrainMap
	{
	public:
		ThreatMap( AIClasses* aiClasses );
	virtual ~ThreatMap();
	void Update();

	Superiority GetSuperiorityAtPos( SAIFloat3 pos, bool debug = false );

	//Our superiority at the specified position if we imagine that we move the unitgroup to that position
	//The bool indicates if the group is friendly or enemy
	Superiority GetImaginarySuperiorityAtPos( SAIFloat3 pos, MilitaryUnitGroup* group, bool enemyUnits, bool debug = false );

	float GetDamageAtPos( SAIFloat3 pos, bool enemy );
private:

	/**
	 * UGLY but useful variable: used when updating the threat map so that EffectCell() 
	 * can see the WeaponDef when called from EffectCircle()
	 **/
	springai::WeaponDef* CurrentWeaponDef; 
	bool CurrentWeaponDefCanHitGround;
	bool CurrentWeaponDefCanHitAir;

	//true: Calc threat for enemy units against friendly units
	bool CalcThreatForEnemy;
	float* OurThreat;

	/*
	 * Used to override the default behaviour of effectcell, 
	 * so that we can specify what types of units either the enemy or we have in a tile
	 */
	UnitInfoCache* OverrideCache;
	//True if the OverrideCache contains friendly units
	bool OverrideCacheFriendly;

	//Caches for containing info of the units in the different cells of the map
	std::vector<UnitInfoCache*> FriendlyUnitInfoCache;
	std::vector<UnitInfoCache*> EnemyUnitInfoCache;
	
	springai::UnitDef* ReferenceUnitDefAir;
	springai::UnitDef* ReferenceUnitDefGround;


	bool CanHit( springai::WeaponMount* mount, bool air );
	void EffectCell(int index, float value);
	void InsertUnit( springai::Unit* u, UnitInformationContainer c );
	float CalculateDPS( int armorType, int numberOfUnitsWithThatArmorType, bool airUnits );
	UnitInfoCache* MakeUnitInfoCache( std::vector<springai::Unit*> units, int x, int z );
	};
}
#endif