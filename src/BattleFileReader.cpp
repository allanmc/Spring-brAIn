#include "BattleFileReader.h"
#include <sstream>

using namespace std;
using namespace springai;
using namespace brainSpace;

BattleFileReader::BattleFileReader(AIClasses* aiClasses)
{
	ai = aiClasses;
	DataDirs *dirs = DataDirs::GetInstance(ai->callback);
	stringstream file_name;
	file_name.str("");
	file_name << dirs->GetWriteableDir();
	file_name << "battles.dat";
	bfs = new fstream ( file_name.str().c_str() , ios::in | ios::binary);
	delete dirs;




	vector<int> ground;
	vector<int> air;
	ground.push_back(120);	//0 - armpw
	ground.push_back(127);	//1 - armrock
	ground.push_back(82);	//2 - armham
	ground.push_back(89);	//3 - armjeth
	ground.push_back(173);	//4 - armwar
	ground.push_back(61);	//5 - armfav
	ground.push_back(70);	//6 - armflash
	ground.push_back(116);	//7 - armpincer
	ground.push_back(151);	//8 - armstump
	ground.push_back(368);	//9 - tawf013
	ground.push_back(88);	//10 - armjanus
	ground.push_back(130);	//11 - armsam
	ground.push_back(-1);	//12 - nullground

	//air.push_back("armpeep");
	air.push_back(-2);	//0 - nullair
	air.push_back(68);	//1 - armfig
	air.push_back(157);	//2 - armthund
	air.push_back(90);	//3 - armkam
	units_all.push_back(ground);
	units_all.push_back(air);

}

BattleFileReader::~BattleFileReader()
{
	bfs->close();
	delete bfs;
}


float BattleFileReader::ReadBattleValue(MilitaryUnitGroup* friendly, MilitaryUnitGroup* enemy)
{
	int game = GetClosestGame( friendly, enemy );
	float value;
	bfs->seekg (game*sizeof(value), ios::beg);
	bfs->read( (char*)&value, sizeof(value) );
	return value;
}


int BattleFileReader::GetClosestGame( MilitaryUnitGroup* friendlyMiliGroup, MilitaryUnitGroup* enemyMiliGroup )
{
	vector<int> friendlyIDs = friendlyMiliGroup->GetUnits();
	vector<int> enemyIDs = friendlyMiliGroup->GetUnits();
	vector<Unit*> friendlies;
	vector<Unit*> enemies;

	for ( unsigned int i = 0 ; i < friendlyIDs.size() ; i++ )
	{
		Unit* u = Unit::GetInstance(ai->callback, friendlyIDs[i] );
		friendlies.push_back(u);
		delete u;
	}
	for ( unsigned int i = 0 ; i < enemyIDs.size() ; i++ )
	{
		Unit* u = Unit::GetInstance(ai->callback, enemyIDs[i] );
		enemies.push_back(u);
		delete u;
	}

	RemoveCommander(friendlies);
	RemoveCommander(enemies);
	int num_friendlies = friendlies.size();
	int num_enemies = enemies.size();

	vector<int> friendlyUnits;
	vector<int> enemyUnits;
	int friendlyUnitType = CountGroupUnits(friendlies, friendlyUnits, friendlyMiliGroup->IsAirGroup() );
	int enemyUnitType = CountGroupUnits(enemies, enemyUnits, enemyMiliGroup->IsAirGroup());

	//Make the unit counts of the two teams match, by adding null-units
	int diff = num_friendlies - num_enemies;
	if ( diff > 0 )
	{
		enemyUnits[GetNullId(enemyUnitType)] = diff;
		num_enemies += diff; 
	}
	else
	{
		//Always ensure we have one null-unit, so that no group of size 0 can get a slot
		enemyUnits[GetNullId(enemyUnitType)] = 1;
	}

	if ( diff < 0 )
	{
		friendlyUnits[GetNullId(friendlyUnitType)] = -1*diff;
		num_friendlies -= diff; 
	}
	else
	{
		//Always ensure we have one null-unit, so that no group of size 0 can get a slot
		friendlyUnits[GetNullId(friendlyUnitType)] = 1;
	}	

	int *friendlyGroup = GetGroupFromUnits(friendlyUnits);
	int *enemyGroup = GetGroupFromUnits(enemyUnits);


	int friendlyId = GetId(friendlyUnitType, friendlyGroup[0], friendlyGroup[1], friendlyGroup[2]);
	int enemiesId = GetId(enemyUnitType, enemyGroup[0], enemyGroup[1], enemyGroup[2]);

	int game = enemiesId * sqrt(TOTAL_NUMBER_OF_GAMES) + friendlyId;

	delete[] friendlyGroup;
	delete[] enemyGroup;

	return game;
}

int* BattleFileReader::GetGroupFromUnits(vector<int> units)
{
	list<pair<int, int> > best;
	list<pair<int, int> >::iterator it;

	for( unsigned int i=0 ; i < units.size() ; i++ )
	{
		bool inserted = false;
		it = best.begin();
		while( it != best.end() )
		{
			if( (*it).second < units[i] )
			{
				best.insert(it, pair<int, int>(i, units[i]));
				inserted = true;
				break;
			}
			it++;
		}
		if( !inserted )
		{
			best.push_back(pair<int,int>(i, units[i]));
		}
	}

	int *outUnits = new int[3];
	it = best.begin();
	int size = (*it).second + (*(++it)).second + (*(++it)).second;
	it = best.begin();
	outUnits[0] = (*it).first;
	//ai->utility->ChatMsg("Group #1 set to: %d, count = %d", outUnits[0], (*(it)).second);
	float ratio = (float)(*it).second / (float)size;
	//ai->utility->ChatMsg("Ratio: %f", ratio);
	if( ratio > 0.5 )
	{
		outUnits[1] = (*it).first;
		//ai->utility->ChatMsg("Group #2 set to: %d, count = %d", outUnits[1], (*(it)).second);
		if( ratio > 0.8333 )
		{
			outUnits[2] = (*it).first;
		}
		else
		{
			outUnits[2] = (*(++it)).first;
		}
	}
	else
	{

		outUnits[1] = (*(++it)).first;
		//ai->utility->ChatMsg("Group #2 set to: %d, count = %d", outUnits[1], (*(it)).second);
		outUnits[2] = (*(++it)).first;
	}
	//ai->utility->ChatMsg("Group #3 set to: %d, count = %d", outUnits[2], (*(it)).second);

	best.clear();

	return outUnits;
}

void BattleFileReader::RemoveCommander(vector<Unit*> &units)
{
	for ( unsigned int i = 0 ; i < units.size() ; i++ )
	{
		UnitDef *d = units[i]->GetDef();
		if(d->IsCommander())
		{
			delete units[i];
			units.erase(units.begin() + i);
			delete d;
			break;
		}
		delete d;
	}
}



int BattleFileReader::GetId(int unitType, int unit1, int unit2, int unit3) 
{
	int id = 0;

	//Fix input
	if (unit1 > unit2)
	{
		swap(unit1, unit2);
	}
	if (unit1 > unit3)
	{
		swap(unit1,unit3);
	}
	if ( unit2 > unit3 )
	{
		swap(unit2,unit3);
	}

	//unit1
	int num_units = units_all[unitType].size();
	int max = num_units ;
	for (int i = 0; i < unit1; i++) {
		id += SumRange(max - i, 1, max - i);
	}
	//unit2
	id += SumRange(num_units - unit1, num_units - unit2 + 1, unit2-unit1 );
	//unit3
	id += unit3 - unit2;

	return id;
}


int BattleFileReader::SumRange(int max, int min, int num)
{
	return (max + min)*num/2;
}

int BattleFileReader::GetUnitId(int unitType, const char* name)
{
	return GetUnitId(unitType, ai->utility->GetUnitDef(name)->GetUnitDefId());
}

int BattleFileReader::GetNullId(int unitType)
{
	for( unsigned int j = 0 ; j < units_all[unitType].size() ; j++ )
	{
		if( units_all[unitType][j] < 0 ) //Null-units are negative (-1, -2)
		{
			return j;
		}
	}
	return -1;
}


//Return unitType, and populates `result`
int BattleFileReader::CountGroupUnits(vector<Unit*> units_old, vector<int> &result, bool airGroup)
{
	int unitType = ( airGroup ) ? 1 : 0;
	
	result.clear();
	for( unsigned int i = 0 ; i < units_all.size() ; i++ )
	{
		result.push_back(0);
	}

	//fill vectors
	for( unsigned int i=0 ; i < units_old.size() ; i++ )
	{
		UnitDef* ud = units_old[i]->GetDef();

		for( unsigned int j = 0 ; j < units_all[unitType].size() ; j++ )
		{
			if( ud->GetUnitDefId() == units_all[unitType][j] )
			{
				//ai->utility->ChatMsg("Added unit: (%d) %s", j, ud->GetName());
				result[j]++;
				break;
			}
		}
		delete ud;
	}
	return unitType;
}

int BattleFileReader::GetUnitId(int unitType, int defId )
{
	for( unsigned int j = 0 ; j < units_all[unitType].size() ; j++ )
	{
		if( units_all[unitType][j] == defId )
		{
			return j;
		}
	}
	return -1;
}