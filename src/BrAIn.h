/*
	Copyright (c) 2008 Robin Vobruba <hoijui.quaero@gmail.com>

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
*/

#ifndef _BRAINSPACE_BRAIN_H
#define _BRAINSPACE_BRAIN_H

// generated by the C++ Wrapper scripts
#include "AICallback.h"
#include <string>
#include <stdarg.h>

namespace brainSpace {

/**
 * This is the main C++ entry point of this AI.
 * 
 * @author	Robin Vobruba <hoijui.quaero@gmail.com>
 */
class BrAIn {

private:
	springai::AICallback* callback;
	int teamId;

public:
	BrAIn(springai::AICallback* callback);
	~BrAIn();

	int HandleEvent(int topic, const void* data);

	void ChatMsg(const char* msg, ...);
	void ChatMsg(std::string msg);

}; // class BrAIn

} // namespace brainSpace

#endif // _BRAINSPACE_BRAIN_H
