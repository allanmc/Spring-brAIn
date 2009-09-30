#ifndef _BRAINSPACE_UTILITY_H
#define _BRAINSPACE_UTILITY_H
#define MAGIC_CIRCLE_NUMBER 0.55228475
#include "global.h"

namespace brainSpace
{
	class Utility
	{
	private:
		AIClasses* ai;
		FILE *fp;

	public:
		Utility( AIClasses* aiClasses );
		~Utility();

		double EuclideanDistance( SAIFloat3 pos1, SAIFloat3 pos2 );
		void Log(int logLevel, int logType, const char* msg, ...);
		void ChatMsg(const char* msg, ...);
		void ChatMsg(string msg);
		UnitDef* GetUnitDef(const char* unitDefName);
		Resource* GetResource(const char* resourceName);
		int DrawCircle(SAIFloat3 pos, float radius);
		int DrawLine(SAIFloat3 start, SAIFloat3 end, bool arrow);
		void RemoveGraphics(int figureId);
	};
}
#endif
