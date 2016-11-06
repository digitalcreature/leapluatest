#include <cstdio>
#include "lua5.1/lua.hpp"
#include "Leap.h"

using namespace std;
using namespace Leap;

typedef lua_State *Lua;

int leap_load(Lua L) {
	lua_getglobal(L, "leap");
	lua_getfield(L, -1, "controller");
	if (lua_isnil(L, -1)) {
		Controller *controller = new Controller();
		lua_pushlightuserdata(L, controller);
		lua_setfield(L, 1, "controller");
		return 0;
	}
	else {
		lua_pushliteral(L, "leap has already been loaded");
		return 1;
	}
}

void pushvector(Lua L, const Vector &vec, float scale) {
	lua_createtable(L, 0, 3);
	lua_pushnumber(L, vec.x * scale); lua_setfield(L, -2, "x");
	lua_pushnumber(L, vec.y * scale); lua_setfield(L, -2, "y");
	lua_pushnumber(L, vec.z * scale); lua_setfield(L, -2, "z");
}
void pushvector(Lua L, const Vector &vec) { pushvector(L, vec, 0.1f); }

int leap_frame(Lua L) {
	lua_getglobal(L, "leap");
	lua_getfield(L, -1, "controller");
	Controller *controller = (Controller *) lua_touserdata(L, -1);
	lua_settop(L, 0);
	Frame frame = controller->frame();
	if (frame.isValid()) {
		lua_newtable(L);
		HandList hands = frame.hands();
		lua_createtable(L, hands.count(), 0);
		for (int h = 0; h < hands.count(); h ++) {
			Hand hand = hands[h];
			FingerList fingers = hand.fingers();
			lua_createtable(L, fingers.count(), 1);
			lua_pushinteger(L, hand.id()); lua_setfield(L, -2, "id");
			for (int f = 0; f < fingers.count(); f ++) {
				Finger finger = fingers[f];
				Vector tip = finger.tipPosition();
				lua_createtable(L, 5, 2);
				lua_pushinteger(L, finger.id()); lua_setfield(L, -2, "id");
				pushvector(L, tip); lua_setfield(L, -2, "tip");
				int b = 0;
				while (b < 4) {
					Bone::Type type = static_cast<Bone::Type>(b);
					Bone bone = finger.bone(type);
					if (bone.isValid()) {
						pushvector(L, bone.prevJoint()); lua_rawseti(L, -2, b + 1);
					}
					else {
						break;
					}
					b ++;
				}
				pushvector(L, tip); lua_rawseti(L, -2, b + 1);
				lua_rawseti(L, -2, f + 1);
			}
			lua_rawseti(L, -2, h + 1);
		}
		lua_setfield(L, -2, "hands");
		return 1;
	}
	else {
		return 0;
	}
}

extern "C" {
	int luaopen_leaplua(Lua L) {
		luaL_Reg lib[] = {
			{ "load", leap_load },
			{ "frame", leap_frame },
			{ NULL, NULL }
		};
		luaL_register(L, "leap", lib);
		return 1;
	}
}
