#ifndef _BRAINSPACE_KMEDOIDS_H
#define _BRAINSPACE_KMEDOIDS_H

#include "global.h"
#include "ConvexHull.h"
#include "time.h"
#include <algorithm>

namespace brainSpace
{
	struct KPoint {
		SAIFloat3 pos;
		int cluster;
		int index;
	};

	class KMedoids
	{
	public:
		KMedoids( AIClasses* aiClasses );
		virtual ~KMedoids();
		void Clear();
		void AddPoints( vector<SAIFloat3> points );
		vector< vector<SAIFloat3> > GetClusters();
		vector< vector<SAIFloat3> > GetClusters( unsigned short numClusters );
		vector< vector<SAIFloat3> > GetConvexHulls();
		vector< vector<SAIFloat3> > GetConvexHulls( unsigned short numClusters );
	private:
		AIClasses* ai;
		float Cost(SAIFloat3 point1, SAIFloat3 point2);
		float AssociateDatapoints();
		void Swap(int pointIndex, int medoidIndex, bool print=false);
		void Print(vector<KPoint> kpoints);
		void Print(vector<int> medoids);
		vector< KPoint > points;
		vector<int> medoids;
		
	};
}

#endif //_BRAINSPACE_KMEDOIDS_H
