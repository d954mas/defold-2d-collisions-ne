// myextension.cpp
// myextension.cpp
// Extension lib defines
#define EXTENSION_NAME Native
#define LIB_NAME "Native"
#define MODULE_NAME "native"
// include the Defold SDK
#include <dmsdk/sdk.h>
#include <math.h>
#include "spatial.hpp"

static void LuaInit(lua_State* L){
	int top = lua_gettop(L);
	// Register lua names
	Shape_register(L);
	assert(top == lua_gettop(L));
}

static dmExtension::Result AppInitializeMyExtension(dmExtension::AppParams* params){
	return dmExtension::RESULT_OK;
}

static dmExtension::Result InitializeMyExtension(dmExtension::Params* params){
	// Init Lua
	LuaInit(params->m_L);
	return dmExtension::RESULT_OK;
}

static dmExtension::Result AppFinalizeMyExtension(dmExtension::AppParams* params){
	return dmExtension::RESULT_OK;
}

static dmExtension::Result FinalizeMyExtension(dmExtension::Params* params){
	return dmExtension::RESULT_OK;
}

DM_DECLARE_EXTENSION(EXTENSION_NAME, LIB_NAME, AppInitializeMyExtension, AppFinalizeMyExtension, InitializeMyExtension, 0, 0, FinalizeMyExtension)