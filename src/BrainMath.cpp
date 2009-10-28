#include "BrainMath.h"

using namespace std;
using namespace springai;
using namespace brainSpace;


static bool cmpSAIFloat3(SAIFloat3 P1, SAIFloat3 P2)
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

vector<SAIFloat3> BrainMath::Sort(vector<SAIFloat3> points)
{
	sort(points.begin(), points.end(), cmpSAIFloat3);
	return points;
}

bool BrainMath::BoxIntersect(SAIFloat3 pos1, float width1, float height1, SAIFloat3 pos2, float width2, float height2)
{
	CBoundingBox box1;
	CBoundingBox box2;
	box1.topLeft.x = pos1.x - width1/2;
	box1.topLeft.z = pos1.z - height1/2;
	box1.topLeft.y = 50;
	box1.bottomRight.x = pos1.x + width1/2;
	box1.bottomRight.z = pos1.z + height1/2;
	box1.bottomRight.y = 50;
	box2.topLeft.x = pos2.x - width2/2;
	box2.topLeft.z = pos2.z - height2/2;
	box2.topLeft.y = 50;
	box2.bottomRight.x = pos2.x + width2/2;
	box2.bottomRight.z = pos2.z + height2/2;
	box2.bottomRight.y = 50;
	return box1.Intersects(box2);
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