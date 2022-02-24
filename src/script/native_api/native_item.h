#pragma once

#include "inventory.h"  // ItemStack
#include "tool.h" 
#include "server.h"
#include "nodedef.h"
#include "itemdef.h"
#include "log.h"

// TODO: should this still be static? I feel like I could just refactor the class methods
// to instead grab item from this (i.e. this->item). Not sure why LuaItemStack has all private 
// methods as static, but I'm not sure if it's a good idea to change that so until then I'm
// going to keep it static.
class NativeItemStack {
// private:
public:
	ItemStack item;

	static int native_gc_object(NativeItemStack* item);
	static std::string native_mt_tostring(ItemStack* item);

	static bool native_is_empty(ItemStack* item);
	static std::string native_get_name(ItemStack* item);
	static bool native_set_name(ItemStack* item, std::string name);
	static int native_get_count(ItemStack* item);
	static bool native_set_count(ItemStack* item, int count);
	static int native_get_wear(ItemStack* item);
	static bool native_set_wear(ItemStack* item, int wear);
	// TODO: implement native_get_meta, blocked until l_itemstackmeta.h is native implemented
	static std::string native_get_description(ItemStack* item, IItemDefManager* idef);
	static std::string native_get_short_description(ItemStack* item, IItemDefManager* idef);
	static bool native_clear(ItemStack* item);
	static bool native_replace(ItemStack* item, ItemStack* new_item);
	static std::string native_to_string(ItemStack* item);

	// TODO: is this function necessary for a native class?
	// UNKNOWN_RETURN native_to_table(ItemStack* item);
	// I am not sure how to implement it

	static int native_get_stack_max(ItemStack* item, IItemDefManager* idef);
	static int native_get_free_space(ItemStack* item, IItemDefManager* idef);
	static bool native_is_known(ItemStack* item, IItemDefManager* idef);

	// TODO: I did not know how to do this one
	// UNKNOWN_RETURN native_get_definition(ItemStack* item);

	static ToolCapabilities native_get_tool_capabilities(ItemStack* item, IItemDefManager* idef);
	static bool native_add_wear(ItemStack* item, int amount, IItemDefManager* idef);
	static ItemStack native_add_item(ItemStack* item, ItemStack* new_item, IItemDefManager* idef);
	// TODO: fix this function implementation
	static std::tuple<bool, ItemStack*> native_item_fits(ItemStack* item, ItemStack* new_item, IItemDefManager* idef);
	static ItemStack native_take_item(ItemStack* item, int count);
	static ItemStack native_peek_item(ItemStack* item, int count);

// public:
	NativeItemStack(const ItemStack& item);
	// TODO: what does this do?
	~NativeItemStack() = default;

	const ItemStack& getItem() const;
	ItemStack& getItem();

	// TODO: is this necessary for native functions?
	// // LuaItemStack(itemstack or itemstring or table or nil)
	// // Creates an LuaItemStack and leaves it on top of stack
	// static int create_object(lua_State *L);

	// TODO: not sure how to implement this
	// Not callable from Lua
	// static int create(const ItemStack &item);

	// TODO: is this necessary for native functions?
	// static LuaItemStack* checkobject(lua_State *L, int narg);
	// static void Register(lua_State *L);
};

class NativeModApiItemMod {
public:
	static int native_register_item_raw(Server *server, std::string name, 
		std::string description, std::string short_description, std::string inventory_image, 
		std::string inventory_overlay, std::string wield_image, std::string wield_overlay,
		std::string palette_image, std::string feature_name, bool on_construct,
		bool on_destruct, bool after_destruct, bool rightclickable 
	);

	static int native_unregister_item_raw(Server *server, std::string name);
	static int native_register_alias_raw(Server *server, std::string name, std::string convert_to);
	static content_t native_get_content_id(IItemDefManager *idef, const NodeDefManager *ndef, std::string name);
	static const char* native_get_name_from_content_id(const NodeDefManager *ndef, content_t c);

public:
	// TODO: determine if this is necessary, it looks like this is just for lua
	// static void Initialize(lua_State *L, int top);
};