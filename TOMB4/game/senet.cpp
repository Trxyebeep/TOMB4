#include "../tomb4/pch.h"
#include "senet.h"
#include "objects.h"
#include "items.h"
#include "deltapak.h"
#include "../specific/function_stubs.h"
#include "sound.h"
#include "control.h"
#include "lara_states.h"
#include "collide.h"
#include "tomb4fx.h"
#include "draw.h"
#include "../specific/3dmath.h"
#include "../specific/output.h"
#include "../specific/input.h"
#include "lara.h"

long SenetTargetX;
long SenetTargetZ;
short senet_item[6];
char piece_moving = -1;
char last_throw = 0;
char senet_board[17];
char senet_piece[6];

static short GameStixBounds[12] = { -256, 256, -200, 200, -256, 256, -1820, 1820, -5460, 5460, 0, 0 };
static PHD_VECTOR GameStixPos = { 0, 0, -100 };

void InitialiseSenet(short item_number)
{
	ITEM_INFO* item;
	short lp;

	if (senet_item[0])
		return;

	for (lp = 0; lp < 6; lp++)
		senet_piece[lp] = 0;

	for (lp = 1; lp < 17; lp++)
		senet_board[lp] = 0;

	senet_board[0] = 3;

	for (lp = 0; lp < level_items; lp++)
	{
		item = &items[lp];

		switch (item->object_number)
		{
		case GAME_PIECE1:
			senet_item[0] = lp;
			SenetTargetX = item->pos.x_pos + 1024;
			SenetTargetZ = item->pos.z_pos;
			break;

		case GAME_PIECE2:
			senet_item[1] = lp;
			break;

		case GAME_PIECE3:
			senet_item[2] = lp;
			break;

		case ENEMY_PIECE:
			senet_item[item->trigger_flags + 3] = lp;
			break;
		}
	}
}

void MakeMove(long piece, long displacement)
{
	short num, spot, lp;

	spot = short(senet_piece[piece] + displacement);
	num = (piece >= 3) + 1;

	if (senet_piece[piece] == NO_ITEM || !displacement || spot > 16 || senet_board[spot] & num)
		return;

	senet_board[senet_piece[piece]] &= ~num;

	if (!senet_piece[piece])
	{
		for (lp = 3 * (piece >= 3); lp < short(3 * (piece >= 3)) + 3; lp++)
		{
			if (lp != piece && !senet_piece[lp])
				senet_board[senet_piece[piece]] |= num;
		}
	}

	senet_piece[piece] = (char)spot;
	piece_moving = (char)piece;

	if (spot > 4)
	{
		senet_board[spot] = 0;

		for (lp = 3 - 3 * (piece >= 3); lp < short(3 - 3 * (piece >= 3)) + 3; lp++)
		{
			if (senet_piece[lp] == spot)
			{
				senet_piece[lp] = 0;
				senet_board[0] |= 3 - num;
			}
		}
	}

	if (spot >= 16)
		senet_piece[piece] = -1;
	else
		senet_board[spot] |= num;

	if (spot & 3 && last_throw != 6)
		last_throw = -1;
	else
		last_throw = 0;
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
	long lp;

	if (ourPiece == 1)
	{
		for (lp = 0; lp < 3; lp++)
		{
			if (senet_piece[lp] != NO_ITEM)
				return 0;
		}

		trigger_item_in_room(0, RAISING_BLOCK2);
		trigger_item_in_room(19, RAISING_BLOCK2);
		return 1;
	}
	else
	{
		for (lp = 3; lp < 6; lp++)
		{
			if (senet_piece[lp] != NO_ITEM)
				return 0;
		}

		trigger_item_in_room(20, TRAPDOOR1);
		trigger_item_in_room(21, TRAPDOOR1);
		trigger_item_in_room(22, TRAPDOOR1);
		trigger_item_in_room(81, TRAPDOOR1);
		return 1;
	}
}

void InitialiseGameStix(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	item->data = item->item_flags;
	item->trigger_flags = NO_ITEM;
}

void ThrowSticks(ITEM_INFO* item)
{
	long lp;
	char rnd;

	last_throw = 0;
	item->trigger_flags = 0;

	for (lp = 0; lp < 4; lp++)
	{
		rnd = GetRandomControl() & 1;
		last_throw += rnd;

		if (rnd)
			item->trigger_flags |= 1 << lp;
	}

	if (!last_throw)
		last_throw = 6;

	item->hit_points = 120;

	for (lp = 0; lp < 3; lp++)
		items[senet_item[lp]].trigger_flags = 1;
}

void GameStixControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* item2;
	ITEM_INFO* piece;
	long piece_num, num, x, z, change;
	short room_number;

	item = &items[item_number];

	if (item->trigger_flags > -1)
	{
		if (item->hit_points == 100)
			SoundEffect(SFX_SPINNING_PUZZLE, &item->pos, SFX_DEFAULT);

		for (int i = 0; i < 4; i++)
		{
			if (item->hit_points < 120 - (2 * i + 20))
			{
				item->item_flags[i] -= item->hit_points << 7;

				if (item->hit_points < 120 - (2 * i + 80))
				{
					if (item->item_flags[i] > -4096 && item->item_flags[i] < 4096 && item->trigger_flags & 1 << i)
						item->item_flags[i] = 0;
					else if ((item->item_flags[i] > 28672 || item->item_flags[i] < -28672) && !(item->trigger_flags & 1 << i))
						item->item_flags[i] = -0x8000;
				}
			}
		}

		item->hit_points--;

		if (!item->hit_points)
		{
			for (int i = 0; i < 3; i++)
				items[senet_item[i]].trigger_flags = 0;

			item->trigger_flags = NO_ITEM;

			if (piece_moving == NO_ITEM && !last_throw)
			{
				RemoveActiveItem(item_number);
				item->status = ITEM_INACTIVE;
			}
		}

		return;
	}

	if (piece_moving > -1)
	{
		num = (piece_moving >= 3) + 1;
		piece = &items[senet_item[piece_moving]];
		piece->flags |= IFL_TRIGGERED;
		piece->after_death = 48;
		piece_num = senet_piece[piece_moving];

		if (piece_num == -1)
			piece_num = 16;

		if (piece_num >= 5)
		{
			x = SenetTargetX + 1024;
			z = SenetTargetZ + ((piece_num - 5) << 10);
		}
		else
		{
			x = ((num - 1) << 11) + SenetTargetX;
			z = SenetTargetZ + ((4 - piece_num) << 10);
		}

		if (abs(x - piece->pos.x_pos) < 128)
			piece->pos.x_pos = x;
		else if (x > piece->pos.x_pos)
			piece->pos.x_pos += 128;
		else
			piece->pos.x_pos -= 128;

		if (abs(z - piece->pos.z_pos) < 128)
			piece->pos.z_pos = z;
		else if (z > piece->pos.z_pos)
			piece->pos.z_pos += 128;
		else
			piece->pos.z_pos -= 128;
		
		room_number = piece->room_number;
		GetFloor(piece->pos.x_pos, piece->pos.y_pos - 32, piece->pos.z_pos, &room_number);

		if (piece->room_number != room_number)
			ItemNewRoom(senet_item[piece_moving], room_number);

		if (x == piece->pos.x_pos && z == piece->pos.z_pos)
		{
			piece->after_death = 0;

			if (piece_num == 16)
			{
				if (num == 1)
				{
					ShockwaveExplosion(piece, 0x6060E0, -32);
					ShockwaveExplosion(piece, 0x6060E0, 48);
				}
				else
				{
					ShockwaveExplosion(piece, 0xFF8020, -32);
					ShockwaveExplosion(piece, 0xFF8020, 48);
				}

				KillItem(senet_item[piece_moving]);

				if (CheckSenetWinner(num))
				{
					for (int i = 0; i < level_items; i++)
					{
						item2 = &items[i];

						if (item2->object_number >= GAME_PIECE1 && item2->object_number <= WHEEL_OF_FORTUNE)
						{
							item2->flags |= IFL_INVISIBLE | IFL_CODEBITS;
							RemoveActiveItem(i);
							item2->status = ITEM_INACTIVE;
							item2->after_death = 1;
						}
					}
				}
			}
			else
			{
				for (int i = 0; i < 6; i++)
				{
					if (piece_moving != i)
					{
						piece = &items[senet_item[i]];

						if (x == piece->pos.x_pos && z == piece->pos.z_pos)
						{
							if (num == 1)
								ShockwaveExplosion(piece, 0xFF8020, -64);
							else
								ShockwaveExplosion(piece, 0x6060E0, -64);

							piece->pos.x_pos = SenetTargetX + ((2 - num) << 12) - 1024;
							piece->pos.z_pos = SenetTargetZ + ((i % 3) << 10);

							room_number = piece->room_number;
							GetFloor(piece->pos.x_pos, piece->pos.y_pos - 32, piece->pos.z_pos, &room_number);

							if (piece->room_number != room_number)
								ItemNewRoom(senet_item[i], room_number);

							if (num == 1)
								ShockwaveExplosion(piece, 0xFF8020, -64);
							else
								ShockwaveExplosion(piece, 0x6060E0, -64);
						}
					}
				}
			}

			if (!last_throw)
			{
				RemoveActiveItem(item_number);
				piece->status = ITEM_INACTIVE;
			}

			piece_moving = -1;
		}

		return;
	}

	if (last_throw == -1)
	{
		ThrowSticks(item);

		for (int i = 3; i < 6; i++)
		{
			MakeMove(i, last_throw);

			if (last_throw == -1 || !last_throw)
				break;
		}

		if (last_throw && last_throw != 6)
			last_throw = 0;
		else
			last_throw = -1;
	}
	else if (!last_throw)
	{
		ThrowSticks(item);
		change = 0;

		for (int i = 0; i < 3; i++)
		{
			if (senet_piece[i] != -1 && last_throw && senet_piece[i] + last_throw < 17 && !(senet_board[senet_piece[i] + last_throw] & 1))
				change = 1;
		}

		if (!change)
		{
			if (last_throw == 6)
				last_throw = 0;
			else
				last_throw = -1;
		}
	}
}

void GameStixCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (input & IN_ACTION && l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH && lara.gun_status == LG_NO_ARMS && !item->active ||
		lara.IsMoving && lara.GeneralPtr == (void*)item_number)
	{
		item->pos.y_rot ^= 0x8000;

		if (TestLaraPosition(GameStixBounds, item, l))
		{
			if (MoveLaraPosition(&GameStixPos, item, l))
			{
				l->anim_number = ANIM_THROWSTIX;
				l->frame_number = anims[ANIM_THROWSTIX].frame_base;
				l->current_anim_state = AS_CONTROLLED;
				lara.IsMoving = 0;
				lara.head_x_rot = 0;
				lara.head_y_rot = 0;
				lara.torso_x_rot = 0;
				lara.torso_y_rot = 0;
				lara.gun_status = LG_HANDS_BUSY;
				item->status = ITEM_ACTIVE;
				AddActiveItem(item_number);
			}
			else
				lara.GeneralPtr = (void*)item_number;
		}

		item->pos.y_rot ^= 0x8000;
	}
	else
		ObjectCollision(item_number, l, coll);
}

void ShockwaveExplosion(ITEM_INFO* item, ulong col, long speed)
{
	PHD_VECTOR pos;
	long InnerOuter;

	item->pos.y_pos -= 384;

	if (speed < 0)
		InnerOuter = 0x2000280;
	else
		InnerOuter = 0xA00020;

	pos.x = item->pos.x_pos;
	pos.y = item->pos.y_pos;
	pos.z = item->pos.z_pos;
	TriggerShockwave(&pos, InnerOuter, speed, col | 0x18000000, 0);
	TriggerShockwave(&pos, InnerOuter, speed, col | 0x18000000, 0x2000);
	TriggerShockwave(&pos, InnerOuter, speed, col | 0x18000000, 0x4000);
	TriggerShockwave(&pos, InnerOuter, speed, col | 0x18000000, 0x6000);
	item->pos.y_pos += 384;
}

void ControlGodHead(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (TriggerActive(item))
	{
		switch (item->pos.y_rot)
		{
		case 0:
			item->pos.z_pos &= ~1023;
			break;

		case 0x4000:
			item->pos.x_pos &= ~1023;
			break;

		case -0x8000:
			item->pos.z_pos |= 1023;
			break;

		case -0x4000:
			item->pos.x_pos |= 1023;
			break;
		}

		if (item->item_flags[0])
		{
			if (item->item_flags[2])
				item->item_flags[2]--;
			else if (item->item_flags[1] < 128)
				KillItem(item_number);
			else
				item->item_flags[1] -= 128;
		}
		else if (item->item_flags[1] < 4096)
			item->item_flags[1] += 128;
		else
		{
			item->item_flags[0] = 1;
			item->item_flags[1] = 4096;
			item->item_flags[2] = 210;
		}
	}
}

void DrawGodHead(ITEM_INFO* item)
{
	ROOM_INFO* r;
	PHD_VECTOR scalar;
	short** meshpp;
	short* frm[2];
	long rate, oldAlpha, alpha;

	r = &room[item->room_number];
	phd_left = r->left;
	phd_right = r->right;
	phd_top = r->top;
	phd_bottom = r->bottom;
	GetFrames(item, frm, &rate);

	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	phd_RotX(item->pos.x_rot);
	phd_RotZ(item->pos.z_rot);
	phd_RotY(item->pos.y_rot);
	rate = S_GetObjectBounds(frm[0]);

	if (rate)
	{
		meshpp = &meshes[objects[item->object_number].mesh_index];
		scalar.x = 0x4000;
		scalar.y = 0x4000;
		scalar.z = item->item_flags[1] << 2;
		ScaleCurrentMatrix(&scalar);
		CalculateObjectLighting(item, frm[0]);
		oldAlpha = GlobalAlpha;
		alpha = item->item_flags[1] >> 5;

		if (alpha < 128)
			alpha <<= 1;
		else
			alpha = 255;

		GlobalAlpha = alpha << 24;
		phd_PutPolygons(*meshpp, rate);
		GlobalAlpha = oldAlpha;
	}

	phd_left = 0;
	phd_right = phd_winwidth;
	phd_top = 0;
	phd_bottom = phd_winheight;
	phd_PopMatrix();
}
