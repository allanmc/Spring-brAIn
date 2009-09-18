#include "Utility.h"

using namespace brainSpace;

Utility::Utility(AICallback* callback )
{
	Callback = callback;
}

void Utility::ChatMsg(const char* msg, ...)
{
	static char c[200];

	/*
	OptionValues* optionValues = OptionValues::GetInstance(Callback);
	
	//optionValues->GetValueByKey("debug");
	//Utility::ChatMsg(string(optionValues->GetValueByKey("debug")));
	SNPRINTF(c, 200, "OptionValues getSize: %i", optionValues->GetSize());
	SSendTextMessageCommand cmd;
	cmd.text = c;
	cmd.zone = 0;
	Callback->GetEngine()->HandleCommand(0, -1, COMMAND_SEND_TEXT_MESSAGE, &cmd);
	
	for (int i = 0; i < optionValues->GetSize(); i++) {
		SNPRINTF(c, 200, "OptionValues debug: %s = %s", optionValues->GetKey(i), optionValues->GetValue(i));
		cmd.text = c;
		cmd.zone = 0;
		Callback->GetEngine()->HandleCommand(0, -1, COMMAND_SEND_TEXT_MESSAGE, &cmd);
	}
	*/
	
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
	SSendTextMessageCommand cmd;
	cmd.text = msg.c_str();
	cmd.zone = 0;
	Callback->GetEngine()->HandleCommand(0, -1, COMMAND_SEND_TEXT_MESSAGE, &cmd);
}