#include <cstdint>

#include <lua.hpp>

typedef void *nint_t;

enum class Status
{
    Fault = -1,
    OK = 0,
    InvalidArgument = 1,
    InvalidData = 2,
};

extern "C"
{
    __declspec(dllexport) nint_t luaCreate(const uint8_t *in_data, int32_t in_data_size)
    {
        lua_State *L = luaL_newstate();

        return 0;
    }

    __declspec(dllexport) int32_t luaFree(const uint8_t *in_data, int32_t in_data_size)
    {
        return 0;
    }

    /*
     ** Prints an error message, adding the program name in front of it
     ** (if present)
     */
    static void l_message(const char *pname, const char *msg)
    {
        if (pname)
            lua_writestringerror("%s: ", pname);
        lua_writestringerror("%s\n", msg);
    }

    /*
     ** Check whether 'status' is not OK and, if so, prints the error
     ** message on the top of the stack.
     */
    static int report(lua_State *L, int status)
    {
        if (status != LUA_OK)
        {
            const char *msg = lua_tostring(L, -1);
            if (msg == NULL)
                msg = "(error message not a string)";
            l_message("Gossamer.Lua", msg);
            lua_pop(L, 1); /* remove message */
        }
        return status;
    }

    /*
     ** Message handler used to run all chunks
     */
    static int msghandler(lua_State *L)
    {
        const char *msg = lua_tostring(L, 1);
        if (msg == NULL)
        {                                            /* is error object not a string? */
            if (luaL_callmeta(L, 1, "__tostring") && /* does it have a metamethod */
                lua_type(L, -1) == LUA_TSTRING)      /* that produces a string? */
                return 1;                            /* that is the message */
            else
                msg = lua_pushfstring(L, "(error object is a %s value)",
                                      luaL_typename(L, 1));
        }
        luaL_traceback(L, L, msg, 1); /* append a standard traceback */
        return 1;                     /* return the traceback */
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

    __declspec(dllexport) int32_t luaRun(const uint8_t *in_data, int32_t in_data_size)
    {
        lua_State *L = luaL_newstate();
        if (L == nullptr)
            return static_cast<int32_t>(Status::Fault);

        luaL_openlibs(L);    
        luaL_checkversion(L);

        int status = luaL_loadbuffer(L, (const char *)in_data, in_data_size, "script");
        if (status != LUA_OK)
        {
            report(L, status);
            return static_cast<int32_t>(Status::InvalidData);
        }

        status = docall(L, 0, 0);
        if (status == LUA_OK)
        {
            //lua_pop(L, 1); 
        }
        else
        {
            //return static_cast<int32_t>(Status::Fault);
        }

        report(L, status);
        lua_close(L);
        return 0;
    }
}

/*
lua_State *L = luaL_newstate();
if (L == nullptr)
{
lua_close(L);
return static_cast<int32_t>(Status::Fault);
}

luaL_openlibs(L);  
luaL_checkversion(L);

int status = luaL_loadbuffer(L, (const char *)in_data, in_data_size, "script");
if (status != LUA_OK)
{
report(L, status);
lua_close(L);
return static_cast<int32_t>(Status::InvalidData);
}

status = docall(L, 0, 0);
if (status == LUA_OK)
{
// lua_pushboolean(L, 1);
// lua_pop(L, 1);
}
else
{
// lua_pushboolean(L, 0);
// return static_cast<int32_t>(Status::Fault);
}

report(L, status);

lua_close(L);
return 0;
*/