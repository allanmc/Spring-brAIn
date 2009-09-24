#ifndef _BRAINSPACE_RESOURCEINFO_H
#define _BRAINSPACE_RESOURCEINFO_H

#include "global.h"
#include "Economy.h"

namespace brainSpace
{
	class ResourceInfo
	{
	public:
		ResourceInfo(AIClasses* ai);
		virtual ~ResourceInfo();

		float GetCurrentConsumption(Resource res);
		float GetCurrentProduction(Resource res);

		float GetCurrentAvailable(Resource res);
		float GetCurrentStorage(Resource res);

		float GetTimeToDepletion(Resource res);

		/*
		production in amount/sec
		ETA in seconds
		returns an id, to be used for removal
		*/
		int AddProductionToCome(Resource res, float production, float ETA);
		void RemoveProductionToCome(int id);

	private:
		AIClasses* ai;

		Economy* economy;
	
	};
}

#endif
