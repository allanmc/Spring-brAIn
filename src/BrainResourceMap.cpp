#include "BrainResourceMap.h"

using namespace brainSpace;
using namespace std;

BrainResourceMap::BrainResourceMap( AIClasses* aiClasses ):BrainMap(aiClasses, 2)
{
}


BrainResourceMap::~BrainResourceMap()
{
}

void BrainResourceMap::Update()
{
	SAIFloat3 dummy;

	vector<SAIFloat3> spots = ai->callback->GetMap()->GetResourceMapSpotsPositions( *(ai->utility->GetResource("Metal")), &dummy );
	for ( int i = 0 ; i < (int)spots.size() ; i++ )
	{
		int MapX, MapZ;
		MapX = (int)floorf(spots[i].x/Resolution);
		MapZ = (int)floorf(spots[i].z/Resolution);
		MapArray[MapZ*MapWidth + MapX ] += 1;
	}
}

void BrainResourceMap::EffectCell( int index, float value )
{
}