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