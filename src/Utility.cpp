#include "Utility.h"

using namespace brainSpace;

Utility::Utility(AICallback* callback )
{
	Callback = callback;
}

void Utility::ChatMsg(const char* msg, ...)
{
	static char c[200];

	if (strcmp(Info::GetInstance(Callback)->GetValueByKey("debug"),"true")!=0)
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
	Callback->GetEngine()->HandleCommand(0, -1, COMMAND_SEND_TEXT_MESSAGE, &cmd);
	
}


void Utility::ChatMsg(std::string msg)
{
	if (strcmp(Info::GetInstance(Callback)->GetValueByKey("debug"),"true")!=0)
	{
		return;
	}

	SSendTextMessageCommand cmd;
	cmd.text = msg.c_str();
	cmd.zone = 0;
	Callback->GetEngine()->HandleCommand(0, -1, COMMAND_SEND_TEXT_MESSAGE, &cmd);
}

UnitDef* Utility::GetUnitDef(const char* unitDefName)
{
	vector<UnitDef*> defs = Callback->GetUnitDefs();

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
	vector<Resource*> resources = Callback->GetResources();

	for ( int i = 0 ; i < resources.size() ; i++ )
	{
		if ( strcmp( resources[i]->GetName(), resourceName ) == 0 )
		{
			return resources[i];
		}
	}
	return NULL;
}
