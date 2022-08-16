#pragma once
#include "../global/types.h"

void inject_train(bool replace);

void DrawTrainObjects();
void DrawTrainFloor();
void InitialiseTrainJeep(short item_number);
void TrainJeepControl(short item_number);

extern long trainmappos;
