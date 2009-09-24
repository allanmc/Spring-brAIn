#include "UtilityClass.h"

using namespace brainSpace;

UtilityClass::UtilityClass( AIClasses* aiClasses )
{
	AI = aiClasses;
}

void UtilityClass::ChatMsg(const char* msg, ...)
{
	static char c[200];

	if (strcmp(Info::GetInstance(AI->Callback)->GetValueByKey("debug"),"true")!=0)
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
	AI->Callback->GetEngine()->HandleCommand(0, -1, COMMAND_SEND_TEXT_MESSAGE, &cmd);
	
}


void UtilityClass::ChatMsg(std::string msg)
{
	if (strcmp(Info::GetInstance(AI->Callback)->GetValueByKey("debug"),"true")!=0)
	{
		return;
	}

	SSendTextMessageCommand cmd;
	cmd.text = msg.c_str();
	cmd.zone = 0;
	AI->Callback->GetEngine()->HandleCommand(0, -1, COMMAND_SEND_TEXT_MESSAGE, &cmd);
}

UnitDef* UtilityClass::GetUnitDef(const char* unitDefName)
{
	vector<UnitDef*> defs = AI->Callback->GetUnitDefs();

	for ( int i = 0 ; i < defs.size() ; i++ )
	{
		if ( strcmp( defs[i]->GetName(), unitDefName ) == 0 )
		{
			return defs[i];
		}
	}
	return NULL;
}

Resource* UtilityClass::GetResource(const char* resourceName)
{
	vector<Resource*> resources = AI->Callback->GetResources();

	for ( int i = 0 ; i < resources.size() ; i++ )
	{
		if ( strcmp( resources[i]->GetName(), resourceName ) == 0 )
		{
			return resources[i];
		}
	}
	return NULL;
}
