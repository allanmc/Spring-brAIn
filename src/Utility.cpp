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

SAIFloat3 Utility::GetBestMetalSpot(SAIFloat3 position, Resource* resource)
{
	vector<unsigned char> rawMap = Callback->GetMap()->GetResourceMapRaw(*resource);
	int rawMapSize = rawMap.size();
	int width = Callback->GetMap()->GetWidth() / 2;
	int height = Callback->GetMap()->GetHeight() / 2;
	for (int z = 0; z < height; z++) {
		for (int x = 0; x < width; x++) {
			/**
			 * @brief resource maps
			 * This map shows the resource density on the map.
			 * 
			 * - do NOT modify or delete the height-map (native code relevant only)
			 * - index 0 is top left
			 * - each data position is 2*2 in size
			 * - the value for the full resolution position (x, z) is at index (x/2 * width + z/2)
			 * - the last value, bottom right, is at index (width/2 * height/2 - 1)
			 */
			ChatMsg("Raw map info %i:%i = %c", x, z, rawMap[z * width + x]);
		}
	}
}