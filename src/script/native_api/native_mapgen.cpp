#include "native_mapgen.h"

int NativeMapGen::native_get_biome_id(const char *biomeName, BiomeManager *bmgr)
{
	if (!biomeName)
		return -1;

	const Biome *biome = (Biome *)bmgr->getByName(biomeName);
	if (!biome || biome->index == OBJDEF_INVALID_INDEX)
		return -1;

	return biome->index;
}

const char *NativeMapGen::native_get_biome_name(BiomeManager *bmgr, int ID)
{
	const Biome *b = (Biome *)bmgr->getRaw(ID);
	return b->name.c_str();
}

float NativeMapGen::native_get_heat(
		MapSettingsManager *msm, const BiomeManager *bmgr, v3s16 pos)
{
	NoiseParams np_heat;
	NoiseParams np_heat_blend;

	if (!msm->getMapSettingNoiseParams("mg_biome_np_heat", &np_heat) ||
			!msm->getMapSettingNoiseParams(
					"mg_biome_np_heat_blend", &np_heat_blend))
		return -1.0f;

	std::string value;
	if (!msm->getMapSetting("seed", &value))
		return -1.0f;

	std::istringstream ss(value);
	u64 seed;
	ss >> seed;

	if (!bmgr)
		return -1.0f;

	return bmgr->getHeatAtPosOriginal(pos, np_heat, np_heat_blend, seed);
}

float NativeMapGen::native_get_humidity(
		MapSettingsManager *msm, const BiomeManager *bmgr, v3s16 pos)
{
	NoiseParams np_humidity;
	NoiseParams np_humidity_blend;

	if (!msm->getMapSettingNoiseParams("mg_biome_np_humidity", &np_humidity) ||
			!msm->getMapSettingNoiseParams(
					"mg_biome_np_humidity_blend", &np_humidity_blend))
		return -1.0f;

	std::string value;
	if (!msm->getMapSetting("seed", &value))
		return 0;
	std::istringstream ss(value);
	u64 seed;
	ss >> seed;

	if (!bmgr)
		return -1.0f;

	return bmgr->getHumidityAtPosOriginal(pos, np_humidity, np_humidity_blend, seed);
}

std::vector<int> NativeMapGen::native_get_biome_data(
		MapSettingsManager *msm, const BiomeManager *bmgr, v3s16 pos)
{
	std::vector<int> biomeData;

	NoiseParams np_heat;
	NoiseParams np_heat_blend;
	NoiseParams np_humidity;
	NoiseParams np_humidity_blend;

	if (!msm->getMapSettingNoiseParams("mg_biome_np_heat", &np_heat) ||
			!msm->getMapSettingNoiseParams(
					"mg_biome_np_heat_blend", &np_heat_blend) ||
			!msm->getMapSettingNoiseParams(
					"mg_biome_np_humidity", &np_humidity) ||
			!msm->getMapSettingNoiseParams(
					"mg_biome_np_humidity_blend", &np_humidity_blend))
		return biomeData;

	std::string value;
	if (!msm->getMapSetting("seed", &value))
		return biomeData;
	std::istringstream ss(value);
	u64 seed;
	ss >> seed;

	if (!bmgr)
		return biomeData;

	float heat = bmgr->getHeatAtPosOriginal(pos, np_heat, np_heat_blend, seed);
	float humidity = bmgr->getHumidityAtPosOriginal(
			pos, np_humidity, np_humidity_blend, seed);

	if (!heat || !humidity)
		return biomeData;

	const Biome *biome = bmgr->getBiomeFromNoiseOriginal(heat, humidity, pos);
	if (!biome || biome->index == OBJDEF_INVALID_INDEX)
		return biomeData;

	biomeData.push_back(biome->index);
	biomeData.push_back(heat);
	biomeData.push_back(humidity);
}

// Template helper for get_mapgen_object

template <typename T1, typename T2>
auto func(T1 a, T2 b)
{
	return std::make_tuple(a, b);
}

// GO BACK TO THIS
template <typename T1, typename T2>
auto NativeMapGen::native_get_mapgen_object(const char *objName, EmergeManager *emerge)
{
	auto dummy = func(-1, -1);
	// Create a dummy array

	int mgobjint;
	if (!string_to_enum(ModApiMapgen::es_MapgenObject, mgobjint,
			    objName ? objName : ""))
		return dummy;

	enum MapgenObject mgobj = (MapgenObject)mgobjint;

	Mapgen *mg = emerge->getCurrentMapgen();

	switch (mgobj) {

	case MGOBJ_VMANIP: {
		MMVManip *vm = mg->vm;
		return func(vm->m_area.MinEdge, vm->m_area.MaxEdge);
	}
	case MGOBJ_HEIGHTMAP: {
		if (!mg->heightmap)
			return dummy;

		return func(mg->heightmap, 2);
	}
	case MGOBJ_BIOMEMAP: {
		if (!mg->biomegen)
			return dummy;

		return func(mg->biomegen, 3);
	}
	case MGOBJ_HEATMAP: {
		if (!mg->biomegen || mg->biomegen->getType() != BIOMEGEN_ORIGINAL)
			return dummyTuple;

		BiomeGenOriginal *bg = (BiomeGenOriginal *)mg->biomegen;
		return func(bg, 4);
	}
	case MGOBJ_HUMIDMAP: {

		if (!mg->biomegen || mg->biomegen->getType() != BIOMEGEN_ORIGINAL)
			return dummyTuple;

		BiomeGenOriginal *bgo2 = (BiomeGenOriginal *)mg->biomegen;
		return func(bgo2, 5);
	}
	case MGOBJ_GENNOTIFY: {

		std::map<std::string, std::vector<v3s16>> event_map;
		mg->gennotify.getEvents(event_map);

		return func(eventMap, 6);
	}
	}

	enum MapgenObject mgobj = (MapgenObject)mgobjint;
}

int NativeMapGen::native_get_spawn_level(EmergeManager *emerge, s16 xPos, s16 zPos)
{
	int spawn_level = emerge->getSpawnLevelAtPoint(v2s16(xPos, zPos));
	return spawn_level;
}

std::tuple<std::string, u64, int, int, std::string>
NativeMapGen::native_get_mapgen_params(MapSettingsManager *msm)
{
	std::string s1, s5;
	int s3, s4;
	u64 s2;
	std::string value;

	msm->getMapSetting("mg_name", &value);
	s1 = value.c_str();

	msm->getMapSetting("seed", &value);
	std::istringstream ss(value);
	u64 seed;
	ss >> seed;
	s2 = seed;

	msm->getMapSetting("water_level", &value);
	s3 = stoi(value, -32768, 32767);

	msm->getMapSetting("chunksize", &value);
	s4 = stoi(value, -32768, 32767);

	msm->getMapSetting("mg_flags", &value);
	s5 = value.c_str();

	return std::make_tuple(s1, s2, s3, s4, s5);
}

// CANNOT BE DONE?
std::vector<std::string> NativeMapGen::native_set_mapgen_params()
{
	return std::vector<std::string>();
}

std::string NativeMapGen::native_get_mapgen_setting(
		MapSettingsManager *msm, const char *name)
{
	std::string value;
	return std::string();

	if (!msm->getMapSetting(name, &value))
		return nullptr;

	return value.c_str();
}

void NativeMapGen::native_set_mapgen_setting(MapSettingsManager *msm, const char *name,
		const char *value, bool override)
{
	if (!msm->setMapSetting(name, value, override)) {
		errorstream << "set_mapgen_setting: cannot set '" << name
			    << "' after initialization" << std::endl;
	}
}

/*
NoiseParams &NativeMapGen::native_get_mapgen_setting_noiseparams(const char *name)
{
	// TODO: insert return statement here
	return
}
*/

void NativeMapGen::native_set_mapgen_setting_noiseparams(
		MapSettingsManager *msm, const char *name, NoiseParams np, bool override)
{

	if (!msm->setMapSettingNoiseParams(name, &np, override)) {
		errorstream << "set_mapgen_setting_noiseparams: cannot set '" << name
			    << "' after initialization" << std::endl;
	}
}

void NativeMapGen::native_set_noiseparams(
		const char *name, NoiseParams np, bool set_default)
{
	Settings::getLayer(set_default ? SL_DEFAULTS : SL_GLOBAL)
			->setNoiseParams(name, np);
}

NoiseParams &NativeMapGen::native_get_noiseparams(std::string name)
{
	NoiseParams np;
	if (g_settings->getNoiseParams(name, np))
		return np;
}
// RETURN
int NativeMapGen::native_register_decoration()
{
	return 0;
}

int NativeMapGen::native_register_ore()
{
	return 0;
}

u32 NativeMapGen::native_register_schematic(
		Schematic *sch, SchematicManager *schemmgr, StringMap names)
{
	ObjDefHandle handle = schemmgr->add(sch);
	if (handle == OBJDEF_INVALID_HANDLE) {
		delete sch;
		return -1;
	}
	return handle;
}

void NativeMapGen::native_clear_registered_biomes(BiomeManager *bmgr)
{
	bmgr->clear();
}

void NativeMapGen::native_clear_registered_decorations(DecorationManager *dmgr)
{
	dmgr->clear();
}

void NativeMapGen::native_clear_registered_schematics(SchematicManager *smgr)
{
	smgr->clear();
}

// RETURN, private field
void NativeMapGen::native_generate_ores(
		EmergeManager *emerge, Mapgen mg, v3s16 pMin, v3s16 pMax, u32 oreID)
{

	u32 blockseed = Mapgen::getBlockSeed(pMin, mg.seed);
	// emerge->oremgr->placeAllOres(&mg, blockseed, pMin, pMax);
}

// RETURN, private field
void NativeMapGen::native_generate_decorations(
		EmergeManager *emerge, Mapgen mg, v3s16 pMin, v3s16 pMax, u32 oreID)
{
}

void NativeMapGen::native_clear_registered_ores(OreManager *omgr)
{
	omgr->clear();
}

void NativeMapGen::native_create_schematic(const NodeDefManager *ndef, Map *map, v3s16 p1,
		v3s16 p2, std::vector<std::pair<v3s16, u8>> probList,
		const char *filename, std::vector<std::pair<s16, u8>> slice_prob_list)
{
	Schematic schem;

	if (!schem.getSchematicFromMap(map, p1, p2)) {
		errorstream << "create_schematic: failed to get schematic "
			       "from map"
			    << std::endl;
		// return 0;
	}

	schem.applyProbabilities(p1, &probList, &slice_prob_list);

	schem.saveSchematicToFile(filename, ndef);
	actionstream << "create_schematic: saved schematic file '" << filename << "'."
		     << std::endl;
}

void NativeMapGen::native_place_schematic(
		Schematic *sch, ServerMap *map, v3s16 p, int rotation, bool FP, u32 flags)
{
	sch->placeOnMap(map, p, flags, (Rotation)rotation, FP);
}

bool NativeMapGen::native_place_schematic_on_vmanip(MMVManip *vm, Schematic *sch,
		ServerMap *map, v3s16 p, int rotation, bool FP, u32 flags)
{
	bool sch_did_fit = sch->placeOnVManip(vm, p, flags, (Rotation)rotation, FP);
	return sch_did_fit;
}

std::string NativeMapGen::native_serialize_schematic(
		Schematic *schem, int format, bool useComm,bool wasLoaded, u32 indSpaces)
{
	std::ostringstream os(std::ios_base::binary);
	switch (format) {
	case SCHEM_FMT_MTS:
		schem->serializeToMts(&os, schem->m_nodenames);
		break;
	case SCHEM_FMT_LUA:
		schem->serializeToLua(&os, schem->m_nodenames, useComm, indSpaces);
		break;
	default:
		return 0;
	}

	if (wasLoaded)
		delete schem;

	std::string ser = os.str();
	return ser;
}

//IDK??
void NativeMapGen::native_read_schematic(Schematic *schem, std::string writeYslice)
{

}
