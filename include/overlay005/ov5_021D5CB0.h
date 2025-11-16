#ifndef POKEPLATINUM_OV5_021D5CB0_H
#define POKEPLATINUM_OV5_021D5CB0_H

#ifdef PLATFORM_DS
#include <nnsys.h>
#else
#include "platform/platform_types.h"
#include "nns_types.h"  /* SDL: All NNSG2d/NNSG3d/GX types */
#endif

#include "overlay005/struct_ov5_021D5CB0_decl.h"

UnkStruct_ov5_021D5CB0 *ov5_021D5CB0(void);
int ov5_021D5CE4(UnkStruct_ov5_021D5CB0 *param0, NNSG3dResTex *param1);
void ov5_021D5DEC(UnkStruct_ov5_021D5CB0 *param0);
void ov5_021D5E5C(UnkStruct_ov5_021D5CB0 *param0, int param1);
void ov5_021D5E8C(UnkStruct_ov5_021D5CB0 *param0);
void ov5_021D5EAC(UnkStruct_ov5_021D5CB0 *param0);

#endif // POKEPLATINUM_OV5_021D5CB0_H
