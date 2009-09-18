#ifndef _BRAINSPACE_UTILITY_H
#define _BRAINSPACE_UTILITY_H

#include "AICallback.h"
#include <stdarg.h>
#include <string>

#include "ExternalAI/Interface/AISCommands.h"
#include "Engine.h"


using namespace springai;
using namespace std;

namespace brainSpace
{
	class Utility
	{
	private:
		AICallback* Callback;
		~Utility();

	public:
		Utility(AICallback* callback );

		void ChatMsg(const char* msg, ...);
		void ChatMsg(string msg);

	};
}
#endif
