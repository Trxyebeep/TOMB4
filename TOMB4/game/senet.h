#pragma once
#include "../global/vars.h"

void inject_senet(bool replace);

void InitialiseSenet(short item_number);
void MakeMove(long piece, long displacement);
void SenetControl(short item_number);
long CheckSenetWinner(long won);
void InitialiseGameStix(short item_number);
void ThrowSticks(ITEM_INFO* item);
void GameStixControl(short item_number);
void GameStixCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void ShockwaveExplosion(ITEM_INFO* item, ulong col, long speed);
