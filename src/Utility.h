#ifndef _BRAINSPACE_UTILITY_H
#define _BRAINSPACE_UTILITY_H
#define MAGIC_CIRCLE_NUMBER 0.55228475
#include "global.h"
#include "SAIFloat3.h"


const SAIFloat3 TEAM_0_COLOR = { 255, 0, 0 };
const SAIFloat3 TEAM_1_COLOR = { 255, 255, 0 };
const SAIFloat3 TEAM_2_COLOR = { 255, 0, 255 };
const SAIFloat3 TEAM_3_COLOR = { 0, 0, 255 };
const SAIFloat3 TEAM_4_COLOR = { 0, 255, 255 };
const SAIFloat3 TEAM_5_COLOR = { 0, 255, 0 };
const SAIFloat3 TEAM_6_COLOR = { 255, 255, 255 };
const SAIFloat3 TEAM_7_COLOR = { 0, 0, 0 };

namespace brainSpace
{

	class Utility
	{
	private:
		AIClasses* ai;
		FILE *fp;
		void AssignColorToGraphics( int figureGroupID );

	public:
		Utility( AIClasses* aiClasses );
		~Utility();

		double EuclideanDistance( SAIFloat3 pos1, SAIFloat3 pos2 );
		void Log(int logLevel, int logType, const char* msg, ...);
		void ChatMsg(const char* msg, ...);
		void ChatMsg(string msg);
		UnitDef* GetUnitDef(const char* unitDefName);
		Resource* GetResource(const char* resourceName);
		int DrawCircle(SAIFloat3 pos, float radius );
		int DrawLine(SAIFloat3 start, SAIFloat3 end, bool arrow);
		void RemoveGraphics(int figureId);
	};
}
#endif
