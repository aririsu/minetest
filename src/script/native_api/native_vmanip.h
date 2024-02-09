//Native version of vmanip class.
//Erik Companhone.

#pragma once

#include <map>
#include "irr_v3d.h"
#include "lua_api/l_base.h"

//class ModApiBase;

class NativeLuaVoxelManip : public ModApiBase
{
private:
	//Native versions
	static MMVManip* native_read_from_map(v3s16 a, v3s16 b, LuaVoxelManip *o);
	static u32 native_get_data(LuaVoxelManip *o);
	static u32 native_set_data(LuaVoxelManip *o, u32 i, content_t c);
	static int native_write_to_map(LuaVoxelManip *o, bool update_light);

	static MapNode native_get_node_at(LuaVoxelManip *o);
	static int native_set_node_at(LuaVoxelManip *o, v3s16 pos, MapNode n);

	static int native_update_map();
	static int native_update_liquids(LuaVoxelManip *o, NodeDefManager *ndef, Map *map, Mapgen mg);

	static v3s16 native_calc_lighting(LuaVoxelManip *o, std::string x, v3s16 pmin, v3s16 pmax, Mapgen *mg, bool propagate_shadow);
	static int native_set_lighting(LuaVoxelManip *o, v3s16 pmin, v3s16 pmax, Mapgen *mg);
	static u32 NativeLuaVoxelManip::native_get_light_data(LuaVoxelManip *o);
	static u32 NativeLuaVoxelManip::native_set_light_data(LuaVoxelManip *o, u32 i, u8 light);

	static u32 native_get_param2_data(LuaVoxelManip *o, u32 i);
	static u32 NativeLuaVoxelManip::native_set_param2_data(LuaVoxelManip *o, u32 i, u8 param2);

	static MMVManip* NativeLuaVoxelManip::native_was_modified(LuaVoxelManip *o);
	static v3s16 NativeLuaVoxelManip::native_get_emerged_area(LuaVoxelManip *o, int x);

	friend class LuaVoxelManip;
};
