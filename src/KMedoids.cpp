#include "KMedoids.h"

using namespace std;
using namespace springai;
using namespace brainSpace;

KMedoids::KMedoids( AIClasses* aiClasses )
{
	ai = aiClasses;
}

KMedoids::~KMedoids()
{

}

void KMedoids::Clear()
{
	points.clear();
}

void KMedoids::AddPoints(vector<SAIFloat3> newPoints)
{
	for ( int i = 0; i < (int)newPoints.size(); i++ )
	{
		points.push_back((KPoint){newPoints[i], 0, points.size()});
	}
}

vector< vector<SAIFloat3> > KMedoids::GetConvexHulls()
{
	return GetConvexHulls( (unsigned short)min( (int)(sqrt(points.size())), 10) );
}

vector< vector<SAIFloat3> > KMedoids::GetConvexHulls( unsigned short numClusters )
{
	vector< vector<SAIFloat3> > clusters = GetClusters( numClusters );
	vector< SAIFloat3 > points;
	vector< SAIFloat3 > result;
	int num;
	ConvexHull *ch = new ConvexHull( ai );

	for (int c = 0; c < (int)clusters.size(); c++ )
	{	
		points.clear();//Is it necesarry?
		points = ai->math->Sort(clusters[c]);
		SAIFloat3 pointsA[points.size()];
		SAIFloat3 resultA[points.size()];
		for (int i = 0; i < (int)points.size(); i++) //Sorry for doing vector->array->vector :)
		{
			pointsA[i] = points[i];
		}
		
		num = ch->chainHull_2D(pointsA, points.size(), resultA);
		clusters[c].clear();
		for (int i = 0; i < num; i++)
		{
			clusters[c].push_back(resultA[i]);
		}
	}
	return clusters;	
}


vector< vector<SAIFloat3> > KMedoids::GetClusters()
{
	return GetClusters( (unsigned short)min( (int)(sqrt(points.size())), 10) );
}

vector< vector<SAIFloat3> > KMedoids::GetClusters( unsigned short numClusters )
{
	vector< vector<SAIFloat3> > result;
	vector<SAIFloat3> result_temp;
	int random_index = 0;
	float cost_current = -1.0f;
	float cost_temp = 0.0f;
	int swap_best_medoid = 0;
	int swap_best_point = 0;
	bool changed = false;
	int tmp_index = 0;
	vector<int>::iterator it;

	srand((unsigned)time(NULL));
	ai->utility->Log(ALL, KMEDOIDS, "Initializing kmedoids...");

	if (points.size()==0)
	{
		//Nothing to do...
		return result;
	}

	//Step 1 - Initialize: randomly select k of the n data points as the mediods
	medoids.clear();

	/*random_index = 8;
	points[random_index].cluster = -1;
	medoids.push_back(random_index);
	random_index = 9;
	points[random_index].cluster = -1;
	medoids.push_back(random_index);*/
	
	for ( int c = 0; c < numClusters; c++ )
	{
		random_index = (rand()%(points.size()-c));
		while (find(medoids.begin(), medoids.end(), random_index) != medoids.end()) 
		{
			random_index++;
		}
		points[random_index].cluster = -1;
		medoids.push_back(random_index);
	}
	ai->utility->Log(ALL, KMEDOIDS, "Initialized!");
	do 
	{
		changed = false;
		//Step 2 - Associate each data point to the closest medoid.

		cost_current = AssociateDatapoints();

		//Step 3 - For each mediod m
		for ( int c = 0; c < numClusters; c++ )
		{
			//Step 3.1 - For each non-mediod data point o
			for ( int i = 0; i < (int)points.size(); i++ )
			{
				//Skip if current point is a medoid (cluster=-1)
				if (points[i].cluster==-1) continue;
				//Step 3.1.1 - Swap m and o and compute the total cost of the configuration
				tmp_index = medoids[c];
				Swap(i, c);
				cost_temp = AssociateDatapoints();
				Swap(tmp_index, c);
				ai->utility->Log(ALL, KMEDOIDS, "Current temp cost = %f", cost_temp);
				if (cost_temp<cost_current /*|| cost_current<0*/) {
					cost_current = cost_temp;
					swap_best_medoid = c;
					swap_best_point = i;
					ai->utility->Log(ALL, KMEDOIDS, "Found possible best swap between medoid %i (point %i) and point %i, costing: %f", swap_best_medoid, medoids[swap_best_medoid], swap_best_point, cost_current);
					changed = true;
				}
			}
		}
		//Step 4 - Select the configuration with the lowest cost.
		if (changed) {
			ai->utility->Log(ALL, KMEDOIDS, "Best swap is between medoid %i (point %i) and point %i, costing: %f", swap_best_medoid, medoids[swap_best_medoid], swap_best_point, cost_current);
		
			Swap(swap_best_point, swap_best_medoid);
			//cost_current = -1.0f;
		}
		//Step 5 - Repeat steps 2 to 5 until there is no change in the medoid.
	} while( changed );
	ai->utility->Log(ALL, KMEDOIDS, "Done with KMedoids!");
	//Done... i hope? :)

	AssociateDatapoints();

	//Return...
	for ( int c = 0; c < numClusters; c++ )
	{
		result_temp.clear();
		result_temp.push_back(points[medoids[c]].pos);
		for ( int i = 0; i < (int)points.size(); i++ )
		{
			//Skip if current point is a medoid (cluster=-1)
			if (points[i].cluster==-1 || points[i].cluster!=c) continue;
			result_temp.push_back(points[i].pos);
		}
		result.push_back(result_temp);
	}
	return result;
}

float KMedoids::Cost(SAIFloat3 point1, SAIFloat3 point2)
{
	return ai->utility->EuclideanDistance(point1, point2);
}

void KMedoids::Swap(int pointIndex, int medoidIndex, bool print)
{
	if (print) ai->utility->Log(ALL, KMEDOIDS, "--- Points --- ");
	if (print) Print(points);
	if (print) ai->utility->Log(ALL, KMEDOIDS, "--- Medoids --- ");
	if (print) Print(medoids);
	points[medoids[medoidIndex]].cluster = points[pointIndex].cluster;
	points[pointIndex].cluster = -1;
	medoids[medoidIndex] = pointIndex;
	if (print) ai->utility->Log(ALL, KMEDOIDS, "--- Points --- ");
	if (print) Print(points);
	if (print) ai->utility->Log(ALL, KMEDOIDS, "--- Medoids --- ");
	if (print) Print(medoids);
}

void KMedoids::Print(vector<int> medoids)
{
	for ( int i = 0; i < (int)medoids.size(); i++ )
	{
		ai->utility->Log(ALL, KMEDOIDS, "Medoid %i: %i", i, medoids[i]);
	}
}

void KMedoids::Print(vector<KPoint> kpoints)
{
	for ( int i = 0; i < (int)kpoints.size(); i++ )
	{
		ai->utility->Log(ALL, KMEDOIDS, "Point %i: index=%i, cluster=%i", i, kpoints[i].index, kpoints[i].cluster);
	}
}

float KMedoids::AssociateDatapoints()
{
	float total_cost = 0.0f;
	float temp_distance = 0.0f;
	int current_best = 0;
	int numClusters = medoids.size();
	float current_min = 0.0f;
	for ( int i = 0; i < (int)points.size(); i++ )
	{
		//Skip if current point is a medoid (cluster=-1)
		if (points[i].cluster==-1) continue;
		current_best = 0;
		current_min = Cost(points[i].pos, points[medoids[0]].pos);
		//ai->utility->Log(ALL, KMEDOIDS, "Calculated cost from point %i to medoid %i (point %i) = %f", i, 0, medoids[0], current_min);
	
		for ( int c = 1; c < numClusters; c++ )
		{
			temp_distance = Cost(points[i].pos, points[medoids[c]].pos);
			//ai->utility->Log(ALL, KMEDOIDS, "Calculated cost from point %i to medoid %i (point %i) = %f", i, c, medoids[c], temp_distance);
			if (current_min>temp_distance)
			{
				current_best = c;
				current_min = temp_distance;
			}
		}
		//ai->utility->Log(ALL, KMEDOIDS, "Adding %f to total_cost. Before: %f, After: %f", current_min, total_cost, total_cost+current_min);
		//ai->utility->Log(ALL, KMEDOIDS, "To medoid %i (point %i) this point was added: %i", current_best, medoids[current_best], i);
		total_cost += current_min;
		points[i].cluster = current_best;
	}
	return total_cost;
}