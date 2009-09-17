#ifndef _BRAINSPACE_GROUPCONTROLLER_H
#define _BRAINSPACE_GROUPCONTROLLER_H



#include "Unit.h"
#include "ConstructionGroupManager.h"
#include "MilitaryGroupManager.h"


using namespace springai;



namespace brainSpace {
	class GroupController
	{
	public:
		GroupController(void);
		~GroupController(void);

		void AddUnit( Unit* unit );
		void RemoveUnit( Unit* unit );


	private:
		ConstructionGroupManager* ConstructionGroupMgr;
		MilitaryGroupManager* MilitaryGroupMgr;

	};
}

#endif // _BRAINSPACE_GROUPCONTROLLER_H
