#include "Utility.h"
#include "global.h"
#include <stdio.h>
#include <sstream>
#include "Battle.h"
#include <map>
using namespace std;
using namespace springai;
using namespace brainSpace;

Utility::Utility( AIClasses* aiClasses )
{
	debug = true;
	ai = aiClasses;
	char filename[200];
	char path[200];
	DataDirs *dirs = DataDirs::GetInstance(ai->callback);
	const char *dir = dirs->GetWriteableDir();
	strcpy(path, dir);
	SNPRINTF( filename, 200, "Brain-log-team%d.txt", aiClasses->callback->GetTeamId() );
	strcat(path, filename);
	fp = FOPEN(path, "w");
	Log(ALL, MISC, "Initialized Utility-class...");
	engine = ai->callback->GetEngine();
	map = ai->callback->GetMap();
	resources = ai->callback->GetResources();
	InitializeOptions();
	isMetalMap = false;

	safePosition = (SAIFloat3){map->GetWidth()*8/2, 0.0, map->GetHeight()*8/2};
	delete dirs;
}

Utility::~Utility()
{
	fclose(fp);	
	delete map;
	delete engine;
	for(int i = 0; i < (int)defs.size(); i++)
	{
		delete defs[i];
	}
	for(int i = 0; i < (int)resources.size(); i++)
	{
		delete resources[i];
	}
}

///@return the safe position whether a building blocks the exit of out base
SAIFloat3 Utility::GetSafePosition()
{
	return safePosition;
}

///@return if debug is enabled
bool Utility::IsDebug()
{
	return debug;
}

float Utility::GetDpsFromUnitDef(springai::UnitDef *def)
{
	vector<WeaponMount*> weaponMounts = def->GetWeaponMounts();
	float dmg = 0;
	for ( int i = 0 ; i < (int)weaponMounts.size() ; i++ )
	{
		WeaponDef* wDef = weaponMounts.at(i)->GetWeaponDef();
		dmg += (wDef->GetDamage()->GetTypes().at(def->GetArmorType()) / wDef->GetReload());
	}
	return dmg;
}

void Utility::LaterInitialization()
{
	ai->utility->Log(ALL, MISC, "Calling GetUnitDefs");
	defs = ai->callback->GetUnitDefs();
	ai->utility->Log(ALL, MISC, "Done calling GetUnitDefs");
	mexDef = GetUnitDef("armmex");
	solarDef = GetUnitDef("armsolar");
	lltDef = GetUnitDef("armllt");
	isMetalMap = map->GetResourceMapSpotsPositions(*ai->utility->GetResource("Metal"), NULL).size() > 200;	
	Log(ALL, MISC, "Done calling LaterInitialization()");
}

bool Utility::IsMetalMap()
{
	return isMetalMap;
}
///Load the options set for the AI
void Utility::InitializeOptions()
{
	OptionValues *opt = OptionValues::GetInstance(ai->callback);
	const char* optionStr = opt->GetValueByKey("debug");
	if (optionStr == NULL
		||
		strcmp(optionStr,"true")==0
		||
		strcmp(optionStr,"1")==0)
	{
		debug = true;
	}
	else debug = false;
	delete opt;
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
		fprintf(fp, "Frame %d: ", ai->frame);

		va_list list;
		va_start(list, msg);
		vfprintf(fp, msg, list);
		va_end(list);
		fputc('\n', fp);
		fflush(fp);
	}
}

///Prints a line in the log file - no new line
///@param logLevel the minimum LOGLEVEL that this should be logged at
///@param logType the type of information being logged, used to filter logging
///@param msg the text to be printed in printf-syntax
void Utility::LogNN(int logLevel, int logType, const char* msg, ...)
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
		fflush(fp);
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
	engine->HandleCommand(0, -1, COMMAND_SEND_TEXT_MESSAGE, &cmd);

	LogNN(ALL, MISC, "Chat: ");
	Log(ALL, MISC, c);
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
	engine->HandleCommand(0, -1, COMMAND_SEND_TEXT_MESSAGE, &cmd);
}

UnitDef* Utility::GetMexDef()
{
	return mexDef;
}

UnitDef* Utility::GetSolarDef()
{
	return solarDef;
}

UnitDef* Utility::GetLLTDef()
{
	return lltDef;
}


UnitDef* Utility::GetUnitDef(int id)
{
	if(ai->frame > 0)
	{
		for ( int i = 0 ; i < (int)defs.size() ; i++ )
		{
			if ( defs[i]->GetUnitDefId() == id )
			{
				return defs[i];
			}
		}
	}
	else
	{
		Log(ALL, MISC, "Could not GetUnitDef, because frame==0, CRITICAL ERROR!");
	}
	return NULL;
}

///@return the UnitDef of with a given name, or NULL if the UnitDef does not exists
UnitDef* Utility::GetUnitDef(const char* unitDefName)
{
	if(ai->frame > 0)
	{
		for ( int i = 0 ; i < (int)defs.size() ; i++ )
		{
			//ai->utility->ChatMsg(defs[i]->GetName());
			if ( strcmp( defs[i]->GetName(), unitDefName ) == 0 )
			{
				return defs[i];
			}
		}
	}
	else
	{
		Log(ALL, MISC, "Could not GetUnitDef, because frame==0, CRITICAL ERROR!");
	}
	ai->utility->ChatMsg("GETUNITDEF: Could not find unitdef %s. Defs size %d", unitDefName, defs.size());
	return NULL;
}

///@return the Resource with the given name, or NULL if the Resource does not exists
Resource* Utility::GetResource(const char* resourceName)
{
	Resource* retval = NULL;
	for ( int i = 0 ; i < (int)resources.size() ; i++ )
	{
		if ( strcmp( resources[i]->GetName(), resourceName ) == 0 )
		{
			retval = resources[i];			
		}
	}
	
	return retval;
}

///Order the unit to goto a specific location using our own pathfinding
SAIFloat3 Utility::GoTo(int unitId, SAIFloat3 pos, bool simulate)
{
	ai->utility->Log(ALL, MISC, "GoTo: I am now going to find a path..");
	Unit* unit = Unit::GetInstance(ai->callback, unitId);
	UnitDef *def = unit->GetDef();
	vector<SAIFloat3> wayPoints;
	bool goToward = true;
	if (ai->utility->EuclideanDistance(unit->GetPos(), pos) < def->GetBuildDistance())
	{
		//wayPoints.push_back(pos);
		Path* p = ai->knowledge->mapInfo->pathfindingMap->FindPathTo(def, unit->GetPos(), GetSafePosition());
		ai->utility->Log(ALL, MISC, "GoTo: I am going backwards!");
		goToward = false;
		wayPoints = p->GetPoints();
		delete p;
	}
	else
	{
		Path* p = ai->knowledge->mapInfo->pathfindingMap->FindPathTo(def, unit->GetPos(), pos);
		ai->utility->Log(ALL, MISC, "GoTo: I am going towards buildlocation!");
		wayPoints = p->GetPoints();
		delete p;
	}
	SAIFloat3 retPos = (SAIFloat3){0,-1,0};
	if(wayPoints.size() == 0)
	{
		delete def;
		delete unit;
		return retPos;
	}

	SMoveUnitCommand moveCommand;
	
	if (!simulate)
	{
		moveCommand.timeOut = 100000000;
		moveCommand.options = 0;
		moveCommand.unitId = unitId;
	}
	int toWalk = 0;
	//for (int i = 0; i < wayPoints.size(); i++)
	for (vector<SAIFloat3>::const_iterator it = wayPoints.begin(); it != wayPoints.end(); ++it)
	{
		
		if ( (goToward && (def->GetBuildDistance() > ai->utility->EuclideanDistance(*it, pos))) ||
			 (!goToward && (def->GetBuildDistance() < ai->utility->EuclideanDistance(*it, pos))) )
			break; //Ignore moves that goes unnecesarry close to the building-spot
		toWalk++;
		retPos = *it;
		if (!simulate && retPos.x != ai->commander->GetPos().x && retPos.z != ai->commander->GetPos().z)
		{
			moveCommand.toPos = retPos;
			moveCommand.options = UNIT_COMMAND_OPTION_SHIFT_KEY;
			engine->HandleCommand(0, -1, COMMAND_UNIT_MOVE, &moveCommand);
		}
	}
	ai->utility->Log(ALL, MISC, "GoTo: I am done! I want to walk %i out of %i waypoints", toWalk, wayPoints.size());
	int res = ai->knowledge->mapInfo->pathfindingMap->GetMapData()->MapResolution;
	ai->utility->Log(ALL, MISC, "GoTo: Final pathfinding INDEX: %d, %d", (int)retPos.x/res, (int)retPos.z/res);

	delete def;
	delete unit;
	return retPos;
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
	engine->HandleCommand(0, -1, COMMAND_DRAWER_FIGURE_CREATE_SPLINE, &circle);
	circle.figureGroupId = circle.ret_newFigureGroupId;

	circle.pos1 = circle.pos4;
	circle.pos4 = pos;
	circle.pos4.z -= radius;
	circle.pos2 = circle.pos1;
	circle.pos3 = circle.pos4;
	circle.pos2.z -= MAGIC_CIRCLE_NUMBER*radius;
	circle.pos3.x += MAGIC_CIRCLE_NUMBER*radius;
	engine->HandleCommand(0, -1, COMMAND_DRAWER_FIGURE_CREATE_SPLINE, &circle);

	circle.pos1 = circle.pos4;
	circle.pos4 = pos;
	circle.pos4.x -= radius;
	circle.pos2 = circle.pos1;
	circle.pos3 = circle.pos4;
	circle.pos2.x -= MAGIC_CIRCLE_NUMBER*radius;
	circle.pos3.z -= MAGIC_CIRCLE_NUMBER*radius;
	engine->HandleCommand(0, -1, COMMAND_DRAWER_FIGURE_CREATE_SPLINE, &circle);

	circle.pos1 = circle.pos4;
	circle.pos4 = pos;
	circle.pos4.z += radius;
	circle.pos2 = circle.pos1;
	circle.pos3 = circle.pos4;
	circle.pos2.z += MAGIC_CIRCLE_NUMBER*radius;
	circle.pos3.x -= MAGIC_CIRCLE_NUMBER*radius;
	engine->HandleCommand(0, -1, COMMAND_DRAWER_FIGURE_CREATE_SPLINE, &circle);

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
	
	engine->HandleCommand(0,-1, COMMAND_DRAWER_FIGURE_CREATE_LINE, &line);

	AssignColorToGraphics( line.ret_newFigureGroupId );
	return line.ret_newFigureGroupId;
}

///removes all grephic elements with a given figureId
void Utility::RemoveGraphics(int figureId)
{
	SDeleteFigureDrawerCommand removeCmd;
	removeCmd.figureGroupId = figureId;
	engine->HandleCommand(0,-1, COMMAND_DRAWER_FIGURE_DELETE, &removeCmd);
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
	engine->HandleCommand(0,-1, COMMAND_DRAWER_FIGURE_SET_COLOR, &color);
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

void Utility::ResetGame(RL **rl)
{
	Log(IMPORTANT, MISC, "brAIn is now resetting..."); 

	Log(IMPORTANT, MISC, "Resetting RL...");
	delete *rl;
	*rl = NULL;
	*rl = new RL( ai, EPSILON_START );//FY!

	/*
	Log(IMPORTANT, MISC, "Creating new commander..."); 
	//Give me a new commander
	//int newCommanderId = 0;
	SGiveMeNewUnitCheatCommand giveUnitOrder;
	giveUnitOrder.pos = (SAIFloat3){10,200, 10};
	giveUnitOrder.unitDefId = ai->utility->GetUnitDef("armcom")->GetUnitDefId();
	engine->HandleCommand(0,-1, COMMAND_CHEATS_GIVE_ME_NEW_UNIT, &giveUnitOrder);
	delete ai->commander;
	ai->commander = Unit::GetInstance(ai->callback, giveUnitOrder.ret_newUnitId);
	
	
	Log(ALL, MISC, "Telling the new commander to move to startPos, commanderid: %d", ai->commander->GetUnitId());
	SMoveUnitCommand moveCommand;
	moveCommand.toPos = map->GetStartPos();
	moveCommand.timeOut = 100000000;
	moveCommand.options = 0;
	moveCommand.unitId = ai->commander->GetUnitId();
	engine->HandleCommand(0, -1, COMMAND_UNIT_MOVE, &moveCommand);

	Log(IMPORTANT, MISC, "Giving us start resources");
	SGiveMeResourceCheatCommand resourceCommand;
	resourceCommand.amount = 1000;
	resourceCommand.resourceId = GetResource("Metal")->GetResourceId();
	engine->HandleCommand(0, -1, COMMAND_CHEATS_GIVE_ME_RESOURCE, &resourceCommand);
	resourceCommand.resourceId = GetResource("Energy")->GetResourceId();
	engine->HandleCommand(0, -1, COMMAND_CHEATS_GIVE_ME_RESOURCE, &resourceCommand);
	Log(IMPORTANT, MISC, "Killing all units besides the commander..."); 
	//Delete all units besides out new commander
	*/
}

void Utility::Suicide(int unitToSurvive, bool killAll, bool stopAll)
{
	Log(IMPORTANT, MISC, "Bye Cruel world!!...."); 
	vector<Unit*> units = ai->callback->GetFriendlyUnits();

	vector<Unit*>::iterator it;
	for(it = units.begin(); it != units.end(); it++)
	{
		UnitDef* d = (*it)->GetDef();
		if ( !killAll )
		{
			if ( d->IsCommander() )
			{
				continue;
			}
		}
		//if ((*it)->GetUnitId() == unitToSurvive) continue; //Don't kill our new commander
		/*if (stopAll)
		{
			SStopUnitCommand stopCommand;
			stopCommand.timeOut = 99999;
			stopCommand.unitId = (*it)->GetUnitId();
			engine->HandleCommand(0,-1, COMMAND_UNIT_STOP, &stopCommand);
		}*/
		
		SSelfDestroyUnitCommand command;
		command.unitId = (*it)->GetUnitId();
		command.timeOut = 99999;
		command.options = ( stopAll ? 0 : UNIT_COMMAND_OPTION_SHIFT_KEY );
		engine->HandleCommand(0,-1, COMMAND_UNIT_SELF_DESTROY, &command);
		delete (*it);
	}
}


Unit* Utility::GiveUnit(const char* defName, SAIFloat3 pos )
{
	UnitDef* d = GetUnitDef(defName);
	SGiveMeNewUnitCheatCommand giveUnitOrder;
	giveUnitOrder.pos = pos;
	giveUnitOrder.unitDefId = d->GetUnitDefId();
	engine->HandleCommand(0,-1, COMMAND_CHEATS_GIVE_ME_NEW_UNIT, &giveUnitOrder);
	return Unit::GetInstance(ai->callback, giveUnitOrder.ret_newUnitId);
}

Unit* Utility::GiveUnit(unsigned int defID, SAIFloat3 pos)
{
	SGiveMeNewUnitCheatCommand giveUnitOrder;
	giveUnitOrder.pos = pos;
	giveUnitOrder.unitDefId = defID;
	engine->HandleCommand(0,-1, COMMAND_CHEATS_GIVE_ME_NEW_UNIT, &giveUnitOrder);
	return Unit::GetInstance(ai->callback, giveUnitOrder.ret_newUnitId);
}


bool Utility::CanHit( WeaponMount* mount, UnitDef* u )
{
	unsigned int cat = u->GetCategory();
	return ( ( mount->GetBadTargetCategory() & cat ) == 0 );
}


vector<Unit*> Utility::GetNearestEnemyUnits( MilitaryUnitGroup* group, int numArgs, ... )
{
	std::map<int, UnitInformationContainer> units = ai->knowledge->enemyInfo->baseInfo->GetUnits();
	vector<Unit*> retUnits;
	va_list ap;
	va_start( ap, numArgs );
	vector<const char*> names;
	vector<double> nearest;
	for ( int i = 0 ; i < numArgs ; i++ )
	{
		const char* name = va_arg(ap, const char* );
		names.push_back(name);
		retUnits.push_back( NULL );
		nearest.push_back( 1000000.0 );
	}

	for ( std::map<int, UnitInformationContainer>::iterator it = units.begin() ; it != units.end() ; it++ )
	{
		Unit *u = Unit::GetInstance( ai->callback, it->first );
		UnitDef* d = u->GetDef();
		delete d;
		delete u;
		for ( unsigned int i = 0 ; i < names.size() ; i++ )
		{
			if ( strcmp( it->second.def->GetName(), names[i] ) == 0 )
			{
				double dist = ai->utility->EuclideanDistance( it->second.pos, group->GetPos() );
				if ( dist < nearest[i] )
				{
					nearest[i] = dist;
					if ( retUnits[i] != NULL )
						delete retUnits[i];
					retUnits[i] = Unit::GetInstance( ai->callback, it->first );
				}
			}
		}
	}
	return retUnits;
}


void Utility::WriteToStateVisitFile( fstream* file, int val, int offset )
{
	if ( offset != -1 )
		file->seekp(sizeof(int)*offset, ios::beg);
	file->write( (char*)&val, sizeof(int) );
	file->flush();
}


int Utility::ReadFromStateVisitFile( fstream* file, int offset )
{
	int val;
	if ( offset != -1 )
		file->seekg(sizeof(int)*offset, ios::beg);
	file->read( (char*)&val, sizeof(int) );
	return val;
}
