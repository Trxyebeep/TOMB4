#include "../tomb4/pch.h"
#include "senet.h"
#include "objects.h"
#include "items.h"
#include "deltapak.h"

void InitialiseSenet(short item_number)
{
	ITEM_INFO* item;

	if (SenetPieceNumbers[0])
		return;

	memset(SenetBoard, 0, 17);
	memset(ActiveSenetPieces, 0, 6);
	SenetBoard[0] = 3;

	for (int i = 0; i < level_items; i++)
	{
		item = &items[i];

		switch (item->object_number)
		{
		case GAME_PIECE1:
			SenetPieceNumbers[0] = i;
			SenetTargetX = item->pos.x_pos + 1024;
			SenetTargetZ = item->pos.z_pos;
			break;

		case GAME_PIECE2:
			SenetPieceNumbers[1] = i;
			break;

		case GAME_PIECE3:
			SenetPieceNumbers[2] = i;
			break;

		case ENEMY_PIECE:
			SenetPieceNumbers[item->trigger_flags + 3] = i;
			break;
		}
	}
}

void MakeMove(long piece, long displacement)
{
	long num;

	num = piece >= 3 ? 2 : 1;

	if (ActiveSenetPieces[piece] == NO_ITEM || !displacement ||
		ActiveSenetPieces[piece] + displacement > 16 || SenetBoard[ActiveSenetPieces[piece] + displacement] & num)
		return;

	SenetBoard[ActiveSenetPieces[piece]] &= ~num;

	if (!ActiveSenetPieces[piece])
	{
		for (int i = 3 * num - 3; i < 3 * num; i++)
		{
			if (i != piece && !ActiveSenetPieces[i])
				SenetBoard[0] |= num;
		}
	}

	ActivePiece = (char)piece;
	ActiveSenetPieces[ActivePiece] += (char)displacement;

	if (ActiveSenetPieces[ActivePiece] > 4)
	{
		SenetBoard[ActiveSenetPieces[ActivePiece]] = 0;

		for (int i = 6 - 3 * num; i < 9 - 3 * num; i++)
		{
			if (ActiveSenetPieces[i] == ActiveSenetPieces[ActivePiece])
			{
				ActiveSenetPieces[i] = 0;
				SenetBoard[0] |= 3 - num;
			}
		}
	}

	if (!(ActiveSenetPieces[ActivePiece] & 3) || SenetDisplacement == 6)
		SenetDisplacement = 0;
	else
		SenetDisplacement = -1;

	if (ActiveSenetPieces[ActivePiece] < 16)
		SenetBoard[ActiveSenetPieces[ActivePiece]] |= num;
	else
		ActiveSenetPieces[ActivePiece] = -1;
}

void SenetControl(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (SenetDisplacement > 0 && item->trigger_flags != 1)
		MakeMove(item->object_number - GAME_PIECE1, SenetDisplacement);

	RemoveActiveItem(item_number);
}

long CheckSenetWinner(long ourPiece)
{
	long num;

	if (ourPiece == 1)
	{
		num = 0;

		while (ActiveSenetPieces[num] == NO_ITEM)			//if our piece is dead
		{
			num++;

			if (num >= 3)									//all 3 dead?
			{
				trigger_item_in_room(0, RAISING_BLOCK2);	//lara winer!
				trigger_item_in_room(19, RAISING_BLOCK2);
				return 1;
			}
		}
	}
	else
	{
		num = 3;

		while (ActiveSenetPieces[num] == NO_ITEM)
		{
			num++;

			if (num >= 6)									//all 3 dead?
			{
				trigger_item_in_room(20, TRAPDOOR1);		//semerkhet winer!	
				trigger_item_in_room(21, TRAPDOOR1);
				trigger_item_in_room(22, TRAPDOOR1);
				trigger_item_in_room(81, TRAPDOOR1);
				return 1;
			}
		}
	}

	return 0;
}

void inject_senet(bool replace)
{
	INJECT(0x0040F3B0, InitialiseSenet, replace);
	INJECT(0x0040F4D0, MakeMove, replace);
	INJECT(0x0040F480, SenetControl, replace);
	INJECT(0x0040F320, CheckSenetWinner, replace);
}
