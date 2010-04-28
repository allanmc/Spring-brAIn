#ifndef PATH_H
#define PATH_H

#include "types.h"

namespace brainSpace
{
	class Path
	{
	public:
		Path( AIClasses* aiClasses, std::vector<SAIFloat3> points );
		double GetLength();

		std::vector<SAIFloat3> GetPoints();

		bool Aggregate( Path* otherPath );

	private:
		AIClasses* ai;
		std::vector<SAIFloat3> Points;
		double Length;
	};
}
#endif