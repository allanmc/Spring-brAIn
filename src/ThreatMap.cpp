#include "ThreatMap.h"


ThreatMap::ThreatMap( AIClasses* aiClasses )
{
	ai = aiClasses;
	ThreatMapResolution = 64;

	ThreatMapWidth = (ai->callback->GetMap()->GetWidth())/ThreatMapResolution;
	ThreatMapHeight = (ai->callback->GetMap()->GetHeight())/ThreatMapResolution;
	ThreatArray = new float[ThreatMapWidth*ThreatMapHeight];
	FigureIDs = new int[ThreatMapWidth*ThreatMapHeight];
	GridFigureIDs = new int[ThreatMapWidth + ThreatMapHeight];

	DrawGrid();
	Reset();

	ArmorType = -1;
}


ThreatMap::~ThreatMap()
{
}

void ThreatMap::Update()
{
	bool printArray = false;
	map<int, SAIFloat3> enemyUnits = ai->knowledge->enemyInfo->armyInfo->GetEnemyUnits();
	Reset();

	for ( map<int, SAIFloat3>::iterator it = enemyUnits.begin() ; it != enemyUnits.end() ; it++ )
	{
		InsertUnit( Unit::GetInstance( ai->callback, it->first ), it->second );
	}

	for ( int i = 0 ; i < ThreatMapHeight*ThreatMapWidth ; i++ )
	{
		ai->utility->RemoveGraphics( FigureIDs[i] );
		ai->utility->Log( DEBUG, KNOWLEDGE, "ThreatArray %d, %f", i, ThreatArray[i]);
	}


	DrawGrid();
	for ( int i = 0 ; i < ThreatMapWidth ; i++ )
		for ( int j = 0 ; j < ThreatMapHeight ; j++ )
		{
			if ( ThreatArray[ j*ThreatMapWidth + i ] != 0 )
			{
				SAIFloat3 start, end;
				start.x = i*8*ThreatMapResolution;
				start.y = 200;
				start.z = j*8*ThreatMapResolution + 0.5*8*ThreatMapResolution;

				end.x = (i+1)*8*ThreatMapResolution;
				end.y = 200;
				end.z = j*8*ThreatMapResolution + 0.5*8*ThreatMapResolution;

				FigureIDs[ j*ThreatMapWidth + i ] = ai->utility->DrawLine( start, end, false, 8*ThreatMapResolution*2 );
				printArray = true;
			}
		}

		if ( printArray )
			for ( int i = 0 ; i < ThreatMapHeight*ThreatMapWidth ; i++ )
			{
				ai->utility->Log( DEBUG, KNOWLEDGE, "ThreatArray %d, %f", i, ThreatArray[i]);
			}
}

void ThreatMap::InsertUnit(Unit *u, SAIFloat3 pos )
{
	if ( ArmorType == -1 )
		ArmorType = u->GetDef()->GetArmorType();

	//HUSK AT GEMME UNITDEF SAMMEN MED UNITID I QUADTREE. EVT INKLUDER DET SAMMEN MED POSITION I EN STRUCT
	if ( u->GetDef()->GetUnitDefId() == -1 )
		return;

	vector<WeaponMount*> weaponMounts = u->GetDef()->GetWeaponMounts();
	//ai->utility->Log( DEBUG, KNOWLEDGE, "Weapon mounts size: %d", weaponMounts.size() );
	for ( int i = 0 ; i < weaponMounts.size() ; i++ )
	{
		WeaponDef* def = weaponMounts.at(i)->GetWeaponDef();

		int currentIndexX = (int)(pos.x/8)/ThreatMapResolution;
		int currentIndexZ = (int)(pos.z/8)/ThreatMapResolution;
		int range = ceilf(def->GetRange()/8/ThreatMapResolution );
		float realRange = def->GetRange();

		for ( int i = currentIndexX-range ; i < currentIndexX+range ; i++ )
			for ( int j = currentIndexZ-range ; j < currentIndexZ+range ; j++ )
			{
				if ( i < 0 || j < 0 || i >= ThreatMapWidth || j >= ThreatMapHeight )
					continue;
				CBoundingBox b;
				b.topLeft.x = i*8*ThreatMapResolution;
				b.topLeft.z = j*8*ThreatMapResolution;
				b.bottomRight.x = (i+1)*8*ThreatMapResolution;
				b.bottomRight.z = (j+1)*8*ThreatMapResolution;
				if ( ai->math->CircleIntersectBoundingBox( b, pos, realRange ) )
				{

					//ai->utility->Log( DEBUG, KNOWLEDGE, "About to write to array i: %d, j: %d, index: %d, ThreatMapWidth %d, ThreatMapHeight %d", i, j, (j*ThreatMapWidth + i), ThreatMapWidth, ThreatMapHeight );
					ThreatArray[ j*ThreatMapWidth + i ] += CalculateDPS( def );
					//ai->utility->Log( DEBUG, KNOWLEDGE, "Wrote to array" );
				}
			}
	}
}

int ThreatMap::CalculateDPS( WeaponDef* w )
{
	return ( w->GetDamage()->GetTypes().at(ArmorType) / w->GetReload() );
}

void ThreatMap::Reset()
{
	for ( int i = 0 ; i < ThreatMapWidth*ThreatMapHeight ; i++ )
		ThreatArray[i] = 0;
}

void ThreatMap::DrawGrid()
{
	for ( int i = 0 ; i < ThreatMapHeight ; i++ )
	{
		//ai->utility->RemoveGraphics( GridFigureIDs[i]);
		SAIFloat3 start, end;
		start.x = 0;
		start.y = 200;
		start.z = i*8*ThreatMapResolution;
		end.x = ThreatMapWidth*8*ThreatMapResolution;
		end.y = 200;
		end.z = i*8*ThreatMapResolution;
		int id = ai->utility->DrawLine( start, end );
		GridFigureIDs[i] = id;
		//ai->utility->Log( DEBUG, KNOWLEDGE, "New figure id %d, array id %d. Start: (%f, %f ) End: (%f, %f)", id, GridFigureIDs[i], start.x, start.z, end.x, end.z );
	}

	for ( int i = 0 ; i < ThreatMapWidth ; i++ )
	{		
		//ai->utility->RemoveGraphics( GridFigureIDs[i+ThreatMapHeight]);
		SAIFloat3 start, end;
		start.x = i*8*ThreatMapResolution;
		start.y = 200;
		start.z = 0;
		end.x = i*8*ThreatMapResolution;
		end.y = 200;
		end.z = ThreatMapWidth*8*ThreatMapResolution;

		int id = ai->utility->DrawLine( start, end );
		GridFigureIDs[i+ThreatMapHeight] = id;
		//ai->utility->Log( DEBUG, KNOWLEDGE, "New figure id %d, array id %d Start: (%f, %f ) End: (%f, %f)", id , GridFigureIDs[i+ThreatMapHeight], start.x, start.z, end.x, end.z );
	}
}