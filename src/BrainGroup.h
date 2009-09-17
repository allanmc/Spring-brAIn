#ifndef _BRAINSPACE_GROUP_H
#define _BRAINSPACE_GROUP_H

#include <string>
#include <vector>
#include <set>
#include "Unit.h"
#include "UnitDef.h"

using namespace std;
using namespace springai;

namespace brainSpace
{
	class BrainGroup
	{
	public:
		BrainGroup( string name );
		virtual ~BrainGroup();

		void AddUnit( Unit* unit );
		void RemoveUnit( Unit* unit );
		int GetSize();
		bool IsIdle();


	protected:
		string GroupName;
		vector<Unit*> Units;
		bool Idle;
	};
}

using namespace brainSpace;

#endif
#ifndef _BRAINSPACE_GROUP_H
#define _BRAINSPACE_GROUP_H

#include <string>
#include <set>
#include "Unit.h"

using namespace std;
using namespace springai;


namespace brainSpace
{
	class Group
	{
	public:
		Group();
		virtual ~Group();

		void AddUnitToGroup( Unit* unit );
		void RemoveUnitFromGroup( Unit* unit );


	private:
		string GroupName;
		int GroupSize;

		
	};
}
#endif