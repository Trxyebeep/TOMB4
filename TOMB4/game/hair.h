#pragma once
#include "../global/types.h"

void InitialiseHair();
void HairControl(long in_cutscene, long pigtail, short* cutscenething);
void GetCorrectStashPoints(long pigtail, long hair_node, long skin_node);
void DrawHair();

extern HAIR_STRUCT hairs[2][7];
