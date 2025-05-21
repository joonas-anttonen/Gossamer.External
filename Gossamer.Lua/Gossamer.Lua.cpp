#include <cstdint>

#include <lua.hpp>

typedef void *nint_t;

enum Severity
{
    Debug = -1,
    Info = 0,
    Warning = 1,
    Error = 2,
};

enum class Status
{
    Fault = -1,
    OK = 0,
    InvalidArgument = 1,
    InvalidData = 2,
    InvalidConversion = 3,
};

extern "C"
{
    __declspec(dllexport) int32_t luaOpen(lua_State **out_state)
    {
        lua_State *state = luaL_newstate();
        if (state == nullptr)
            return static_cast<int32_t>(Status::Fault);

        *out_state = state;
        return static_cast<int32_t>(Status::OK);
    }

    __declspec(dllexport) int32_t luaClose(lua_State *in_state)
    {
        if (in_state == nullptr)
            return static_cast<int32_t>(Status::InvalidArgument);

        lua_close(in_state);
        return static_cast<int32_t>(Status::OK);
    }

    __declspec(dllexport) int32_t luaRegisterApiFunction(
        lua_State *in_state,
        const uint8_t *in_module_name,
        const int32_t in_module_name_size,
        const uint8_t *in_function_name,
        const int32_t in_function_name_size,
        nint_t in_function)
    {
        if (in_state == nullptr)
            return static_cast<int32_t>(Status::InvalidArgument);

        if (in_module_name == nullptr || in_module_name_size <= 0)
            return static_cast<int32_t>(Status::InvalidArgument);
        if (in_function_name == nullptr || in_function_name_size <= 0)
            return static_cast<int32_t>(Status::InvalidArgument);
        if (in_function == 0)
            return static_cast<int32_t>(Status::InvalidArgument);

        lua_getglobal(in_state, (const char *)in_module_name);
        if (lua_isnoneornil(in_state, -1))
        {
            lua_pop(in_state, 1);
            lua_newtable(in_state);
            lua_setglobal(in_state, (const char *)in_module_name);
            lua_getglobal(in_state, (const char *)in_module_name);
        }

        lua_pushlstring(in_state, (const char *)in_function_name, in_function_name_size);
        lua_pushcfunction(in_state, static_cast<lua_CFunction>(in_function));
        lua_settable(in_state, -3);
        lua_pop(in_state, 1);

        return static_cast<int32_t>(Status::OK);
    }

    /*
     ** Message handler used to run all chunks
     */
    static int msghandler(lua_State *state)
    {
        const char *msg = lua_tostring(state, 1);
        if (msg == NULL)
        {                                                /* is error object not a string? */
            if (luaL_callmeta(state, 1, "__tostring") && /* does it have a metamethod */
                lua_type(state, -1) == LUA_TSTRING)      /* that produces a string? */
                return 1;                                /* that is the message */
            else
                msg = lua_pushfstring(state, "(error object is a %s value)",
                                      luaL_typename(state, 1));
        }
        luaL_traceback(state, state, msg, 1); /* append a standard traceback */

        lua_getglobal(state, "core");
        lua_getfield(state, -1, "log");
        if (lua_isfunction(state, -1))
        {
            lua_pushstring(state, lua_tostring(state, -3));
            lua_pushinteger(state, static_cast<int64_t>(Severity::Error));
            lua_call(state, 2, 0);
            lua_pop(state, 1);
        }
        else
        {
            lua_pop(state, 2);
        }

        return 1; /* return the traceback */
    }

    /*
    ** Interface to 'lua_pcall', which sets appropriate message function
    ** and C-signal handler. Used to run all chunks.
    */
    static int docall(lua_State *L, int narg, int nres)
    {
        int status;
        int base = lua_gettop(L) - narg;  /* function index */
        lua_pushcfunction(L, msghandler); /* push message handler */
        lua_insert(L, base);              /* put it under function and args */
        // globalL = L;                      /* to be available to 'laction' */
        // setsignal(SIGINT, laction);       /* set C-signal handler */
        status = lua_pcall(L, narg, nres, base);
        // setsignal(SIGINT, SIG_DFL); /* reset C-signal handler */
        lua_remove(L, base); /* remove message handler from the stack */
        return status;
    }

    __declspec(dllexport) int32_t luaPopInteger(lua_State *in_state, int64_t *out_integer)
    {
        if (out_integer == nullptr)
            return static_cast<int32_t>(Status::InvalidArgument);

        if (in_state == nullptr)
            return static_cast<int32_t>(Status::InvalidArgument);

        int is_integer = 0;
        lua_Integer i = lua_tointegerx(in_state, -1, &is_integer);
        if (is_integer)
        {
            *out_integer = i;
            lua_pop(in_state, 1);
            return static_cast<int32_t>(Status::OK);
        }
        else
        {
            *out_integer = 0;
            return static_cast<int32_t>(Status::InvalidConversion);
        }
    }

    __declspec(dllexport) int32_t luaPushInteger(lua_State *in_state, int64_t in_integer)
    {
        if (in_state == nullptr)
            return static_cast<int32_t>(Status::InvalidArgument);
        lua_pushinteger(in_state, static_cast<lua_Integer>(in_integer));
        return static_cast<int32_t>(Status::OK);
    }

    __declspec(dllexport) int32_t luaPopNumber(lua_State *in_state, double *out_number)
    {
        if (out_number == nullptr)
            return static_cast<int32_t>(Status::InvalidArgument);

        if (in_state == nullptr)
            return static_cast<int32_t>(Status::InvalidArgument);

        if (lua_isnumber(in_state, -1))
        {
            *out_number = lua_tonumber(in_state, -1);
            lua_pop(in_state, 1);
            return static_cast<int32_t>(Status::OK);
        }
        else
        {
            *out_number = 0;
            return static_cast<int32_t>(Status::InvalidConversion);
        }
    }

    __declspec(dllexport) int32_t luaPushNumber(lua_State *in_state, double in_number)
    {
        if (in_state == nullptr)
            return static_cast<int32_t>(Status::InvalidArgument);

        lua_pushnumber(in_state, in_number);
        return static_cast<int32_t>(Status::OK);
    }

    __declspec(dllexport) int32_t luaSetGlobal(lua_State *in_state, const uint8_t *in_name, int32_t in_name_size)
    {
        if (in_state == nullptr)
            return static_cast<int32_t>(Status::InvalidArgument);

        if (in_name == nullptr || in_name_size <= 0)
            return static_cast<int32_t>(Status::InvalidArgument);

        lua_setglobal(in_state, (const char *)in_name);
        return static_cast<int32_t>(Status::OK);
    }

    __declspec(dllexport) int32_t luaPushTable(lua_State *in_state)
    {
        if (in_state == nullptr)
            return static_cast<int32_t>(Status::InvalidArgument);

        lua_newtable(in_state);
        return static_cast<int32_t>(Status::OK);
    }

    __declspec(dllexport) int32_t luaWriteTable(lua_State *in_state)
    {
        if (in_state == nullptr)
            return static_cast<int32_t>(Status::InvalidArgument);

        lua_settable(in_state, -3);
        return static_cast<int32_t>(Status::OK);
    }

    __declspec(dllexport) int32_t luaPushFunction(lua_State *in_state, nint_t function)
    {
        if (in_state == nullptr)
            return static_cast<int32_t>(Status::InvalidArgument);

        lua_pushcfunction(in_state, static_cast<lua_CFunction>(function));
        return static_cast<int32_t>(Status::OK);
    }

    __declspec(dllexport) int32_t luaGetStackCount(lua_State *in_state, int32_t *out_count)
    {
        if (out_count == nullptr)
            return static_cast<int32_t>(Status::InvalidArgument);

        if (in_state == nullptr)
            return static_cast<int32_t>(Status::InvalidArgument);

        *out_count = lua_gettop(in_state);
        return static_cast<int32_t>(Status::OK);
    }

    __declspec(dllexport) int32_t luaPushString(lua_State *in_state, const uint8_t *in_string, int32_t in_string_size)
    {
        if (in_state == nullptr)
            return static_cast<int32_t>(Status::InvalidArgument);

        if (in_string == nullptr || in_string_size <= 0)
            return static_cast<int32_t>(Status::InvalidArgument);

        lua_pushlstring(in_state, (const char *)in_string, in_string_size);
        return static_cast<int32_t>(Status::OK);
    }

    __declspec(dllexport) int32_t luaPopString(lua_State *in_state, uint8_t **out_string)
    {
        if (out_string == nullptr)
            return static_cast<int32_t>(Status::InvalidArgument);

        if (in_state == nullptr)
            return static_cast<int32_t>(Status::InvalidArgument);

        const char *msg = lua_tostring(in_state, -1);
        if (msg != nullptr)
        {
            *out_string = (uint8_t *)msg;
            lua_pop(in_state, 1);
            return static_cast<int32_t>(Status::OK);
        }
        else if (luaL_callmeta(in_state, -1, "__tostring") && lua_type(in_state, -1) == LUA_TSTRING)
        {
            *out_string = (uint8_t *)lua_tostring(in_state, -1);
            lua_pop(in_state, 1);
            return static_cast<int32_t>(Status::OK);
        }
        else
        {
            *out_string = nullptr;
            return static_cast<int32_t>(Status::InvalidConversion);
        }
    }

    __declspec(dllexport) int32_t luaRun(lua_State *in_state, const uint8_t *in_data, int32_t in_data_size)
    {
        if (in_state == nullptr)
            return static_cast<int32_t>(Status::InvalidArgument);

        int status = luaL_loadbuffer(in_state, (const char *)in_data, in_data_size, "script");
        if (status != LUA_OK)
        {
            return static_cast<int32_t>(Status::InvalidData);
        }

        status = docall(in_state, 0, 0);
        if (status == LUA_OK)
        {
            return static_cast<int32_t>(Status::OK);
        }
        else
        {
            return static_cast<int32_t>(Status::Fault);
        }
    }
}