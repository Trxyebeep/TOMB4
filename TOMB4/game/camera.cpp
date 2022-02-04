#include "../tomb4/pch.h"
#include "camera.h"
#include "../specific/3dmath.h"
#include "../specific/function_stubs.h"
#include "control.h"

void InitialiseCamera()
{
	last_target.x = lara_item->pos.x_pos;
	camera.target.x = last_target.x;
	camera.shift = lara_item->pos.y_pos - 1024;
	last_target.y = camera.shift;
	camera.target.y = camera.shift;
	last_target.z = lara_item->pos.z_pos;
	camera.target.z = last_target.z;
	camera.pos.y = camera.shift;
	last_target.room_number = lara_item->room_number;
	camera.target.room_number = lara_item->room_number;
	camera.pos.x = last_target.x;
	camera.pos.z = last_target.z - 100;
	camera.pos.room_number = lara_item->room_number;
	camera.target_distance = 1536;
	camera.item = 0;
	camera.number_frames = 1;
	camera.type = CHASE_CAMERA;
	camera.speed = 1;
	camera.flags = 0;
	camera.bounce = 0;
	camera.number = -1;
	camera.fixed_camera = 0;
	AlterFOV(14560);
	UseForcedFixedCamera = 0;
	CalculateCamera();
}

void MoveCamera(GAME_VECTOR* ideal, long speed)
{
	FLOOR_INFO* floor;
	GAME_VECTOR temp1;
	GAME_VECTOR temp2;
	long height, ceiling, shake, rndval, wx, wy, wz, dx, dy, dz;
	short room_number;

	if (BinocularOn < 0)
	{
		speed = 1;
		BinocularOn++;
	}

	if (old_cam.pos.x_rot == lara_item->pos.x_rot && old_cam.pos.y_rot == lara_item->pos.y_rot && old_cam.pos.z_rot == lara_item->pos.z_rot &&
		old_cam.pos2.x_rot == lara.head_x_rot && old_cam.pos2.y_rot == lara.head_y_rot && old_cam.pos2.x_pos == lara.torso_x_rot &&
		old_cam.pos2.y_pos == lara.torso_y_rot && old_cam.pos.x_pos == lara_item->pos.x_pos && old_cam.pos.y_pos == lara_item->pos.y_pos &&
		old_cam.pos.z_pos == lara_item->pos.z_pos && old_cam.current_anim_state == lara_item->current_anim_state &&
		old_cam.goal_anim_state == lara_item->goal_anim_state && old_cam.target_distance == camera.target_distance &&
		old_cam.target_elevation == camera.target_elevation && old_cam.target_angle == camera.target_angle &&
		camera.old_type == camera.type && BinocularOn >= 0)
	{
		ideal->x = last_ideal.x;
		ideal->y = last_ideal.y;
		ideal->z = last_ideal.z;
		ideal->room_number = last_ideal.room_number;
	}
	else
	{
		old_cam.pos.x_rot = lara_item->pos.x_rot;
		old_cam.pos.y_rot = lara_item->pos.y_rot;
		old_cam.pos.z_rot = lara_item->pos.z_rot;
		old_cam.pos2.x_rot = lara.head_x_rot;
		old_cam.pos2.y_rot = lara.head_y_rot;
		old_cam.pos2.x_pos = lara.torso_x_rot;
		old_cam.pos2.y_pos = lara.torso_y_rot;
		old_cam.pos.x_pos = lara_item->pos.x_pos;
		old_cam.pos.y_pos = lara_item->pos.y_pos;
		old_cam.pos.z_pos = lara_item->pos.z_pos;
		old_cam.current_anim_state = lara_item->current_anim_state;
		old_cam.goal_anim_state = lara_item->goal_anim_state;
		old_cam.target_distance = camera.target_distance;
		old_cam.target_elevation = camera.target_elevation;
		old_cam.target_angle = camera.target_angle;
		last_ideal.x = ideal->x;
		last_ideal.y = ideal->y;
		last_ideal.z = ideal->z;
		last_ideal.room_number = ideal->room_number;
	}

	camera.pos.x += (ideal->x - camera.pos.x) / speed;
	camera.pos.y += (ideal->y - camera.pos.y) / speed;
	camera.pos.z += (ideal->z - camera.pos.z) / speed;
	camera.pos.room_number = ideal->room_number;

	if (camera.bounce)
	{
		if (camera.bounce <= 0)
		{
			rndval = -camera.bounce;
			shake = rndval >> 1;
			camera.target.x += GetRandomControl() % rndval - shake;
			camera.target.y += GetRandomControl() % rndval - shake;
			camera.target.z += GetRandomControl() % rndval - shake;
			camera.bounce += 5;
		}
		else
		{
			camera.pos.y += camera.bounce;
			camera.target.y += camera.bounce;
			camera.bounce = 0;
		}
	}

	wz = camera.pos.z;
	wy = camera.pos.y;
	wx = camera.pos.x;
	room_number = camera.pos.room_number;
	floor = GetFloor(wx, wy, wz, &room_number);
	height = GetHeight(floor, wx, wy, wz);
	ceiling = GetCeiling(floor, wx, wy, wz);

	if (wy < ceiling || wy > height)
	{
		mgLOS(&camera.target, &camera.pos, 0);
		dx = ABS(camera.pos.x - ideal->x);
		dy = ABS(camera.pos.y - ideal->y);
		dz = ABS(camera.pos.z - ideal->z);

		if (dx < 768 && dy < 768 && dz < 768)
		{
			temp1.x = camera.pos.x;
			temp1.y = camera.pos.y;
			temp1.z = camera.pos.z;
			temp1.room_number = camera.pos.room_number;
			temp2.x = ideal->x;
			temp2.y = ideal->y;
			temp2.z = ideal->z;
			temp2.room_number = ideal->room_number;

			if (!(mgLOS(&temp2, &temp1, 0)))
			{
				camerasnaps++;

				if (camerasnaps >= 8)
				{
					camera.pos.x = ideal->x;
					camera.pos.y = ideal->y;
					camera.pos.z = ideal->z;
					camera.pos.room_number = ideal->room_number;
					camerasnaps = 0;
				}
			}
		}
	}

	wz = camera.pos.z;
	wy = camera.pos.y;
	wx = camera.pos.x;
	room_number = camera.pos.room_number;
	floor = GetFloor(wx, wy, wz, &room_number);
	height = GetHeight(floor, wx, wy, wz);
	ceiling = GetCeiling(floor, wx, wy, wz);

	if (wy - 255 < ceiling && height < wy + 255 && ceiling < height && ceiling != NO_HEIGHT && height != NO_HEIGHT)
		camera.pos.y = (height + ceiling) >> 1;
	else if (height < wy + 255 && ceiling < height && ceiling != NO_HEIGHT && height != NO_HEIGHT)
		camera.pos.y = height - 255;
	else if (wy - 255 < ceiling && ceiling < height && ceiling != NO_HEIGHT && height != NO_HEIGHT)
		camera.pos.y = ceiling + 255;
	else if (ceiling >= height || height == NO_HEIGHT || ceiling == NO_HEIGHT)
	{
		camera.pos.x = ideal->x;
		camera.pos.y = ideal->y;
		camera.pos.z = ideal->z;
		camera.pos.room_number = ideal->room_number;
	}

	GetFloor(camera.pos.x, camera.pos.y, camera.pos.z, &camera.pos.room_number);
	phd_LookAt(camera.pos.x, camera.pos.y, camera.pos.z, camera.target.x, camera.target.y, camera.target.z, 0);

	if (camera.mike_at_lara)
	{
		camera.mike_pos.x = lara_item->pos.x_pos;
		camera.mike_pos.y = lara_item->pos.y_pos;
		camera.mike_pos.z = lara_item->pos.z_pos;
	}
	else
	{
		dx = camera.target.x - camera.pos.x;
		dz = camera.target.z - camera.pos.z;
		dx = phd_atan(dz, dx);
		camera.actual_elevation = (short)dx;
		camera.mike_pos.x = camera.pos.x + (phd_sin(dx) * phd_persp >> 14);
		camera.mike_pos.y = camera.pos.y;
		camera.mike_pos.z = camera.pos.z + (phd_cos(dx) * phd_persp >> 14);
	}

	camera.old_type = camera.type;
}

long mgLOS(GAME_VECTOR* start, GAME_VECTOR* target, long push)
{
	FLOOR_INFO* floor;
	long x, y, z, h, c, cdiff, hdiff, dx, dy, dz, clipped, nc, i;
	short room_number, room_number2;

	dx = (target->x - start->x) >> 3;
	dy = (target->y - start->y) >> 3;
	dz = (target->z - start->z) >> 3;
	x = start->x;
	y = start->y;
	z = start->z;
	room_number = start->room_number;
	room_number2 = room_number;
	nc = 0;
	clipped = 0;

	for (i = 0; i < 8; i++)
	{
		room_number = room_number2;
		floor = GetFloor(x, y, z, &room_number2);
		h = GetHeight(floor, x, y, z);
		c = GetCeiling(floor, x, y, z);

		if (h == NO_HEIGHT || c == NO_HEIGHT || c >= h)
		{
			if (!nc)
			{
				x += dx;
				y += dy;
				z += dz;
				continue;
			}

			clipped = 1;
			break;
		}

		if (y > h)
		{
			hdiff = y - h;

			if (hdiff < push)
				y = h;
			else
			{
				clipped = 1;
				break;
			}
		}

		if (y < c)
		{
			cdiff = c - y;

			if (cdiff < push)
				y = c;
			else
			{
				clipped = 1;
				break;
			}
		}

		nc = 1;
		x += dx;
		y += dy;
		z += dz;
	}

	if (i)
	{
		x -= dx;
		y -= dy;
		z -= dz;
	}

	target->x = x;
	target->y = y;
	target->z = z;
	GetFloor(x, y, z, &room_number);
	target->room_number = room_number;
	return !clipped;
}

void inject_camera(bool replace)
{
	INJECT(0x00442E70, InitialiseCamera, replace);
	INJECT(0x00442F40, MoveCamera, replace);
	INJECT(0x004451C0, mgLOS, replace);
}
