#include "native_item.h"
#include <tuple>

// NativeItemStack function definitions

int NativeItemStack::native_gc_object(NativeItemStack *item)
{
    delete item;
    return 0;
}

std::string NativeItemStack::native_mt_tostring(ItemStack* item)
{
    return item->getItemString(false);
}

bool NativeItemStack::native_is_empty(ItemStack* item)
{
    return item->empty();
}

std::string NativeItemStack::native_get_name(ItemStack* item)
{
    return item->name;
}

bool NativeItemStack::native_set_name(ItemStack* item, std::string name)
{
    bool status = true;

    item->name = name;

    if (item->name.empty() || item->empty()) {
        item->clear();
        status = false;
    }

    return status;
}

int NativeItemStack::native_get_count(ItemStack* item)
{
    return item->count;
}

bool NativeItemStack::native_set_count(ItemStack* item, int count)
{
    bool status = true;

    if (count > 0 && count <= 65535) {
        item->count = count;
    } else {
        item->clear();
        status = false;
    }

    return status;
}

int NativeItemStack::native_get_wear(ItemStack* item)
{
    return item->wear;
}

bool NativeItemStack::native_set_wear(ItemStack* item, int wear)
{
    bool status = true;

    if (wear <= 65535) {
        item->wear = wear;
    } else {
        item->clear();
        status = false;
    }

    return status;
}

// TODO: implement native_get_meta

std::string NativeItemStack::native_get_description(ItemStack* item, IItemDefManager* idef)
{
    return item->getDescription(idef);
}

std::string NativeItemStack::native_get_short_description(ItemStack* item, IItemDefManager* idef)
{
    return item->getShortDescription(idef);
}

bool NativeItemStack::native_clear(ItemStack* item)
{
    item->clear();
    return true;
}

// TODO: I am unsure if this is the correct way to do this.
bool NativeItemStack::native_replace(ItemStack* item, ItemStack* new_item)
{
    bool status = true;

    if (new_item == nullptr) {
        new_item = new ItemStack();
    }

    item = new_item;

    return true;
}

std::string NativeItemStack::native_to_string(ItemStack* item)
{
    return item->getItemString();
}

// TODO: implement native_to_table
// I am not sure how to implement it

int NativeItemStack::native_get_stack_max(ItemStack* item, IItemDefManager* idef)
{
    return item->getStackMax(idef);
}


int NativeItemStack::native_get_free_space(ItemStack* item, IItemDefManager* idef)
{
    return item->freeSpace(idef);
}

bool NativeItemStack::native_is_known(ItemStack* item, IItemDefManager* idef)
{
    return item->isKnown(idef);
}

ToolCapabilities NativeItemStack::native_get_tool_capabilities(ItemStack* item, IItemDefManager* idef)
{
    return item->getToolCapabilities(idef);
}

// TODO: implement native_get_definition

bool NativeItemStack::native_add_wear(ItemStack* item, int amount, IItemDefManager* idef)
{
    return item->addWear(amount, idef);
}

ItemStack NativeItemStack::native_add_item(ItemStack* item, ItemStack* new_item, IItemDefManager* idef)
{
    return item->addItem(*new_item, idef);
}

std::tuple<bool, ItemStack*> NativeItemStack::native_item_fits(ItemStack* item, ItemStack* new_item, IItemDefManager* idef)
{
    ItemStack *rest_item;
    bool fits = item->itemFits(*new_item, rest_item, idef);

    return std::tuple<bool, ItemStack*>(fits, rest_item);
}

ItemStack NativeItemStack::native_take_item(ItemStack* item, int count)
{
    return item->takeItem(count);
}

ItemStack NativeItemStack::native_peek_item(ItemStack* item, int count)
{
    return item->peekItem(count);
}

NativeItemStack::NativeItemStack(const ItemStack &item):
	item(item)
{
}

const ItemStack& NativeItemStack::getItem() const
{
    return item;
}

ItemStack& NativeItemStack::getItem()
{
    return item;
}

// NativeModApiItemMod function definitions

int NativeModApiItemMod::native_register_item_raw(Server *server, std::string name,
		std::string description, std::string short_description,
		std::string inventory_image, std::string inventory_overlay,
		std::string wield_image, std::string wield_overlay,
		std::string palette_image, std::string feature_name, bool on_construct,
		bool on_destruct, bool after_destruct, bool rightclickable)
{

    IWritableItemDefManager *idef = server->getWritableItemDefManager();
    NodeDefManager *ndef = server->getWritableNodeDefManager();

    ItemDefinition def;

	// Set a distinctive default value to check if this is set
    def.node_placement_prediction = "__default";

    // TODO: this line is from common/c_content.cpp. It takes in lua state,
    // and then a bunch of other parameters from the stack. This makes the function 
    // signature very large.
    //
	// read_item_definition(L, table, def, def);

    // Assign the item definition values to the def object
    // TODO: I am not sure this is the best method, maybe add a new constructor to ItemDefinition
    def.name = name;
    def.description = description;
    def.short_description = short_description;
    def.inventory_image = inventory_image;
    def.inventory_overlay = inventory_overlay;
    def.wield_image = wield_image;
    def.wield_overlay = wield_overlay;
    def.palette_image = palette_image;

    if (def.node_placement_prediction == "__default") {
        if (def.type == ITEM_NODE) {
            def.node_placement_prediction = name;
        } else {
            def.node_placement_prediction = "";
        }
    }

    idef->registerItem(def);

	// Read the node definition (content features) and register it
    if (def.type == ITEM_NODE) {
        ContentFeatures f;

        // TODO: again, this is something that is rather a large function in c_content.cpp,
        // making this function signature really large.
        //
        // read_content_features(L, f, table)

        // feature fields
        f.name = feature_name;
        f.has_on_construct = on_construct;
        f.has_on_destruct = on_destruct;
        f.has_after_destruct = after_destruct;
        f.rightclickable = rightclickable;

        // TODO: feature group fields
        // lua calls read_groups(L, -1, f.groups); Not sure how to handle this one

;

		// when a mod reregisters ignore, only texture changes and such should
		// be done
		if (f.name == "ignore") {
            return 0;
        }

        content_t id = ndef->set(f.name, f);

        if (id > MAX_REGISTERED_CONTENT) {
            // TODO: this won't be a lua error anymore. Unsure which kind to throw, however.
            // for now, I will use ModError
            throw ModError("Number of registerable nodes ("
                + itos(MAX_REGISTERED_CONTENT+1)
                + ") exceeded (" + name + ")");
        }
    }

    return 0; /* number of results */
}

int NativeModApiItemMod::native_unregister_item_raw(Server *server, std::string name)
{
    IWritableItemDefManager *idef = server->getWritableItemDefManager();
	
    // Unregister the node
    if (idef->get(name).type == ITEM_NODE) {
        NodeDefManager *ndef = server->getWritableNodeDefManager();
        ndef->removeNode(name);
    }

    idef->unregisterItem(name);

    return 0;
}

int NativeModApiItemMod::native_register_alias_raw(Server *server, std::string name,
        std::string convert_to)
{
    IWritableItemDefManager *idef = server->getWritableItemDefManager();

    idef->registerAlias(name, convert_to);

    return 0;
}

content_t NativeModApiItemMod::native_get_content_id(IItemDefManager *idef, const NodeDefManager *ndef, std::string name)
{
    // If this is called at mod load time, NodeDefManager isn't aware of
	// aliases yet, so we need to handle them manually
    std::string alias_name = idef->getAlias(name);

    content_t content_id;

    if (alias_name != name) {
        if (!ndef->getId(alias_name, content_id)) {
            throw ModError("Unknown node: " + alias_name +
					" (from alias " + name + ")");
        } else if (!ndef->getId(name, content_id)) {
            throw ModError("Unknown node: " + name);
        }
    }

    return content_id;
}

const char* NativeModApiItemMod::native_get_name_from_content_id(const NodeDefManager *ndef, content_t c)
{
    return ndef->get(c).name.c_str();
}