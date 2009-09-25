#ifndef _BRAINSPACE_RESOURCEINFO_H
#define _BRAINSPACE_RESOURCEINFO_H

#include "global.h"
#include "Economy.h"

#define AIFRAMES_PR_SECOND 30

namespace brainSpace
{
	class ResourceInfo
	{
	public:
		ResourceInfo(AIClasses* ai);
		virtual ~ResourceInfo();

		void Update(int frame);

		float GetCurrentConsumption(Resource *res);
		float GetCurrentProduction(Resource *res);

		float GetCurrentAvailable(Resource *res);
		float GetCurrentStorage(Resource *res);

		float GetTimeToDepletion(Resource *res);

		/*
		production in amount/sec
		ETA, the build time in frames
		returns an id, to be used for removal
		*/
		int AddChangeToCome(Resource *res, float production, int ETA);
		void RemoveChangeToCome(int id);

	private:
		AIClasses* ai;
		Economy* economy;

		unsigned int changeIdCounter;
		struct Change{
			Change(){}
			unsigned int id;
			Resource *res;
			float production;
			unsigned int ETA; //the frame where it is expected to be finished
		};
		list<Change> changes;
		unsigned int currentFrame;
	};
}

#endif
