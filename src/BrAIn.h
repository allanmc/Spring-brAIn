#ifndef _BRAINSPACE_BRAIN_H
#define _BRAINSPACE_BRAIN_H

//#include "AICallback.h"
//#include <string>
#include "ExternalAI/Interface/AISEvents.h"
//#include "ExternalAI/Interface/AISCommands.h"
//#include "Unit.h"
//#include "UnitDef.h"
//#include "Engine.h"

#include "Decision.h"
#include "KMedoids.h"

namespace brainSpace {

/**
 * This is the main C++ entry point of this AI.
 * 
 * @author	Robin Vobruba <hoijui.quaero@gmail.com>
 */
class BrAIn {

private:
	int teamId;

	AIClasses *ai;
	Decision *decision;

public:
	BrAIn(springai::AICallback* callback);
	~BrAIn();

	int HandleEvent(int topic, const void* data);

}; // class BrAIn

} // namespace brainSpace

#endif // _BRAINSPACE_BRAIN_H

