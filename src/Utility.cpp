#include "Utility.h"
#include "global.h"

Utility::Utility( AIClasses* aiClasses )
{
	ai = aiClasses;
	fp = FOPEN("brAIn-log.txt", "w");
}

Utility::~Utility()
{
	fclose(fp);
}

void Utility::Log(int logLevel, int logType, const char* msg, ...)
{		
	if (LOG_LEVEL < logLevel)
	{
		return;
	}
	else if (((LOG_TYPE) & logType) == 0)
	{
		return;
	}
	static char c[200];
	
	if (fp)
	{
		va_list list;
		va_start(list, msg);
		vfprintf(fp, msg, list);
		va_end(list);
	}
	fputc('\n', fp);
}

void Utility::ChatMsg(const char* msg, ...)
{
	static char c[200];

	if (strcmp(Info::GetInstance(ai->callback)->GetValueByKey("debug"),"true")!=0)
	{
		return;
	}
	
	va_list list;
	va_start(list, msg);
	VSNPRINTF(c, 200, msg, list);
	va_end(list);
	SSendTextMessageCommand cmd;
	cmd.text = c;
	cmd.zone = 0;
	ai->callback->GetEngine()->HandleCommand(0, -1, COMMAND_SEND_TEXT_MESSAGE, &cmd);
}


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

UnitDef* Utility::GetUnitDef(const char* unitDefName)
{
	vector<UnitDef*> defs = ai->callback->GetUnitDefs();

	for ( int i = 0 ; i < defs.size() ; i++ )
	{
		if ( strcmp( defs[i]->GetName(), unitDefName ) == 0 )
		{
			return defs[i];
		}
	}
	return NULL;
}

Resource* Utility::GetResource(const char* resourceName)
{
	vector<Resource*> resources = ai->callback->GetResources();

	for ( int i = 0 ; i < resources.size() ; i++ )
	{
		if ( strcmp( resources[i]->GetName(), resourceName ) == 0 )
		{
			return resources[i];
		}
	}
	return NULL;
}

int Utility::DrawCircle(SAIFloat3 pos, float radius)
{
	SCreateSplineFigureDrawerCommand circle;
	circle.lifeTime = 0;
	circle.figureGroupId = 0;
	circle.width = 20;
	circle.arrow = false;
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

	SSetColorFigureDrawerCommand color;
	color.figureGroupId = circle.figureGroupId;
	color.color.x = 255;
	color.color.z = 0;
	color.color.y = 0;
	color.alpha = 255;
	ai->callback->GetEngine()->HandleCommand(0,-1, COMMAND_DRAWER_FIGURE_SET_COLOR, &color);
	return color.figureGroupId;
}

int Utility::DrawLine(SAIFloat3 start, SAIFloat3 end, bool arrow)
{
	SCreateLineFigureDrawerCommand line;
	line.arrow = arrow;
	line.lifeTime = 0;
	line.width = 20;
	line.pos1 = start;
	line.pos2 = end;
	
	ai->callback->GetEngine()->HandleCommand(0,-1, COMMAND_DRAWER_FIGURE_CREATE_LINE, &line);

	SSetColorFigureDrawerCommand color;
	color.figureGroupId = line.ret_newFigureGroupId;
	color.color.x = 255;
	color.color.z = 0;
	color.color.y = 0;
	color.alpha = 255;
	ai->callback->GetEngine()->HandleCommand(0,-1, COMMAND_DRAWER_FIGURE_SET_COLOR, &color);
	return color.figureGroupId;
}

void Utility::RemoveGraphics(int figureId)
{
	SDeleteFigureDrawerCommand removeCmd;
	removeCmd.figureGroupId = figureId;
	ai->callback->GetEngine()->HandleCommand(0,-1, COMMAND_DRAWER_FIGURE_DELETE, &removeCmd);
}

double Utility::EuclideanDistance(SAIFloat3 pos1, SAIFloat3 pos2)
{
	return sqrt( pow( fabs( pos1.x - pos2.x ), 2 ) + pow( fabs( pos1.z - pos2.z ), 2  ) );
}

