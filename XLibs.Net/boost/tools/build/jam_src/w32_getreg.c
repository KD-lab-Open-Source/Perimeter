# include "jam.h"
# include "lists.h"
# include "newstr.h"
# include "parse.h"
# include "frames.h"
# include "strings.h"

# define WIN32_LEAN_AND_MEAN
# include <windows.h>

# define  MAX_REGISTRY_DATA_LENGTH  4096

typedef struct
{
    LPCSTR  name;
    HKEY    value;
} KeyMap;

static const KeyMap dlRootKeys[] = {
    { "HKLM", HKEY_LOCAL_MACHINE },
    { "HKCU", HKEY_CURRENT_USER },
    { "HKCR", HKEY_CLASSES_ROOT },
    { "HKEY_LOCAL_MACHINE", HKEY_LOCAL_MACHINE },
    { "HKEY_CURRENT_USER", HKEY_CURRENT_USER },
    { "HKEY_CLASSES_ROOT", HKEY_CLASSES_ROOT },
    { 0, 0 }
};

LIST*
builtin_system_registry(
    PARSE    *parse,
    FRAME    *frame )
{
    char const* path = lol_get(frame->args, 0)->string;
    LIST* result = L0;
    HKEY key;
    
    {
        const KeyMap *p;
        
        for (p = dlRootKeys; p->name; ++p)
        {
            int n = strlen(p->name);
            if (!strncmp(path,p->name,n))
            {
                if (path[n] == '\\' || path[n] == 0)
                {
                    path += n + 1;
                    break;
                }
            }
        }
        
        key = p->value;
    }

    if (
        key != 0
        && ERROR_SUCCESS == RegOpenKeyEx(key, path, 0, KEY_QUERY_VALUE, &key) 
    )
    {
        DWORD  type, len;
        BYTE   data[MAX_REGISTRY_DATA_LENGTH];
        LIST const* const field = lol_get(frame->args, 1);
        
        if ( ERROR_SUCCESS ==
             RegQueryValueEx(key, field ? field->string : 0, 0, &type, data, &len) )
        {
            switch (type)
            {
                
             case REG_EXPAND_SZ:
                 {
                     long len;
                     string expanded[1];
                     string_new(expanded);

                     while (
                         (len = ExpandEnvironmentStrings(
                             (LPCSTR)data, expanded->value, expanded->capacity))
                         > expanded->capacity
                     )
                         string_reserve(expanded, len);

                     expanded->size = len - 1;

                     result = list_new( result, newstr(expanded->value) );
                     string_free( expanded );
                 }
                 break;
            
             case REG_MULTI_SZ:
                 {
                     char* s;

                     for (s = (char*)data; *s; s += strlen(s) + 1)
                         result = list_new( result, newstr(s) );

                 }
                 break;
             
             case REG_DWORD:
                 {
                     char buf[100];
                     sprintf( buf, "%u", *(PDWORD)data );
                     result = list_new( result, newstr(buf) );
                 }
                 break;

             case REG_SZ:
                 result = list_new( result, newstr((char*)data) );
                 break;
            }
        }
        RegCloseKey(key);
    }
    return  result;
}
