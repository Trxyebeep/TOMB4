#pragma once
#include "../global/vars.h"

void inject_hair(bool replace);

void InitialiseHair();
void HairControl(long in_cutscene, long pigtail, short* cutscenething);
void GetCorrectStashPoints(long pigtail, long hair_node, long skin_node);
void DrawHair();
