#ifndef _BRAINSPACE_UTILITY_H
#define _BRAINSPACE_UTILITY_H
#define MAGIC_CIRCLE_NUMBER 0.55228475
#include "types.h"

#include "Cheats.h"

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
	///A collection of functions that provide shortcuts to commonly used functionallity
	class Utility
	{
	private:
		AIClasses* ai;
		FILE *fp;
		void AssignColorToGraphics( int figureGroupID );
		bool debug;
		void InitializeOptions();
		bool isMetalMap;
		springai::UnitDef *mexDef;
		springai::UnitDef *solarDef;
		springai::UnitDef *lltDef;
		SAIFloat3 safePosition;
		std::vector<springai::UnitDef*> defs;

	public:
		Utility( AIClasses* aiClasses );
		~Utility();

		double EuclideanDistance( SAIFloat3 pos1, SAIFloat3 pos2 );
		void Log(int logLevel, int logType, const char* msg, ...);
		void LogNN(int logLevel, int logType, const char* msg, ...);
		void ChatMsg(const char* msg, ...);
		void ChatMsg(std::string msg);
		springai::UnitDef* GetUnitDef(const char* unitDefName);
		springai::Resource* GetResource(const char* resourceName);
		int DrawCircle(SAIFloat3 pos, float radius );
		int DrawCircle(SAIFloat3 pos, float radius, int figureId);
		int DrawLine(SAIFloat3 start, SAIFloat3 end, bool arrow = false, float width = 20.0f, int figureId = 0 );
		void RemoveGraphics(int figureId);
		bool IsDebug();
		bool FileExists( const char* FileName );
		bool IsMetalMap();
		void LaterInitialization();
		void Suicide(int unitToSurvive=0, bool stopAll=false);
		void ResetGame(RL **rl);
		SAIFloat3 GoTo(int unitId, SAIFloat3 pos, bool simulate = false);
		SAIFloat3 GetSafePosition();
		springai::UnitDef* GetMexDef();
		springai::UnitDef* GetSolarDef();
		springai::UnitDef* GetLLTDef();
	};
}
#endif
