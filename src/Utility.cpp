#include "Utility.h"
#include "global.h"
#include <stdio.h>

using namespace std;
using namespace springai;
using namespace brainSpace;

Utility::Utility( AIClasses* aiClasses )
{
	ai = aiClasses;
	char filename[200];
	char path[200];
	const char *dir = DataDirs::GetInstance(ai->callback)->GetWriteableDir();
	strcpy(path, dir);
	SNPRINTF( filename, 200, "Brain-log-team%d.txt", aiClasses->callback->GetTeamId() );
	strcat(path, filename);
	fp = FOPEN(path, "w");
	debug = false;
	InitializeOptions();
	isMetalMap = false;
}

Utility::~Utility()
{
	fclose(fp);
}

///@return if debug is enabled
bool Utility::IsDebug()
{
	return debug;
}

void Utility::LaterInitialization()
{
	isMetalMap = ai->callback->GetMap()->GetResourceMapSpotsPositions(*ai->utility->GetResource("Metal"), NULL).size() > 200;
}

bool Utility::IsMetalMap()
{
	return isMetalMap;
}
///Load the options set for the AI
void Utility::InitializeOptions()
{
	const char* optionStr = OptionValues::GetInstance(ai->callback)->GetValueByKey("debug");
	if (optionStr == NULL
		||
		strcmp(optionStr,"true")==0
		||
		strcmp(optionStr,"1")==0)
	{
		debug = true;
	}
	else debug = false;
}

///Prints a line in the log file
///@param logLevel the minimum LOGLEVEL that this should be logged at
///@param logType the type of information being logged, used to filter logging
///@param msg the text to be printed in printf-syntax
void Utility::Log(int logLevel, int logType, const char* msg, ...)
{		
	if (!IsDebug() || LOG_LEVEL < logLevel)
	{
		return;
	}
	else if (((LOG_TYPE) & logType) == 0)
	{
		return;
	}
	
	if (fp)
	{
		va_list list;
		va_start(list, msg);
		vfprintf(fp, msg, list);
		va_end(list);
		fputc('\n', fp);
	}
}

///prints a line to the chat
void Utility::ChatMsg(const char* msg, ...)
{
	static char c[200];

	
	va_list list;
	va_start(list, msg);
	VSNPRINTF(c, 200, msg, list);
	va_end(list);
	SSendTextMessageCommand cmd;
	cmd.text = c;
	cmd.zone = 0;
	ai->callback->GetEngine()->HandleCommand(0, -1, COMMAND_SEND_TEXT_MESSAGE, &cmd);
}

///prints a line to the chat
void Utility::ChatMsg(std::string msg)
{
	if (strcmp(Info::GetInstance(ai->callback)->GetValueByKey("debug"),"true")!=0)
	{
		return;
	}

	SSendTextMessageCommand cmd;
	cmd.text = msg.c_str();
	cmd.zone = 0;
	ai->callback->GetEngine()->HandleCommand(0, -1, COMMAND_SEND_TEXT_MESSAGE, &cmd);
}

///@return the UnitDef of with a given name, or NULL if the UnitDef does not exists
UnitDef* Utility::GetUnitDef(const char* unitDefName)
{
	vector<UnitDef*> defs = ai->callback->GetUnitDefs();

	for ( int i = 0 ; i < (int)defs.size() ; i++ )
	{
		if ( strcmp( defs[i]->GetName(), unitDefName ) == 0 )
		{
			return defs[i];
		}
	}
	return NULL;
}

///@return the Resource with the given name, or NULL if the Resource does not exists
Resource* Utility::GetResource(const char* resourceName)
{
	vector<Resource*> resources = ai->callback->GetResources();

	for ( int i = 0 ; i < (int)resources.size() ; i++ )
	{
		if ( strcmp( resources[i]->GetName(), resourceName ) == 0 )
		{
			return resources[i];
		}
	}
	return NULL;
}
///draws a circle on the map
int Utility::DrawCircle(SAIFloat3 pos, float radius)
{
	return DrawCircle(pos, radius, 0);
}

///draws a circle on the map given figureId
int Utility::DrawCircle(SAIFloat3 pos, float radius, int figureId)
{
	SCreateSplineFigureDrawerCommand circle;
	circle.lifeTime = 0;
	circle.figureGroupId = 0;
	circle.width = 20;
	circle.arrow = false;
	circle.figureGroupId = figureId;

	circle.pos1 = pos;
	circle.pos4 = pos;

	circle.pos1.z += radius;
	circle.pos4.x += radius;
	circle.pos2 = circle.pos1;
	circle.pos3 = circle.pos4;
	circle.pos2.x += MAGIC_CIRCLE_NUMBER*radius;
	circle.pos3.z += MAGIC_CIRCLE_NUMBER*radius;
	ai->callback->GetEngine()->HandleCommand(0, -1, COMMAND_DRAWER_FIGURE_CREATE_SPLINE, &circle);
	circle.figureGroupId = circle.ret_newFigureGroupId;

	circle.pos1 = circle.pos4;
	circle.pos4 = pos;
	circle.pos4.z -= radius;
	circle.pos2 = circle.pos1;
	circle.pos3 = circle.pos4;
	circle.pos2.z -= MAGIC_CIRCLE_NUMBER*radius;
	circle.pos3.x += MAGIC_CIRCLE_NUMBER*radius;
	ai->callback->GetEngine()->HandleCommand(0, -1, COMMAND_DRAWER_FIGURE_CREATE_SPLINE, &circle);

	circle.pos1 = circle.pos4;
	circle.pos4 = pos;
	circle.pos4.x -= radius;
	circle.pos2 = circle.pos1;
	circle.pos3 = circle.pos4;
	circle.pos2.x -= MAGIC_CIRCLE_NUMBER*radius;
	circle.pos3.z -= MAGIC_CIRCLE_NUMBER*radius;
	ai->callback->GetEngine()->HandleCommand(0, -1, COMMAND_DRAWER_FIGURE_CREATE_SPLINE, &circle);

	circle.pos1 = circle.pos4;
	circle.pos4 = pos;
	circle.pos4.z += radius;
	circle.pos2 = circle.pos1;
	circle.pos3 = circle.pos4;
	circle.pos2.z += MAGIC_CIRCLE_NUMBER*radius;
	circle.pos3.x -= MAGIC_CIRCLE_NUMBER*radius;
	ai->callback->GetEngine()->HandleCommand(0, -1, COMMAND_DRAWER_FIGURE_CREATE_SPLINE, &circle);

	AssignColorToGraphics( circle.figureGroupId );

	return circle.figureGroupId;
}

///draws a line on the map
int Utility::DrawLine(SAIFloat3 start, SAIFloat3 end, bool arrow, float width, int figureId )
{
	SCreateLineFigureDrawerCommand line;
	line.arrow = arrow;
	line.lifeTime = 0;
	line.width = width;
	line.pos1 = start;
	line.pos2 = end;
	line.figureGroupId = figureId;
	
	ai->callback->GetEngine()->HandleCommand(0,-1, COMMAND_DRAWER_FIGURE_CREATE_LINE, &line);

	AssignColorToGraphics( line.ret_newFigureGroupId );
	return line.ret_newFigureGroupId;
}

///removes all grephic elements with a given figureId
void Utility::RemoveGraphics(int figureId)
{
	SDeleteFigureDrawerCommand removeCmd;
	removeCmd.figureGroupId = figureId;
	ai->callback->GetEngine()->HandleCommand(0,-1, COMMAND_DRAWER_FIGURE_DELETE, &removeCmd);
}

///@return the direct distance bestween two points(2D)
double Utility::EuclideanDistance(SAIFloat3 pos1, SAIFloat3 pos2)
{
	return sqrt( pow( fabs( pos1.x - pos2.x ), 2 ) + pow( fabs( pos1.z - pos2.z ), 2  ) );
}

///colors all graphic objects with a given figureID
void Utility::AssignColorToGraphics( int figureGroupID )
{
	SSetColorFigureDrawerCommand color;
	color.figureGroupId = figureGroupID;
	color.alpha = 255;
	switch ( ai->callback->GetTeamId() )
	{
	case 0:
		color.color.x = TEAM_0_COLOR.x;
		color.color.y = TEAM_0_COLOR.y;
		color.color.z = TEAM_0_COLOR.z;
		break;
	case 1:
		color.color.x = TEAM_1_COLOR.x;
		color.color.y = TEAM_1_COLOR.y;
		color.color.z = TEAM_1_COLOR.z;
		break;
	case 2:
		color.color.x = TEAM_2_COLOR.x;
		color.color.y = TEAM_2_COLOR.y;
		color.color.z = TEAM_2_COLOR.z;
		break;
	case 3:
		color.color.x = TEAM_3_COLOR.x;
		color.color.y = TEAM_3_COLOR.y;
		color.color.z = TEAM_3_COLOR.z;
		break;
	case 4:
		color.color.x = TEAM_4_COLOR.x;
		color.color.y = TEAM_4_COLOR.y;
		color.color.z = TEAM_4_COLOR.z;
		break;
	case 5:
		color.color.x = TEAM_5_COLOR.x;
		color.color.y = TEAM_5_COLOR.y;
		color.color.z = TEAM_5_COLOR.z;
		break;
	case 6:
		color.color.x = TEAM_6_COLOR.x;
		color.color.y = TEAM_6_COLOR.y;
		color.color.z = TEAM_6_COLOR.z;
		break;
	case 7:
		color.color.x = TEAM_7_COLOR.x;
		color.color.y = TEAM_7_COLOR.y;
		color.color.z = TEAM_7_COLOR.z;
		break;
	}
	ai->callback->GetEngine()->HandleCommand(0,-1, COMMAND_DRAWER_FIGURE_SET_COLOR, &color);
}

bool Utility::FileExists( const char* FileName )
{
    FILE* fp = NULL;

    fp = fopen( FileName, "rb" );
    if( fp != NULL )
    {
        fclose( fp );
        return true;
    }

    return false;
}

void Utility::Suicide()
{
	Log(IMPORTANT, MISC, "Bye Cruel world!!...."); 
	vector<Unit*> units = ai->callback->GetFriendlyUnits();

	vector<Unit*>::iterator it;
	for(it = units.begin(); it != units.end(); it++)
	{
		SSelfDestroyUnitCommand command;
		command.unitId = (*it)->GetUnitId();
		command.timeOut = 99999;
		ai->callback->GetEngine()->HandleCommand(0,-1, COMMAND_UNIT_SELF_DESTROY, &command);
	}
}
