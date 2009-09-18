#include "Utility.h"

using namespace brainSpace;

Utility* Utility::UtilityInstance = 0;

Utility::Utility(AICallback* callback )
{
	Callback = callback;
}

Utility::Utility()
{
}


Utility* Utility::GetInstance(AICallback* callback )
{
	if ( !UtilityInstance )
	{
		Utility::UtilityInstance = new Utility(callback);
	}
	return Utility::UtilityInstance;
}


Utility* Utility::GetInstance( )
{
	return Utility::UtilityInstance;
}


void Utility::ChatMsg(const char* msg, ...)
{
	static char c[200];
	va_list list;
	va_start(list, msg);
	SNPRINTF(c, 200, msg, list);
	va_end(list);
	SSendTextMessageCommand cmd;
	cmd.text = c;
	cmd.zone = 0;
	Callback->GetEngine()->HandleCommand(0, -1, COMMAND_SEND_TEXT_MESSAGE, &cmd);
}


void Utility::ChatMsg(std::string msg)
{
	SSendTextMessageCommand cmd;
	cmd.text = msg.c_str();
	cmd.zone = 0;
	Callback->GetEngine()->HandleCommand(0, -1, COMMAND_SEND_TEXT_MESSAGE, &cmd);
}