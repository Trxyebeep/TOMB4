#include "../tomb4/pch.h"
#include "tomb4.h"
#include "../specific/registry.h"

tomb4_options tomb4;

void init_tomb4_stuff()
{
	char buf[40];
	bool first;

	OpenRegistry("tomb4");
	first = REG_KeyWasCreated();

	if (first)	//key was created = no settings found, write defaults
	{

	}
	else	//Key already exists, settings already written, read them. also falls back to default if any of them missing
	{

	}

	CloseRegistry();
}

void save_new_tomb4_settings()
{
	char buf[40];

	OpenRegistry("tomb5");

	CloseRegistry();
}
