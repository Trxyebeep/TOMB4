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
		sprintf(buf, "footprints");
		tomb4.footprints = 1;							//footprints on
		REG_WriteBool(buf, tomb4.footprints);

		sprintf(buf, "shadow");
		tomb4.shadow_mode = 3;							//PSX like shadow
		REG_WriteLong(buf, tomb4.shadow_mode);

		sprintf(buf, "crawltilt");
		tomb4.crawltilt = 1;							//crawl tilt on
		REG_WriteBool(buf, tomb4.crawltilt);
	}
	else	//Key already exists, settings already written, read them. also falls back to default if any of them missing
	{
		sprintf(buf, "footprints");
		REG_ReadBool(buf, tomb4.footprints, 1);

		sprintf(buf, "shadow");
		REG_ReadLong(buf, tomb4.shadow_mode, 3);

		sprintf(buf, "crawltilt");
		REG_ReadBool(buf, tomb4.crawltilt, 1);
	}

	CloseRegistry();
}

void save_new_tomb4_settings()
{
	char buf[40];

	OpenRegistry("tomb4");

	sprintf(buf, "footprints");
	REG_WriteBool(buf, tomb4.footprints);

	sprintf(buf, "shadow");
	REG_WriteLong(buf, tomb4.shadow_mode);

	sprintf(buf, "crawltilt");
	REG_WriteLong(buf, tomb4.crawltilt);

	CloseRegistry();
}
