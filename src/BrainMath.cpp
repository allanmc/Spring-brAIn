#include "BrainMath.h"


bool BrainMath::CircleIntersectBoundingBox( CBoundingBox box, SAIFloat3 center, float radius )
{

	CBoundingBox circleBoundingbox;

	circleBoundingbox.topLeft.x = center.x - radius;
	circleBoundingbox.topLeft.z = center.z - radius;
	circleBoundingbox.bottomRight.x = center.x + radius;
	circleBoundingbox.bottomRight.z = center.z + radius;

	if ( !circleBoundingbox.Intersects( box ) )
		return false;

	SAIFloat3 temp;
	if ( box.bottomRight.x < center.x )//To the right
		temp.x = box.bottomRight.x;
	else if ( box.topLeft.x > center.x )
		temp.x = box.topLeft.x;
	else
	{
		return true;
	}

	if ( box.bottomRight.z < center.z )
		temp.z = box.bottomRight.z;
	else if ( box.topLeft.z > center.z )
		temp.z = box.topLeft.z;
	else
	{
		return true;
	}

	return ( ai->utility->EuclideanDistance( center, temp ) < radius );
}