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

		float GetCurrentConsumption(springai::Resource *res);
		float GetCurrentProduction(springai::Resource *res);

		float GetCurrentAvailable(springai::Resource *res);
		float GetCurrentStorage(springai::Resource *res);

		float GetTimeToMetalDepletion();
		float GetTimeToEnergyDepletion();

		/*
		production in amount/sec
		ETA, the build time in frames
		returns an id, to be used for removal
		*/
		int AddChangeToCome(springai::Unit* unit, int ETA);
		void RemoveChangeToCome(springai::Unit* unit);

	private:
		AIClasses* ai;
		springai::Economy* economy;

		springai::Resource *metal;
		springai::Resource *energy;

		struct Change{
			Change(){}//dummy contructor
			unsigned int id;
			float metalProduction;
			float energyProduction;
			unsigned int ETA; //the frame where it is expected to be finished
		};
		std::list<Change> changes;
		unsigned int currentFrame;
	};
}

#endif
