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

		float GetTimeToMetalDepletion();
		float GetTimeToEnergyDepletion();

		/*
		production in amount/sec
		ETA, the build time in frames
		returns an id, to be used for removal
		*/
		int AddChangeToCome(Unit* unit, int ETA);
		void RemoveChangeToCome(Unit* unit);

	private:
		AIClasses* ai;
		Economy* economy;

		Resource *metal;
		Resource *energy;

		struct Change{
			Change(){}//dummy contructor
			unsigned int id;
			float metalProduction;
			float energyProduction;
			unsigned int ETA; //the frame where it is expected to be finished
		};
		list<Change> changes;
		unsigned int currentFrame;
	};
}

#endif
