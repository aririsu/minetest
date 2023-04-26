#pragma once
#include "irr_v3d.h"
#include <vector>
#include <string>
#include "noise.h"
#include "../src/mapgen/mg_schematic.h"
#include "../src/mapgen/mg_biome.h"
#include "../src/mapgen/mg_ore.h"
#include "../src/settings.h"
#include "../src/emerge.h"
#include "../src/map_settings_manager.h"
#include "../src/script/common/c_content.h"
#include "../../src/emerge.h"
#include "../../src/script/lua_api/l_mapgen.h"

class NativeMapGen
{
	static int native_get_biome_id(const char *biomeName, BiomeManager *bmgr);

	// get_biome_name(biome_id)
	// returns the biome name string
	static const char *native_get_biome_name(BiomeManager *bmgr, int ID);

	// get_heat(pos)
	// returns the heat at the position
	static float native_get_heat(
			MapSettingsManager *msm, const BiomeManager *bmgr, v3s16 pos);

	// get_humidity(pos)
	// returns the humidity at the position
	static float native_get_humidity(
			MapSettingsManager *msm, const BiomeManager *bmgr, v3s16 pos);

	// get_biome_data(pos)
	// returns a table containing the biome id, heat and humidity at the position
	static std::vector<int> native_get_biome_data(
			MapSettingsManager *msm, const BiomeManager *bmgr, v3s16 pos);

	// get_mapgen_object(objectname)
	// returns the requested object used during map generation
	template <typename T1, typename T2>
	auto native_get_mapgen_object(const char *objName, EmergeManager *emerge);

	// get_spawn_level(x = num, z = num)
	static int native_get_spawn_level(EmergeManager *emerge, s16 xPos, s16 zPos);

	// get_mapgen_params()
	// returns the currently active map generation parameter set
	static std::tuple<std::string, u64, int, int, std::string>
	native_get_mapgen_params(MapSettingsManager *msm);

	// set_mapgen_params(params)
	// set mapgen parameters
	static std::vector<std::string> native_set_mapgen_params();

	// get_mapgen_setting(name)
	static std::string native_get_mapgen_setting(
			MapSettingsManager *msm, const char *name);

	// set_mapgen_setting(name, value, override_meta)
	static void native_set_mapgen_setting(MapSettingsManager *msm, const char *name,
			const char *value, bool override);

	// get_mapgen_setting_noiseparams(name)
	// static NoiseParams& native_get_mapgen_setting_noiseparams(const char *name);

	// set_mapgen_setting_noiseparams(name, value, override_meta)
	static void native_set_mapgen_setting_noiseparams(MapSettingsManager *msm,
			const char *name, NoiseParams np, bool override);

	// set_noiseparam_defaults(name, noiseparams, set_default)
	static void native_set_noiseparams(
			const char *name, NoiseParams np, bool set_defaults);

	// get_noiseparam_defaults(name)
	// static NoiseParams& native_get_noiseparams(std::string name);

	// RETURN
	// set_gen_notify(flags, {deco_id_table})

	// static void native_set_gen_notify();
	// RETURN
	// get_gen_notify()
	// static void native_get_gen_notify();

	// get_decoration_id(decoration_name)
	// returns the decoration ID as used in gennotify
	// static int native_get_decoration_id(const char* decName);

	// RETURN
	// register_biome({lots of stuff})
	// static int native_register_biome();

	static NoiseParams &native_get_noiseparams(std::string name);

	// register_decoration({lots of stuff})
	static int native_register_decoration();

	// register_ore({lots of stuff})
	static int native_register_ore();

	// register_schematic({schematic}, replacements={})
	static u32 native_register_schematic(
			Schematic *sch, SchematicManager *schemmgr, StringMap names);

	// clear_registered_biomes()
	static void native_clear_registered_biomes(BiomeManager *bmgr);

	// clear_registered_decorations()
	static void native_clear_registered_decorations(DecorationManager *dmgr);

	// clear_registered_schematics()
	static void native_clear_registered_schematics(SchematicManager *smgr);

	// generate_ores(vm, p1, p2)
	static void native_generate_ores(EmergeManager *emerge, Mapgen mg, v3s16 pMin,
			v3s16 pMax, u32 oreID);

	// generate_decorations(vm, p1, p2)
	static void native_generate_decorations(EmergeManager *emerge, Mapgen mg,
			v3s16 pMin, v3s16 pMax, u32 oreID);

	// clear_registered_ores
	static void native_clear_registered_ores(OreManager *omgr);

	// create_schematic(p1, p2, probability_list, filename)
	static void native_create_schematic(const NodeDefManager *ndef, Map *map,
			v3s16 p1, v3s16 p2, std::vector<std::pair<v3s16, u8>> probList,
			const char *filename,
			std::vector<std::pair<s16, u8>> slice_prob_list);

	// place_schematic(p, schematic, rotation,
	//     replacements, force_placement, flagstring)
	static void native_place_schematic(Schematic *sch, ServerMap *map, v3s16 p,
			int rotation, bool FP, u32 flags);

	// place_schematic_on_vmanip(vm, p, schematic, rotation,
	//     replacements, force_placement, flagstring)
	static bool native_place_schematic_on_vmanip(MMVManip *vm, Schematic *sch,
			ServerMap *map, v3s16 p, int rotation, bool FP, u32 flags);

	// serialize_schematic(schematic, format, options={...})
	static std::string native_serialize_schematic(Schematic *schem, int format,
			bool useComm, bool wasLoaded, u32 indSpaces);

	// read_schematic(schematic, options={...})
	static void native_read_schematic(Schematic *schem, std::string writeYslice);
};