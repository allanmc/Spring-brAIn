#include "Path.h"

using namespace brainSpace;

Path::Path( AIClasses* aiClasses, std::vector<SAIFloat3> points )
{
	ai = aiClasses;
	Points = points;
	double length = 0;
	for ( int i = 1 ; i < points.size() ; i++ )
	{
		length += ai->utility->EuclideanDistance( points[i], points[i-1] );
	}
	Length = length;
}


double Path::GetLength()
{
	return Length;
}

std::vector<SAIFloat3> Path::GetPoints()
{
	return Points;
}

bool Path::Aggregate( Path* otherPath )
{
	bool success = false;
	std::vector<SAIFloat3> p = otherPath->GetPoints();
	std::vector<SAIFloat3> newPath;
	if ( Points[0].x == p[p.size()-1].x && Points[0].y == p[p.size()-1].y && Points[0].z == p[p.size()-1].z )
	{
		for ( int i = 0 ; i < p.size(); i++ )
		{
			newPath.push_back( p[i] );
		}
		for ( int i = 0 ; i < Points.size(); i++ )
		{
			newPath.push_back( Points[i] );
		}
		Points = newPath;
		success = true;
	}
	else if  ( Points[Points.size()-1].x == p[0].x && Points[Points.size()-1].y == p[0].y && Points[Points.size()-1].z == p[0].z )
	{
		for ( int i = 0 ; i < Points.size(); i++ )
		{
			newPath.push_back( Points[i] );
		}
		for ( int i = 0 ; i < p.size(); i++ )
		{
			newPath.push_back( p[i] );
		}
		Points = newPath;
		success = true;
	}
	return success;
}