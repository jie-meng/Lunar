#ifndef UTIL_LUAX_HPP
#define UTIL_LUAX_HPP

#include <vector>
#include "cmdline.hpp"
#include "any.hpp"
#include "typeinfo.hpp"

struct lua_State;

namespace util
{

typedef int (*LuaCFunc)(lua_State*);

enum LuaType
{
    LuaNone = -1,
    LuaNil,
    LuaBoolean,
    LuaLightUserData,
    LuaNumber,
    LuaString,
    LuaTable,
    LuaFunction,
    LuaUserData,
    LuaThread,
    LuaNumTags
};

//#define LUA_TNONE		(-1)
//
//#define LUA_TNIL		0
//#define LUA_TBOOLEAN		1
//#define LUA_TLIGHTUSERDATA	2
//#define LUA_TNUMBER		3
//#define LUA_TSTRING		4
//#define LUA_TTABLE		5
//#define LUA_TFUNCTION		6
//#define LUA_TUSERDATA		7
//#define LUA_TTHREAD		8
//
//#define LUA_NUMTAGS		9

//get operate
double luaGetDouble(lua_State* plua_state, int index);
double luaGetDouble(lua_State* plua_state, int index, double default_num);
int luaGetInteger(lua_State* plua_state, int index);
int luaGetInteger(lua_State* plua_state, int index, int default_int);
std::string luaGetString(lua_State* plua_state, int index);
std::string luaGetString(lua_State* plua_state, int index, const std::string& default_str);
bool luaGetBoolean(lua_State* plua_state, int index);
bool luaGetBoolean(lua_State* plua_state, int index, bool default_bool);
void* luaGetLightUserData(lua_State* plua_state, int index);
void* luaGetLightUserData(lua_State* plua_state, int index, void* default_data);
any luaGetAny(lua_State* plua_state, int index);
std::vector< std::pair<any, any> > luaGetTable(lua_State* plua_state, int index);

//push operate
void luaPushDouble(lua_State* plua_state, double double_val);
void luaPushInteger(lua_State* plua_state, int int_val);
void luaPushString(lua_State* plua_state, const std::string& str_val);
void luaPushNil(lua_State* plua_state);
void luaPushBoolean(lua_State* plua_state, bool boolean);
void luaPushLightUserData(lua_State* plua_state, void* p);
void luaPushAny(lua_State* plua_state, const any& a);
void luaPushTable(lua_State* plua_state, const std::vector< std::pair<any, any> >& key_value_vec);
void luaPushTable(lua_State* plua_state, const std::vector<any>& vec);

template <typename Coll>
void luaPushTable(lua_State* plua_state, const Coll& coll)
{
    std::vector<any> any_vec;
    typename Coll::const_iterator it;
    for (it = coll.begin(); it != coll.end(); ++it)
        any_vec.push_back(any(*it));
    luaPushTable(plua_state, any_vec);
}

//other operate
LuaType luaGetType(lua_State* plua_state, int index);
std::string luaGetTypeName(lua_State* plua_state, LuaType type);
void luaPop(lua_State* plua_state, int index);
int luaGetTop(lua_State* plua_state);
void luaGetGlobal(lua_State* plua_state, const std::string& name);
int luaCallFunc(lua_State* plua_state, int nargs, int nrets);
void luaAssert(lua_State* plua_state, bool assertion, const std::string& str);
void luaError(lua_State* plua_state, const std::string& str);

std::string luaGetError(lua_State* plua_state, int err);
int luaParseLine(lua_State* plua_state, const std::string& line, std::string& error_str);
int luaParseFile(lua_State* plua_state, const std::string& file, std::string& error_str);

void* luaNewUserData(lua_State* plua_state, size_t sz);
void luaSetMetaTable(lua_State* plua_state, const std::string& tname);
void* luaCheckUData(lua_State* plua_state, int ud, const std::string& tname);
int luaFileresult(lua_State* plua_state, bool stat, const std::string& fname);

//LuaState
class LuaState
{
public:
    LuaState();
    ~LuaState();
    inline lua_State* getState() const { return plua_state_; }
    inline std::string getError() const { return error_str; }
    void registerFunction(const std::string& func_name, LuaCFunc lua_reg_func);
    int parseLine(const std::string& line);
    int parseFile(const std::string& file);
    bool reset();
private:
    bool init();
    void cleanup();
    bool loadLibs();
private:
    lua_State* plua_state_;
    std::string error_str;
private:
    DISALLOW_COPY_AND_ASSIGN(LuaState)
};

class LuaCmdLine : public CommonCmdLine
{
public:
    LuaCmdLine(const std::string& prompt = ">>>",
               const std::string& quit_cmd = "quit",
               bool is_case_sensitive = false);
    virtual ~LuaCmdLine();

    inline lua_State* getState() const { return lua_state_.getState(); }
    void registerFunction(const std::string& func_name, LuaCFunc lua_reg_func);
    virtual int parseFile(const std::string& file);
    virtual void process(const std::string& cmd);
    virtual void clear();
private:
    LuaState lua_state_;
private:
    DISALLOW_COPY_AND_ASSIGN(LuaCmdLine)
};

// C++ cal lua function
//int main(int argc, char* argv[])
//{
//    LuaState ls;
//    ls.parseFile("base.lua");
//    luaGetGlobal(ls.getState(), "add");
//    luaSetInteger(ls.getState(), 6);
//    luaSetInteger(ls.getState(), 9);
//    int err = luaCallFunc(ls.getState(), 2, 1);
//    if (err != 0)
//    {
//        printLine(luaGetError(ls.getState(), err));
//        return -1;
//    }
//
//    printLine("");
//    printLine("cnt = %d", luaGetTop(ls.getState()));
//    for(int i = 1; i<=luaGetTop(ls.getState()); ++i)
//    {
//        printLine(luaGetInteger(ls.getState(), i));
//    }
//    luaPop(ls.getState(), -1);
//}

} // namespace util

#endif // UTIL_LUAX_HPP
