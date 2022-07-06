#include "../tomb4/pch.h"
#include "senet.h"
#include "objects.h"
#include "items.h"
#include "deltapak.h"
#include "../specific/function_stubs.h"

void InitialiseSenet(short item_number)
{
	ITEM_INFO* item;

	if (senet_item[0])
		return;

	memset(senet_board, 0, 17);
	memset(senet_piece, 0, 6);
	senet_board[0] = 3;

	for (int i = 0; i < level_items; i++)
	{
		item = &items[i];

		switch (item->object_number)
		{
		case GAME_PIECE1:
			senet_item[0] = i;
			SenetTargetX = item->pos.x_pos + 1024;
			SenetTargetZ = item->pos.z_pos;
			break;

		case GAME_PIECE2:
			senet_item[1] = i;
			break;

		case GAME_PIECE3:
			senet_item[2] = i;
			break;

		case ENEMY_PIECE:
			senet_item[item->trigger_flags + 3] = i;
			break;
		}
	}
}

void MakeMove(long piece, long displacement)
{
	long num;

	num = piece >= 3 ? 2 : 1;

	if (senet_piece[piece] == NO_ITEM || !displacement ||
		senet_piece[piece] + displacement > 16 || senet_board[senet_piece[piece] + displacement] & num)
		return;

	senet_board[senet_piece[piece]] &= ~num;

	if (!senet_piece[piece])
	{
		for (int i = 3 * num - 3; i < 3 * num; i++)
		{
			if (i != piece && !senet_piece[i])
				senet_board[0] |= num;
		}
	}

	piece_moving = (char)piece;
	senet_piece[piece_moving] += (char)displacement;

	if (senet_piece[piece_moving] > 4)
	{
		senet_board[senet_piece[piece_moving]] = 0;

		for (int i = 6 - 3 * num; i < 9 - 3 * num; i++)
		{
			if (senet_piece[i] == senet_piece[piece_moving])
			{
				senet_piece[i] = 0;
				senet_board[0] |= 3 - num;
			}
		}
	}

	if (!(senet_piece[piece_moving] & 3) || last_throw == 6)
		last_throw = 0;
	else
		last_throw = -1;

	if (senet_piece[piece_moving] < 16)
		senet_board[senet_piece[piece_moving]] |= num;
	else
		senet_piece[piece_moving] = -1;
}

void SenetControl(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (last_throw > 0 && item->trigger_flags != 1)
		MakeMove(item->object_number - GAME_PIECE1, last_throw);

	RemoveActiveItem(item_number);
}

long CheckSenetWinner(long ourPiece)
{
	long num;

	if (ourPiece == 1)
	{
		num = 0;

		while (senet_piece[num] == NO_ITEM)			//if our piece is dead
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

		while (senet_piece[num] == NO_ITEM)
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

void InitialiseGameStix(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	item->trigger_flags = NO_ITEM;
	item->data = item->item_flags;
}

void ThrowSticks(ITEM_INFO* item)
{
	char rnd;

	last_throw = 0;
	item->trigger_flags = 0;

	for (int i = 0; i < 4; i++)
	{
		rnd = GetRandomControl() & 1;
		last_throw += rnd;

		if (rnd)
			item->trigger_flags |= 1 << i;
	}

	if (!last_throw)
		last_throw = 6;

	item->hit_points = 120;

	for (int i = 0; i < 3; i++)
		items[senet_item[i]].trigger_flags = 1;
}

void inject_senet(bool replace)
{
	INJECT(0x0040F3B0, InitialiseSenet, replace);
	INJECT(0x0040F4D0, MakeMove, replace);
	INJECT(0x0040F480, SenetControl, replace);
	INJECT(0x0040F320, CheckSenetWinner, replace);
	INJECT(0x0040F600, InitialiseGameStix, replace);
	INJECT(0x0040FC60, ThrowSticks, replace);
}
