/*
 * Copyright 1993, 1995 Christopher Seiwald.
 *
 * This file is part of Jam - see jam.c for Copyright information.
 */

# include "jam.h"
# include "hash.h"
# include <assert.h>

/* 
 * hash.c - simple in-memory hashing routines 
 *
 * External routines:
 *
 *     hashinit() - initialize a hash table, returning a handle
 *     hashitem() - find a record in the table, and optionally enter a new one
 *     hashdone() - free a hash table, given its handle
 *
 * Internal routines:
 *
 *     hashrehash() - resize and rebuild hp->tab, the hash table
 *
 * 4/29/93 - ensure ITEM's are aligned
 */

char 	*hashsccssid="@(#)hash.c	1.14  ()  6/20/88";

/* Header attached to all data items entered into a hash table. */

struct hashhdr {
	struct item *next;
	unsigned int keyval;			/* for quick comparisons */
} ;

/* This structure overlays the one handed to hashenter(). */
/* It's actual size is given to hashinit(). */

struct hashdata {
	char	*key;
	/* rest of user data */
} ;

typedef struct item {
	struct hashhdr hdr;
	struct hashdata data;
} ITEM ;

# define MAX_LISTS 32

struct hash 
{
	/*
	 * the hash table, just an array of item pointers
	 */
	struct {
		int nel;
		ITEM **base;
	} tab;

	int bloat;	/* tab.nel / items.nel */
	int inel; 	/* initial number of elements */

	/*
	 * the array of records, maintained by these routines
	 * essentially a microallocator
	 */ 
	struct {
		int more;	/* how many more ITEMs fit in lists[ list ] */
        ITEM *free; /* free list of items */
		char *next;	/* where to put more ITEMs in lists[ list ] */
		int datalen;	/* length of records in this hash table */
		int size;	/* sizeof( ITEM ) + aligned datalen */
		int nel;	/* total ITEMs held by all lists[] */
		int list;	/* index into lists[] */

		struct {
			int nel;	/* total ITEMs held by this list */
			char *base;	/* base of ITEMs array */
		} lists[ MAX_LISTS ];
	} items;

	char *name;	/* just for hashstats() */
} ;

static void hashrehash( struct hash *hp );
static void hashstat( struct hash *hp );

/*
 * hash_free() - remove the given item from the table if it's there.
 * Returns 1 if found, 0 otherwise.
 *
 * NOTE: 2nd argument is HASHDATA*, not HASHDATA** as elsewhere.
 */
int
hash_free(
	register struct hash *hp,
	HASHDATA *data)
{
	ITEM **prev;
	register ITEM **i;
	unsigned char *b = (unsigned char*)data->key;
	unsigned int keyval;

	keyval = *b;

	while( *b )
		keyval = keyval * 2147059363 + *b++;

    prev = hp->tab.base + ( keyval % hp->tab.nel );
	while(*prev )
    {
        register ITEM* i = *prev;
	    if( keyval == i->hdr.keyval && 
            !strcmp( i->data.key, data->key ) )
        {
            /* unlink the record from the hash chain */
            *prev = i->hdr.next;
            /* link it into the freelist */
            i->hdr.next = hp->items.free;
            hp->items.free = i;
            /* mark it free so we skip it during enumeration */
            i->data.key = 0;
            /* we have another item */
            hp->items.more++;
            return 1;
        }
        prev = &i->hdr.next;
    }
    return 0;
}

/*
 * hashitem() - find a record in the table, and optionally enter a new one
 */

int
hashitem(
	register struct hash *hp,
	HASHDATA **data,
	int enter )
{
	ITEM **base;
	register ITEM *i;
	unsigned char *b = (unsigned char*)(*data)->key;
	unsigned int keyval;

	if( enter && !hp->items.more )
	    hashrehash( hp );

	if( !enter && !hp->items.nel )
	    return 0;

	keyval = *b;

	while( *b )
		keyval = keyval * 2147059363 + *b++;

	base = hp->tab.base + ( keyval % hp->tab.nel );

	for( i = *base; i; i = i->hdr.next )
	    if( keyval == i->hdr.keyval && 
		!strcmp( i->data.key, (*data)->key ) )
	{
		*data = &i->data;
		return !0;
	}

	if( enter ) 
	{
        /* try to grab one from the free list */
        if ( hp->items.free )
        {
            i = hp->items.free;
            hp->items.free = i->hdr.next;
            assert( i->data.key == 0 );
        }
        else
        {
            i = (ITEM *)hp->items.next;
            hp->items.next += hp->items.size;
        }
		hp->items.more--;
		memcpy( (char *)&i->data, (char *)*data, hp->items.datalen );
		i->hdr.keyval = keyval;
		i->hdr.next = *base;
		*base = i;
		*data = &i->data;
	}

	return 0;
}

/*
 * hashrehash() - resize and rebuild hp->tab, the hash table
 */

static void hashrehash( register struct hash *hp )
{
	int i = ++hp->items.list;

	hp->items.more = i ? 2 * hp->items.nel : hp->inel;
	hp->items.next = (char *)malloc( hp->items.more * hp->items.size );
    hp->items.free = 0;
    
	hp->items.lists[i].nel = hp->items.more;
	hp->items.lists[i].base = hp->items.next;
	hp->items.nel += hp->items.more;

	if( hp->tab.base )
		free( (char *)hp->tab.base );

	hp->tab.nel = hp->items.nel * hp->bloat;
	hp->tab.base = (ITEM **)malloc( hp->tab.nel * sizeof(ITEM **) );

	memset( (char *)hp->tab.base, '\0', hp->tab.nel * sizeof( ITEM * ) );

	for( i = 0; i < hp->items.list; i++ )
	{
		int nel = hp->items.lists[i].nel;
		char *next = hp->items.lists[i].base;

		for( ; nel--; next += hp->items.size )
		{
			register ITEM *i = (ITEM *)next;
			ITEM **ip = hp->tab.base + i->hdr.keyval % hp->tab.nel;
            /* code currently assumes rehashing only when there are no free items */
            assert( i->data.key != 0 ); 
            
			i->hdr.next = *ip;
			*ip = i;
		}
	}
}

void hashenumerate( struct hash *hp, void (*f)(void*,void*), void* data )
{
    int i;
    for( i = 0; i <= hp->items.list; i++ )
    {
        char *next = hp->items.lists[i].base;
        int nel = hp->items.lists[i].nel;
        if ( i == hp->items.list )
            nel -= hp->items.more;

        for( ; nel--; next += hp->items.size )
        {
            register ITEM *i = (ITEM *)next;
            
            if ( i->data.key != 0 ) /* don't enumerate freed items */
                f(&i->data, data);
        }
    }
}

/* --- */

# define ALIGNED(x) ( ( x + sizeof( ITEM ) - 1 ) & ~( sizeof( ITEM ) - 1 ) )

/*
 * hashinit() - initialize a hash table, returning a handle
 */

struct hash *
hashinit( 
	int datalen,
	char *name )
{
	struct hash *hp = (struct hash *)malloc( sizeof( *hp ) );

	hp->bloat = 3;
	hp->tab.nel = 0;
	hp->tab.base = (ITEM **)0;
	hp->items.more = 0;
    hp->items.free = 0;
	hp->items.datalen = datalen;
	hp->items.size = sizeof( struct hashhdr ) + ALIGNED( datalen );
	hp->items.list = -1;
	hp->items.nel = 0;
	hp->inel = 11;
	hp->name = name;

	return hp;
}

/*
 * hashdone() - free a hash table, given its handle
 */

void
hashdone( struct hash *hp )
{
	int i;

	if( !hp )
	    return;

	if( DEBUG_MEM )
	    hashstat( hp );

	if( hp->tab.base )
		free( (char *)hp->tab.base );
	for( i = 0; i <= hp->items.list; i++ )
		free( hp->items.lists[i].base );
	free( (char *)hp );
}

/* ---- */

static void
hashstat( struct hash *hp )
{
	ITEM **tab = hp->tab.base;
	int nel = hp->tab.nel;
	int count = 0;
	int sets = 0;
	int run = ( tab[ nel - 1 ] != (ITEM *)0 );
	int i, here;

	for( i = nel; i > 0; i-- )
	{
		if( here = ( *tab++ != (ITEM *)0 ) )
			count++;
		if( here && !run )
			sets++;
		run = here;
	}

	printf( "%s table: %d+%d+%d (%dK+%dK) items+table+hash, %f density\n",
		hp->name, 
		count, 
		hp->items.nel,
		hp->tab.nel,
		hp->items.nel * hp->items.size / 1024,
		hp->tab.nel * sizeof( ITEM ** ) / 1024,
		(float)count / (float)sets );
}
