/*
	Copyright 2009 Robin Vobruba <hoijui.quaero@gmail.com>

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

	@author Robin Vobruba <hoijui.quaero@gmail.com>
*/

#include "AIExport.h"

#include "ExternalAI/Interface/AISEvents.h"
#include "AICallback.h" // generated by the C++ Wrapper scripts
#include "BrAIn.h"

#include <map>

static std::map<int, brainSpace::BrAIn*> myAIs;
static std::map<int, springai::AICallback*> myAICallbacks;

EXPORT(int) init(int teamId, const struct SSkirmishAICallback* innerCallback) {

	springai::AICallback* clb = springai::AICallback::GetInstance(innerCallback, teamId);
	brainSpace::BrAIn* ai = new brainSpace::BrAIn(clb);

	myAIs[teamId] = ai;
	myAICallbacks[teamId] = clb;

	// signal: everything went OK
	return 0;
}

EXPORT(int) release(int teamId) {

	springai::AICallback* clb = myAICallbacks[teamId];
	myAICallbacks.erase(teamId);

	brainSpace::BrAIn* ai = myAIs[teamId];
	myAIs.erase(teamId);

	delete ai;
	delete clb;

	// signal: everything went OK
	return 0;
}

EXPORT(int) handleEvent(int teamId, int topic, const void* data) {
	return myAIs[teamId]->HandleEvent(topic, data);
}
