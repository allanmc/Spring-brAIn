#include "BaseInfo.h"

using namespace std;
using namespace springai;
using namespace brainSpace;

BaseInfo::BaseInfo( AIClasses* aiClasses )
{
	ai = aiClasses;
	CBoundingBox bbox;
	bbox.topLeft = (SAIFloat3){0,0,0};
	bbox.bottomRight = (SAIFloat3){	ai->callback->GetMap()->GetWidth()*8,
		0,
		ai->callback->GetMap()->GetHeight()*8};
	quadTree = new QuadTree( ai, bbox );
	buildingCount = 0;
	resourceBuildings = 0;
	productionBuildings = 0;
	basePerimiterDrawID = 0;
}

BaseInfo::~BaseInfo()
{
	delete quadTree;
	quadTree = NULL;
}

void BaseInfo::AddBuilding(Unit* building)
{
	UnitDef* d = building->GetDef();
	//check for builder units
	if(d->GetSpeed() > 0)
	{
		delete d;
		d = NULL;
		return;
	}
	delete d;
	d = NULL;
	//ai->utility->Log(ALL, MISC, "Adding base unit...");
	map<int, UnitInformationContainer> units = quadTree->GetUnits();

	if ( units.find(building->GetUnitId()) == units.end())
	{
		quadTree->InsertUnit(building->GetUnitId(), building->GetPos());

		buildingCount++;
		if(building->GetDef()->IsBuilder())
		{
			productionBuildings++;
		}
		else
		{
			resourceBuildings++;
		}

		ai->knowledge->mapInfo->pathfindingMap->AddBuilding( building );

	}
}

void BaseInfo::RemoveBuilding(int building)
{
	if (buildingCount==0)
	{
		return;
	}
	Unit *buildingU = Unit::GetInstance(ai->callback, building);
	UnitDef *def = quadTree->GetUnitDef(building);
	//remove unit from quadtree, using pos
	if(def != NULL)
	{
		buildingCount--;

		if(def->IsBuilder())
		{
			productionBuildings--;
		}
		else
		{
			resourceBuildings--;
		}

		ai->knowledge->mapInfo->pathfindingMap->RemoveBuilding( buildingU );
	}
	else
		ai->utility->Log(ALL,KNOWLEDGE,"RemoveBuilding: We are trying to remove a seen building with no unitdef"); 

	quadTree->RemoveUnit( building );
	delete buildingU;
	buildingU = NULL;
	delete def;
	def = NULL;
}

///@return the number of resource producing buildings
int BaseInfo::CountResourceBuildings()
{
	return resourceBuildings;
}

///@return the number of buildings that are able to contruct other units
int BaseInfo::CountProductionBuildings()
{
	return productionBuildings;
}

void BaseInfo::DrawBasePerimiter()
{
	KMedoids *km = new KMedoids( ai );
	vector<SAIFloat3> points;

	map<int, UnitInformationContainer> units = quadTree->GetUnits();
	map<int, UnitInformationContainer>::iterator iter;

	for ( iter = units.begin() ; iter != units.end() ; iter++ )
	{
		SAIFloat3 pos = (*iter).second.pos;
		points.push_back(pos);
	}

	km->AddPoints(points);

	vector< vector<SAIFloat3> > clusters = km->GetConvexHulls();
	delete km;
	km = NULL;

	if (basePerimiterDrawID>0) 
	{
		ai->utility->RemoveGraphics(basePerimiterDrawID);
	}

	for (int c = 0; c < (int)clusters.size(); c++ )
	{	
		if (clusters[c].size()==2) {
			basePerimiterDrawID = ai->utility->DrawCircle(clusters[c][0], 70, basePerimiterDrawID);
		} else {
			for (int i = 0; i < (int)clusters[c].size() -1 ; i++ )
			{
				basePerimiterDrawID = ai->utility->DrawLine(  clusters[c][i], clusters[c][i+1], true, 20, basePerimiterDrawID );
			}
			//basePerimiterDrawID = ai->utility->DrawLine(  clusters[c][clusters[c].size()-1], clusters[c][0], true, 20, basePerimiterDrawID );
		}
	}

}

vector<Unit*> BaseInfo::GetUnitsInRange(SAIFloat3 pos, float radius)
{
	SAIFloat3 topLeft = pos;
	SAIFloat3 bottomRight = pos;
	topLeft.x -= radius;
	topLeft.z -= radius;
	bottomRight.x += radius;
	bottomRight.z += radius;

	vector<Unit*> units = quadTree->RangeQuery(topLeft, bottomRight);
	return units;
}

bool BaseInfo::IsBuildingInRange(SAIFloat3 pos, float radius)
{
	vector<Unit*> units = GetUnitsInRange(pos, radius);
	if(units.size() > 0)
	{
		return true;
	}
	return false;
}

int BaseInfo::CountBuildingsByName( const char* name )
{
	map<int, UnitInformationContainer> i = quadTree->GetUnits();
	map<int, UnitInformationContainer>::iterator it = i.begin();

	int count = 0;
	while ( it != i.end() )
	{
		if ( strcmp(it->second.def->GetName(), name ) == 0 )
			count++;
		it++;
	}
	return count;
}

vector< vector<SAIFloat3> > BaseInfo::GetBases()
{
	KMedoids *km = new KMedoids( ai );
	vector<SAIFloat3> points;

	map<int, UnitInformationContainer> units = quadTree->GetUnits();
	map<int, UnitInformationContainer>::iterator iter;

	for ( iter = units.begin() ; iter != units.end() ; iter++ )
	{
		SAIFloat3 pos = (*iter).second.pos;
		points.push_back(pos);
	}

	km->AddPoints(points);

	vector< vector<SAIFloat3> > clusters = km->GetClusters();
	delete km;
	km = NULL;
	return clusters;
}

Unit* BaseInfo::GetWeakestBaseBuilding()
{
	vector< vector<SAIFloat3> > bases = GetBases();
	SAIFloat3 startpos = ai->callback->GetMap()->GetStartPos();
	SAIFloat3 best;
	brainSpace::Superiority bestthreat = INFERIOR;

	for(unsigned int i = 0; i < bases.size(); i++)
	{
		for(unsigned int j = 0; j < bases[i].size(); j++)
		{
			Superiority current = ai->knowledge->mapInfo->threatMap->GetSuperiorityAtPos(bases[i][j]);
			if(current > bestthreat)
			{
				best = bases[i][j];
				bestthreat = current;
			}
		}
	}
	vector<Unit*> units = GetUnitsInRange(best, 5);
	if(units.size() < 1)
		return NULL;
	return units[0];
}
Unit* BaseInfo::GetNearestBaseBuilding()
{
	vector< vector<SAIFloat3> > bases = GetBases();
	SAIFloat3 startpos = ai->callback->GetMap()->GetStartPos();
	SAIFloat3 best;
	double bestdist = 99999;
	for(unsigned int i = 0; i < bases.size(); i++)
	{
		for(unsigned int j = 0; j < bases[i].size(); j++)
		{
			double current = ai->utility->EuclideanDistance(bases[i][j], startpos);
			if(current < bestdist)
			{
				best = bases[i][j];
				bestdist = current;
			}
		}
	}
	vector<Unit*> units = GetUnitsInRange(best, 5);
	if(units.size() < 1)
		return NULL;
	return units[0];
}

const map<int, struct UnitInformationContainer> BaseInfo::GetUnits()
{
	return quadTree->GetUnits();
}

UnitDef* BaseInfo::GetUnitDef(int unitID)
{
	return quadTree->GetUnitDef(unitID);
}


vector<Unit*> BaseInfo::RangeQuery(SAIFloat3 topLeft, SAIFloat3 bottomRight)
{
	return quadTree->RangeQuery(topLeft, bottomRight);
}