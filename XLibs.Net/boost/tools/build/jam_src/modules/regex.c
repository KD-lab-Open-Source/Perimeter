
#include "../native.h"
#include "../timestamp.h"
#include "../newstr.h"
#include "../strings.h"
#include "../regexp.h"
#include "../compile.h"

/*
rule transform ( list * : pattern )
{
    local result ;
    for local e in $(list)
    {
        local m = [ MATCH $(pattern) : $(e) ] ;
        if $(m)
        {
            result += $(m[1]) ;
        }        
    }
    return $(result) ;
}
*/
LIST *regex_transform( PARSE *parse, FRAME *frame )
{
    LIST* l = lol_get( frame->args, 0 );    
    LIST* pattern = lol_get( frame->args, 1 );    
    LIST* result = 0;

    string buf[1];
    string_new(buf);


    /* Result is cached and intentionally never freed */
    {
        regexp *re = regex_compile( pattern->string );

        for(; l; l = l->next)
        {
            if( regexec( re, l->string ) )
            {
                if (re->startp[1])
                {
                    string_append_range( buf, re->startp[1], re->endp[1] );
                    result = list_new( result, newstr( buf->value ) );                
                    string_truncate( buf, 0 );
                }
            }
        }
        string_free( buf );
    }
    
    return result;
}

void init_regex()
{
    {
        char* args[] = { "list", "*", ":", "pattern", 0 };
        declare_native_rule("regex", "transform", args, regex_transform);
    }
}
