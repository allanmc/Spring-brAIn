#ifndef _BRAINSPACE_DECISION_H
#define _BRAINSPACE_DECISION_H

#include "GroupController.h"
#include "BuildingController.h"

namespace brainSpace {

class Decision
{
public:
	GroupController * gc;
	BuildingController * bc;

	Decision(void);
	~Decision(void);
};

}

#endif // _BRAINSPACE_DECISION_H
