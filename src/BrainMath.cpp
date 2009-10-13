#include "BrainMath.h"

using namespace std;
using namespace springai;
using namespace brainSpace;

vector<SAIFloat3> BrainMath::sort(vector<SAIFloat3> points)
{
	sort(points.begin(), points.end(), BrainMath::cmpSAIFloat3);
	return points;
}

bool BrainMath::cmpSAIFloat3 (SAIFloat3 P1, SAIFloat3 P2)
{
	if (P1.x<P2.x)
	{
		return true;
	}
	else if (P1.x==P2.x)
	{
		return (P1.z<P2.z);
	}
	else
	{
		return false;
	}
}

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

bool BrainMath::CircleIntersetCircle(SAIFloat3 center1, float radius1, SAIFloat3 center2, float radius2)
{
	return ( ai->utility->EuclideanDistance( center1, center2 ) < radius1+radius2 );
}