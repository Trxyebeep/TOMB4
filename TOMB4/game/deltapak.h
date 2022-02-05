#pragma once
#include "../global/vars.h"

void inject_deltapack(bool replace);

void handle_cutseq_triggering(long name);
void do_new_cutscene_camera();
void InitPackNodes(NODELOADHEADER* lnode, PACKNODE* pnode, char* packed, long numnodes);
void DecodeAnim(PACKNODE* node, long num_nodes, long frame, long flags);
short DecodeTrack(char* packed, RTDECODE* decode);
short GetTrackWord(long off, char* packed, long packmethod);
void frigup_lara();
void updateAnimFrame(PACKNODE* node, int flags, short* frame);
void* cutseq_malloc(long size);
void finish_cutseq();
void DrawCutSeqActors();
void CalcActorLighting(ITEM_INFO* item, OBJECT_INFO* obj, short* rot);
void GetJointAbsPositionCutSeq(ITEM_INFO* item, OBJECT_INFO* obj, short* rot, PHD_VECTOR* pos);
void CalculateObjectLightingLaraCutSeq();
void third_cutseq_control();
void fourth_cutseq_init();
void fourth_cutseq_control();
void fourth_cutseq_end();
void fifth_cutseq_control();
void fifth_cutseq_end();
void sixth_cutseq_init();
void sixth_cutseq_control();
void sixth_cutseq_end();
void seventh_cutseq_init();
void seventh_control();
void eighth_control();
void ninth_cutseq_init();
void ninth_control();
void tenth_cutseq_init();
void tenth_cutseq_control();
void tenth_cutseq_end();
void eleventh_cutseq_init();
void eleventh_cutseq_control();
void twelth_cutseq_init();
void twelth_cutseq_end();
void thirteen_end();
void fourteen_end();
void fourteen_control();
void fifteen_init();
void fifteen_control();
void fifteen_end();
void sixteen_init();
void sixteen_control();
void sixteen_end();
void seventeen_control();
void seventeen_end();
void eighteen_init();
void eighteen_control();
void eighteen_end();
void nineteen_init();
void nineteen_control();
void nineteen_end();
void twenty_control();
void twenty_end();
void twentyone_init();
void twentyone_end();
void twentythree_init();
void twentythree_control();
void twentythree_end();
void twentyfour_init();
void twentyfour_control();
void twentyfour_end();
void twentyfive_control();
void twentyfive_end();
void twentyfive_init();
void twentysix_control();
void twentyseven_init();
void twentyseven_control();
void twentyseven_end();
void twentytwo_init();
void twentytwo_end();
void do_spade_meshswap();
void do_key_meshswap();
void cutseq_shoot_pistols(long left_or_right);
void trigger_weapon_dynamics(long left_or_right);
void deal_with_pistols();

#define Load_and_Init_Cutseq	( (long(__cdecl*)(long)) 0x0046CD20 )
#define handle_actor_chatting	( (void(__cdecl*)(long, long, long, long, short*)) 0x0046D270 )
#define trigger_item_in_room	( (void(__cdecl*)(long, long)) 0x0046D350 )
#define untrigger_item_in_room	( (void(__cdecl*)(long, long)) 0x0046D3D0 )
#define DelsHandyTeleportLara	( (void(__cdecl*)(long, long, long, long)) 0x0046D040 )
#define init_voncroy_meshbits	( (void(__cdecl*)(long)) 0x0046D030 )
#define handle_lara_chatting	( (void(__cdecl*)(short*)) 0x0046D1F0 )
#define cutseq_kill_item	( (void(__cdecl*)(long)) 0x0046CC40 )
#define cutseq_restore_item	( (void(__cdecl*)(long)) 0x0046CCB0 )
#define nail_intelligent_object	( (void(__cdecl*)(short)) 0x0046D160 )
#define find_a_fucking_item	( (ITEM_INFO*(__cdecl*)(long)) 0x0046D450 )
