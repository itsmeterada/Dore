/**************************************************************************
# Copyright (C) 1994 Kubota Graphics Corp.
# 
# Permission to use, copy, modify, and distribute this material for
# any purpose and without fee is hereby granted, provided that the
# above copyright notice and this permission notice appear in all
# copies, and that the name of Kubota Graphics not be used in
# advertising or publicity pertaining to this material.  Kubota
# Graphics Corporation MAKES NO REPRESENTATIONS ABOUT THE ACCURACY
# OR SUITABILITY OF THIS MATERIAL FOR ANY PURPOSE.  IT IS PROVIDED
# "AS IS", WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE AND KUBOTA GRAPHICS CORPORATION DISCLAIMS ALL WARRANTIES,
# EXPRESS OR IMPLIED.
**************************************************************************/

/*
==============================================================================
Functions 
    dor_dictionary_create
    dor_dictionary_set_name_integer
    dor_dictionary_set_name_string
    dor_dictionary_set_name
    dor_dictionary_inq_name
    dor_dictionary_inq_handle
==============================================================================
*/

#include <string.h>
#include <dore/internal/dogen.h>


    /***************************/
    /***  Defined Constants  ***/
    /***************************/

#define HASH_MAX      137	/* Use a prime number for hash tables. */

#define DcNameHandle  (-1)	/* Object Handle -> Name */


    /*************************/
    /***  Data Structures  ***/
    /*************************/

typedef struct
{   DtNameType  type;		/* Type of Name */
    union {
	DtInt       integer;	/* Integer Name Value */
	char       *string;	/* String Name Value */
	dot_object *handle;	/* Object Handle Value */
    } u;
} dot_name;

	/* Symbol Type 1:  Free Symbol
	**     next, previous:  Neighboring symbols in free list.
	**     type:  DcNameNone
	**
	** Symbol Type 2:  Name to Object Handle
	**     next, previous:  Neighboring symbols in hash list.
	**     type:   DcNameInteger or DcNameString
	**     name:   Set to integer value or string pointer
	**     value.objhandle:  Object handle
	**
	** Symbol Type 3:  Object Handle to Name Symbol
	**     next, previous:  Neighboring symbols in hash list.
	**     type:   DcNameHandle
	**     name.handle:  Object handle
	**     value.namesym:  Pointer to name symbol-entry
	*/

typedef struct dict_symbol dot_dict_symbol;
struct dict_symbol {
    dot_name         name;	/* Symbol Name/Identifier */
    dot_dict_symbol *previous;	/* Prev Entry w/Same Hash Code */
    dot_dict_symbol *next;	/* Next Entry w/Same Hash or Next Free Symbol */
    union {
	dot_object      *objhandle;	/* (for DcNameInteger/String entries) */
	dot_dict_symbol *namesym;	/* (for DcNameHandle entries) */
    } value;
};

	/*  Dictionary blocks contain a bank of symbol entries and a pointer
	**  to the next allocated block.  At initial allocation, all symbols
	**  are linked into the free-symbol list.  */

typedef struct dict_block  dot_dict_block;
struct dict_block {
    dot_dict_symbol  symbols[HASH_MAX];	/* Block of Dictionary Symbols */
    dot_dict_block  *next;		/* Next Dictionary Block */
};

	/*  Each dictionary contains a preliminary hash table, a list of free
	**  symbol entries, and a list of allocated dictionary blocks.  */

typedef struct dictionary {
    dot_dict_block   hash_table;	/* Dictionary Name Hash Table */
    dot_dict_symbol *free_symbol;	/* Next Free Symbol Entry */
    dot_dict_block  *allocated_blocks;	/* List of Allocated Blocks */
} dot_dict;


	/*******************************/
	/***  Function Declarations  ***/
	/*******************************/

static void     dor_dictionary_set_name (dot_dict*,dot_object*,dot_name,DtFlag);
static dot_name dor_dictionary_inq_name      (dot_dict*, dot_object*);
static dot_dict_symbol *
	        dor_dictionary_find_symbol   (dot_dict*,dot_name,DtInt,DtFlag);
static void     dor_dictionary_delete_symbol (dot_dict*, dot_dict_symbol*);
static void     dor_dictionary_add_block     (dot_dict*);
static int      dor_dictionary_hash_handle   (dot_object*);
static int      dor_dictionary_hash_integer  (DtInt);
static int      dor_dictionary_hash_string   (char*);



#if 0
............................................................................... 
: How This Mess Works
: ~~~~~~~~~~~~~~~~~~~
:     The dictionary holds correspondences of names and object handles.  Each
: correspondence yields two separate entries in the dictionary:  one for name
: to object handle, and one for object handle to name (there are two separate
: entries to yield quick lookups).
: 
:     A dictionary consists of a hash table, a list of symbol blocks, and a
: list of free symbol entries.
: 
:     A symbol entry consists of a name, an associated value, and links to the
: previous and next symbol entries.
: 
:     Dictionaries start out with a preliminary hash table that is embedded in
: the dictionary structure.  Each entry in this table has a single symbol entry
: slot.  Additional symbol entries for a particular hash value are allocated
: from the free symbol list and are linked in as needed.  Free symbol entries
: are allocated a block at a time, and are pulled off as needed.
: 
:     There are three kinds of symbol entries.  The first one contains the
: object name as either an integer or a string, the name type is set to either
: DcNameInteger or DcNameString, and the value.objhandle field contains the
: associated object handle.  These entries are used for lookups given a name.
: 
:     The second kind of symbol entry is the reverse association.  The
: name type field is set to DcNameHandle, the name is set to the object handle,
: and the value.namesym field contains a pointer to the object-name symbol
: entry.
: 
:     The third kind of symbol entry is a free one, available for subsequent
: use.  It is linked into the free-symbol list (via the free_symbol field),
: and is marked by a name type field set to DcNameNone.
:............................................................................. 
#endif



/*****************************************************************************
**  This routine creates a new dictionary with an initial block of free hash
**  table entries.  Each symbol entry in the preliminary hash table is
**  initialized as a free symbol in a single-entry list.  Symbol entries will
**  later be added, one at a time, as they are needed.
*****************************************************************************/

dot_dict *dor_dictionary_create ()
{
    auto     dot_dict *dictionary;	/* Pointer to New Dictionary */
    register int       ii;		/* Hash Slot Index */

    /*  Allocate the dictionary entry. */

    if (!(dictionary = dor_space_allocate (sizeof(dot_dict))))
	return DcNullPtr;

    /*  Initialize the list of allocated blocks to point to the "built-in"
    **  block of hash-table symbols.  */

    dictionary->allocated_blocks = &(dictionary->hash_table);
    dictionary->hash_table.next  = DcNullPtr;

    /*  For each symbol in the initial hash table, set the entries to null.  */

    for (ii=0;  ii < HASH_MAX;  ++ii) {
	dictionary->hash_table.symbols[ii].name.type = DcNameNone;
	dictionary->hash_table.symbols[ii].previous  = DcNullPtr;
	dictionary->hash_table.symbols[ii].next      = DcNullPtr;
    }

    dictionary->free_symbol = DcNullPtr;

    /*  Allocate a block of free symbol-table entries to the dictionary.  */

    dor_dictionary_add_block (dictionary);

    return dictionary;
}



/*****************************************************************************
**  This function assigns the name of a given object to an integer value.
**  It first packs the name into a name structure and then calls the generic
**  set-name function.  If the user specifies an integer name of 0, then the
**  existing integer name is removed from the dictionary.
*****************************************************************************/

void dor_dictionary_set_name_integer (
    dot_dict   *dictionary,	/* Dictionary to Use */
    dot_object *object,		/* Object to Name */
    DtInt       name,		/* New Integer Identifier */
    DtFlag      replace)	/* Force Name Change If Duplicate */
{
    auto dot_name objname;

    if (!name)
    {   objname.type = DcNameNone;
    }
    else
    {   objname.type = DcNameInteger;
	objname.u.integer = name;
    }

    dor_dictionary_set_name (dictionary, object, objname, replace);
}



/*****************************************************************************
**  This function assigns the name of a given object to an integer value.
**  It first packs the name into a name structure and then calls the generic
**  set-name function.  If the user specifies a null pointer to the name, then
**  the existing name is removed from the dictionary.
*****************************************************************************/

void dor_dictionary_set_name_string (
    dot_dict   *dictionary,	/* Dictionary to Use */
    dot_object *object,		/* Object To Name */
    char       *name,		/* New String Identifier */
    DtFlag      replace)	/* Force Name Change If Duplicate */
{
    auto dot_name objname;

    if (!name)
    {   objname.type = DcNameNone;
    }
    else
    {   objname.type = DcNameString;
	objname.u.string = name;
    }

    dor_dictionary_set_name (dictionary, object, objname, replace);
}



/*****************************************************************************
**  This function returns the name type of the specified object.  Possible
**  return values are DcNameInteger and DcNameString.
*****************************************************************************/

DtNameType dor_dictionary_inq_name_type (
    dot_dict   *dictionary,	/* Dictionary to Search */
    dot_object *object)		/* Object Identifier */
{
    return (dor_dictionary_inq_name(dictionary,object)).type;
}



/*****************************************************************************
**  This function returns the integer name of the specified object.  If the
**  specified object does not have an integer name, the function returns zero.
*****************************************************************************/

DtInt dor_dictionary_inq_name_integer (
    dot_dict   *dictionary,	/* Dictionary to Search */
    dot_object *object)		/* Object Identifier */
{
    auto dot_name name;		/* Object Name */

    name = dor_dictionary_inq_name (dictionary, object);
    return (name.type == DcNameInteger) ? name.u.integer : 0;
}



/*****************************************************************************
**  This function returns a pointer to the string name given to the specified
**  object.  If the object does not have a string-valued name, the function
**  returns null.
*****************************************************************************/

char *dor_dictionary_inq_name_string (
    dot_dict   *dictionary,	/* Dictionary to Search */
    dot_object *object)		/* Object Identifier */
{
    auto dot_name name;		/* Object Name */

    name = dor_dictionary_inq_name (dictionary, object);
    return (name.type == DcNameString) ? name.u.string : DcNullPtr;
}



/*****************************************************************************
**  This function looks up the object associated with a given integer name.
**  If no such object exists, it returns null, otherwise it returns a pointer
**  to the associated object.
*****************************************************************************/

dot_object *dor_dictionary_inq_obj_integer (
    dot_dict *dictionary,	/* Dictionary to Search */
    DtInt     name,		/* Object Integer Name */
    DtInt     class_id)		/* Object Class Identifier */
{
    auto dot_name         objname;	/* Object Name Structure */
    auto dot_dict_symbol *symbol;	/* Name Symbol Entry */

    objname.type = DcNameInteger;
    objname.u.integer = name;

    symbol = dor_dictionary_find_symbol(dictionary, objname, class_id, DcFalse);

    return (symbol) ? (symbol->value.objhandle) : DcNullPtr;
}



/*****************************************************************************
**  This function returns a pointer to the object associated with the given
**  string name.  If no such object exists, the function returns null.
*****************************************************************************/

dot_object *dor_dictionary_inq_obj_string (
    dot_dict *dictionary,	/* Dictionary to Search */
    char     *name,		/* Object String Name */
    DtInt     class_id)		/* Object Class Identifier */
{
    auto dot_name         objname;	/* Object Name Structure */
    auto dot_dict_symbol *symbol;	/* Name Symbol Entry */

    objname.type = DcNameString;
    objname.u.string = name;

    symbol = dor_dictionary_find_symbol(dictionary, objname, class_id, DcFalse);

    return (symbol) ? (symbol->value.objhandle) : DcNullPtr;
}



/*****************************************************************************
**  This routine sets the name of the given object to the given value.  The
**  name type can be either DcNameInteger, DcNameString or DcNameNone.  If the
**  type is DcNameNone, the object's current name will be deleted from the
**  dictionary.
*****************************************************************************/

static void dor_dictionary_set_name (
    dot_dict   *dictionary,	/* Target Dictionary */
    dot_object *object,		/* Object to be Named */
    dot_name    name,		/* Name to Give Object */
    DtFlag      replace)	/* Replace-Name Flag */
{
    auto dot_dict_symbol *handle_symbol;	/* Handle-to-Name Symbol */
    auto dot_dict_symbol *name_symbol;		/* Name-to-Handle Symbol */
    auto DtInt            object_type;		/* Type of Passed Object */
    auto dot_name         handle_name;		/* Name Struct for Handle */

    handle_symbol = DcNullPtr;
    name_symbol   = DcNullPtr;
    object_type   = dor_object_inq_type (object);

    /*  If the name type is DcNameNone, then the request is to delete the
    **  object's name from the dictionary.  Find the name and remove it.  */

    if (name.type == DcNameNone) {
	handle_name.type     = DcNameHandle;
	handle_name.u.handle = object;

	handle_symbol = dor_dictionary_find_symbol
			    (dictionary, handle_name, 0, DcFalse);
	name_symbol   = handle_symbol->value.namesym;

	if (handle_symbol) {
	    dor_dictionary_delete_symbol (dictionary, name_symbol);
	    dor_dictionary_delete_symbol (dictionary, handle_symbol);
	}
	return;
    }

    /*  Look for a name-to-handle symbol-entry with the given name.  If one
    **  does not exist, then create it.  */

    name_symbol = dor_dictionary_find_symbol
		      (dictionary, name, object_type, DcTrue);

    /*  If the value field is null, then the name_symbol is newly created,
    **  otherwise, it's a previous name-to-object entry.  This entry may
    **  already point to the given object, or it may point to some other
    **  object.  */

    if (name_symbol->value.objhandle) {

	/*  If the name already points to the correct object, then return. */

	if (name_symbol->value.objhandle == object)
	    return;

	/*  The name points to some other object.  If the replace flag is
	**  set to false, then signal the collision and return.  */

	if (!replace) {
	    if (name.type == DcNameString) {
		sprintf (dor_error_get_line(), "\"%s\"", name.u.string);
	    } else {
		sprintf (dor_error_get_line(), "%d", name.u.integer);
	    }
	    DDerror (ERR_OBJ_NAME_IN_USE, "DsSetObjName", dor_error_get_line());
	    return;
	}

	/*  If the replace flag is set to true, then delete the association
	**  of the previous object with the given name.  */

	handle_name.type     = DcNameHandle;
	handle_name.u.handle = name_symbol->value.objhandle;

	handle_symbol = dor_dictionary_find_symbol
			    (dictionary, handle_name, 0, DcFalse);

	if (handle_symbol)
	    dor_dictionary_delete_symbol (dictionary, handle_symbol);
    }

    /*  Look for entries that match the given object to any other name.  If
    **  no such entry exists, allocate one.  Either way, handle_symbol will
    **  point to a symbol entry after this call.  If the value field is null,
    **  then it's a new symbol, otherwise it's a pre-existing one, so we
    **  should delete the old name symbol-entry.  */

    handle_name.type     = DcNameHandle;
    handle_name.u.handle = object;

    handle_symbol = dor_dictionary_find_symbol
			(dictionary, handle_name, 0, DcTrue);

    if (handle_symbol->value.namesym) {
	dor_dictionary_delete_symbol (dictionary, handle_symbol->value.namesym);
    }

    /*  At this point, we've removed all references to the given object and to
    **  the given name, and we have the two complementary entries needed for
    **  the name/object association.  Set the object-handle symbol value to
    **  the address of the matching name symbol, and set the name-symbol value
    **  to the associated object handle.  */

    handle_symbol->value.namesym = name_symbol;
    name_symbol->value.objhandle = object;
}



/*****************************************************************************
**  This function finds the name associated with a given object handle.  It
**  returns the name type to the 'name_type' parameter, and returns the name
**  to the 'name' parameter.
*****************************************************************************/

static dot_name dor_dictionary_inq_name (
    dot_dict   *dictionary,	/* Dictionary to Search */
    dot_object *object)		/* Object Handle to Search For */
{
    register dot_dict_symbol *handle_symbol;  /* Handle-to-Name Symbol Entry */
    auto     dot_name         name;	      /* Object Name */

    /*  Find the name-symbol associated with the given object handle.  */

    name.type = DcNameHandle;
    name.u.handle = object;

    handle_symbol = dor_dictionary_find_symbol (dictionary, name, 0, DcFalse);

    /*  If no symbol was found for the given object handle, then set the
    **  name type to none and return.  If the handle symbol was found, then
    **  grab the pointer to the name symbol entry from the 'value' field,
    **  and get the name information.  */

    if (!handle_symbol){
	name.type = DcNameNone;
    } else {
	name = handle_symbol->value.namesym->name;
    }

    return name;
}



/*****************************************************************************
**  This procedure finds the symbol table entry given either an object handle
**  (finds the object-to-name symbol) or a name (finds the name-to-object
**  symbol).  If no such entry exists, this function returns null if the 'add'
**  parameter is false, or creates a new entry if the 'add' parameter is true.
**  If a new entry is created, this is signalled by the fact that the
**  appropriate value field is set to null (if name_type == DcNameHandle, then
**  value.namesym is set to null, otherwise value.objhandle is set to null).
*****************************************************************************/

static dot_dict_symbol *dor_dictionary_find_symbol (
    dot_dict   *dictionary,	/* Dictionary for the Given Symbol */
    dot_name    name,		/* Object Name */
    DtInt       object_type,	/* Type of Object */
    DtFlag      add)		/* Add-Entry Flag */
{
    register int              hashcode;		/* Symbol Name Hash Code */
    register dot_dict_symbol *start;		/* Start of Hash Slot List */
    register dot_dict_symbol *entry;		/* Current Hash Slot Node */
    register dot_dict_symbol *previous;		/* Previous Hash Slot Node */
    auto     DtFlag           compare_types;	/* Compare Object Types */

    /*  Get the hash table index according to the name or object handle.  */

    if (name.type == DcNameHandle)
	hashcode = dor_dictionary_hash_handle (name.u.handle);
    else if (name.type == DcNameString)
	hashcode = dor_dictionary_hash_string (name.u.string);
    else
	hashcode = dor_dictionary_hash_integer (name.u.integer);

    /*  Start at the beginning of the hash slot list and traverse all entries
    **  in the list (these all have the same hash code).  */

    start = dictionary->hash_table.symbols + hashcode;
    previous = DcNullPtr;

    if ((name.type != DcNameInteger) && (name.type != DcNameString))
	compare_types = DcFalse;
    else
	compare_types = (object_type != DcTypeAny);

    for (entry=start;  entry;  entry=entry->next) {

	previous = entry;

	/*  Skip to the next entry if the name types don't match.  */

	if (name.type != entry->name.type) continue;

	/*  Compare the object types if necessary.  */

	if (  (compare_types)
	   && (object_type != dor_object_inq_type (entry->value.objhandle))
	   )
	{
	    continue;
	}

	/*  Compare the name.  If the name matches, then return the pointer
	**  to that symbol.  */

	switch (name.type) {
	    case DcNameString:
		if (0 == strcmp (name.u.string, entry->name.u.string))
		    return entry;
		break;

	    case DcNameInteger:
		if (name.u.integer == entry->name.u.integer)
		    return entry;
		break;

	    case DcNameHandle:
		if (name.u.handle == entry->name.u.handle)
		    return entry;
		break;

	    default: break;
	}
    }

    /*  At this point, the specified name was not found.  If the add flag is
    **  false, then the name is not to be added, so return a null pointer.  */

    if (!add) return DcNullPtr;

    /*  We need to create a new entry.  If the primary hash table entry is
    **  free, then use that one, otherwise grab one from the free-symbol list
    **  and link it in.  */

    if (start->name.type == DcNameNone)
	entry = start;
    else {
	/*  If there are no free symbols available, then allocate another block
	**  of symbol entries.  */

	if (!(dictionary->free_symbol))
	    dor_dictionary_add_block (dictionary);

	if (!(dictionary->free_symbol))
	    return DcNullPtr;

	/*  Grab the next free symbol, yank it from the head of the free-symbol
	**  list, and add it to the tail of the symbol list for the given hash
	**  slot.  */

	entry                   = dictionary->free_symbol;
	dictionary->free_symbol = dictionary->free_symbol->next;
	entry->next             = DcNullPtr;
	entry->previous         = previous;
	previous->next          = entry;
    }

    /*  Add the entry to the dictionary.  For each entry type, set the name
    **  accordingly and set the appropriate value field to null.  */

    entry->name.type = name.type;

    switch (name.type) {

	case DcNameInteger:
	    entry->name.u.integer  = name.u.integer;
	    entry->value.objhandle = DcNullPtr;
	    break;

	case DcNameString:
	    entry->name.u.string = dor_space_allocate (1+strlen(name.u.string));
	    if (!(entry->name.u.string)) return DcNullPtr;
	    strcpy (entry->name.u.string, name.u.string);
	    entry->value.objhandle = DcNullPtr;
	    break;

	case DcNameHandle:
	    entry->name.u.handle = name.u.handle;
	    entry->value.namesym = DcNullPtr;
	    break;

	default: break;
    }

    return entry;
}



/*****************************************************************************
**  This routine deletes a symbol entry from the dictionary.  If the symbol is
**  not a primary symbol entry in the hash table, then it is added to the
**  dictionary's symbol free list.
*****************************************************************************/

static void dor_dictionary_delete_symbol (
    dot_dict        *dictionary,	/* Target Dictionary */
    dot_dict_symbol *symbol)		/* Symbol Entry to Delete */
{
    dot_dict_symbol *next;	/* Next Symbol Table Entry */
    dot_dict_symbol *previous;	/* Previous Symbol Table Entry */

    /*  If the symbol name is a string, then free the string memory.  */

    if (symbol->name.type == DcNameString)
	dor_space_deallocate (symbol->name.u.string);

    /*  If the symbol entry is the primary entry in the hash table, then mark
    **  it as available and return.  */

    if (!(symbol->previous)) {
	symbol->name.type = DcNameNone;
	return;
    }

    /*  The symbol is a node somewhere after the first node in the hash entry
    **  list.  Unlink the symbol from the symbol list and add it to the free
    **  list.  */

    previous = symbol->previous;
    next     = symbol->next;

    previous->next = next;
    if (next) next->previous = previous;

    /*  Link the unused symbol entry into the dictionary's free list.  */

    symbol->next = dictionary->free_symbol;
    dictionary->free_symbol = symbol;
}



/*****************************************************************************
**  This routine adds a block of symbol-table entries to the specified
**  dictionary.  Symbol entry "next" pointers are set to the next entry in the
**  block of symbols, and the last symbol entry "next" pointer is set to null.
*****************************************************************************/

static void dor_dictionary_add_block (dot_dict *dictionary)
{
    register dot_dict_block *new_block;		/* Newly Allocated Block */
    register int             ii;		/* Symbol Entry Index */

    /* Allocate a new block of symbol entries. */

    if (!(new_block = dor_space_allocate (sizeof(dot_dict_block))))
	return;

    /* Set the 'next' pointer of each symbol entry to point to the next entry
    ** in the symbol table.  The last symbol entry is set to null.  Also, the
    ** name types are set to DcNameNone to indicate that they're not in use. */

    for (ii=0;  ii < HASH_MAX;  ++ii) {
	new_block->symbols[ii].name.type = DcNameNone;
	new_block->symbols[ii].next      = new_block->symbols + ii + 1;
    }

    new_block->symbols[HASH_MAX-1].next = DcNullPtr;

    /*  Add the new symbol block to the list of allocated blocks for the
    **  specified dictionary.  */

    new_block->next = dictionary->allocated_blocks;
    dictionary->allocated_blocks = new_block;
    dictionary->free_symbol = new_block->symbols;
}



/*****************************************************************************
**  This function operates with essentially the same method as the function
**  dor_dictionary_hash_integer, except that the pointer type is first cast
**  to a long value.  Note that pointers may always be word, longword or
**  quadword aligned, so it's important that the HASH_MAX value not be a power
**  of two in order to get a good distribution of indices across the hash
**  table.  In fact, HASH_MAX (and all hash-table sizes, actually) should be
**  a prime number.  Note also that in the worst case, this function will
**  always return the same hash code and all entries will collapse to a single
**  list.  This is failure in terms of speed, but the code should still
**  function.
*****************************************************************************/

static int dor_dictionary_hash_handle (dot_object *handle)
{
    return (long)(handle) % HASH_MAX;
}



/*****************************************************************************
**  This function returns a hashed index value from an integer identifier, to
**  be used in the object-name lookup table.
*****************************************************************************/

static int dor_dictionary_hash_integer (DtInt value)
{
    return value % HASH_MAX;
}



/*****************************************************************************
**  This function returns a hashed index value from a character string, to be
**  used in the object-naming lookup table.
*****************************************************************************/

static int dor_dictionary_hash_string (char *name)
{
    register int ii;		/* Loop Counter */
    register int length;	/* Length of the Name String */
    register int hcode;		/* Hash Code */

    length = strlen (name);
    hcode  = 0;

    for (ii=0;  ii < (length+1)/2;  ++ii) {
	hcode = (hcode * 27) + (name[ii] - 0x20);
	hcode = (hcode * 27) + (name[length - 1 - ii] - 0x20);
    }

    hcode = abs (hcode);

    return hcode % HASH_MAX;
}
