#ifndef _BRAINSPACE_GROUP_H
#define _BRAINSPACE_GROUP_H

#include "types.h"
#include "KMedoids.h"

using namespace std;
using namespace springai;

namespace brainSpace
{
	///A group of units, used for both combat and construction units.
	class BrainGroup
	{
	public:
		BrainGroup( AIClasses* aiClasses, int groupID );
		virtual ~BrainGroup();

		///Adds a unit to the group.
		virtual bool AddUnit( Unit* unit ) = 0;

		///Removes a unit from the group.
		void RemoveUnit( Unit* unit );

		///Get the size of the group.
		int GetSize();

		///Get whether all units in the group are idle.
		bool IsIdle();
		///Get the position of all units within a group. Calculated using the K-medoids method.
		SAIFloat3 GetPos();

		///Gets the GroupID. IDs are only unique for groups of the same type (construction or military).
		int GetGroupID();
		
		vector<int> GetUnits();

		bool Contains( int unitID );
	protected:
		///int = unitID. The boolean flag indicates if the unit is idle.
		map<int, bool> Units;

		///Is the group idle?
		bool Idle;

		///Svend!
		AIClasses* ai;

		///The ID of the group.
		int GroupID;
	private:
		
		//If we get asked for our position several times during one frame, return the cached info
		unsigned int LastFrameGetPos;
		vector< vector< SAIFloat3 > > CachedPosition;
	};
}

using namespace brainSpace;

#endif
