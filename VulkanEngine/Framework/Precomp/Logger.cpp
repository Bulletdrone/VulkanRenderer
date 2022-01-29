#include "precomp.h"
#include "Logger.h"

#include <assert.h>
#include <iostream>

namespace Engine
{
	/*  Check for unintented behaviour, if a_Check is false the program will stop and post a message.
		@param a_Check, If false the program will print the message and assert.
		@param a_Msg, The message that will be printed. */
	void Logger::Assert(bool a_Check, const char* a_Msg)
	{
		if (!a_Check)
		{
			printf(a_Msg);
			assert(false);
		}
	}
}