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
		static Utility* UtilityInstance;
		AICallback* Callback;

		Utility(AICallback* callback );
		Utility();
		~Utility();

	public:
		static Utility* GetInstance(AICallback* callback );
		static Utility* GetInstance();
		void ChatMsg(const char* msg, ...);

		
		void ChatMsg(string msg);

	};
}
#endif
