/****************************************************/
/* File: symtab.h                                   */
/* Symbol table interface for the TINY compiler     */
/* (allows only one symbol table)                   */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/
#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include "globals.h"

/* SIZE is the size of the hash table */
#define SIZE 211

/* the list of line numbers of the source 
 * code in which a variable is referenced
 */
typedef struct LineListRec
{ 
    int lineno;
    struct LineListRec * next;
} * LineList;

typedef struct BucketListRec
{
    char * name;
    TreeNode * node;
    LineList lines;
    int memloc; /* memory location for variable */
    struct BucketListRec * next;
} * BucketList;

typedef struct ScopeListRec
{ 
    char * name;
    BucketList bucket[SIZE];
    int depth;
    int memloc;
    struct ScopeListRec * parent;
} * ScopeList;

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
void st_insert( char * name, int lineno, int loc, TreeNode * node );

/* Function st_lookup returns the memory 
 * location of a variable or -1 if not found
 */
int st_lookup ( char * name );
int st_lookup_scope_now(char * name);
BucketList bl_lookup (char * name);

void scope_push(ScopeList scope);
void scope_pop();
ScopeList scope_now();
void line_add(char * name, int lineno);
int loc_add();

ScopeList create_scope(char * name);
/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
void printSymTab(FILE * listing);
void printFuncTab(FILE * listing);
void printGlobSym(FILE * listing);
void printScope(FILE * listing);

#endif
