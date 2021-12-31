/****************************************************/
/* File: symtab.c                                   */
/* Symbol table implementation for the TINY compiler*/
/* (allows only one symbol table)                   */
/* Symbol table is implemented as a chained         */
/* hash table                                       */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"
#include "globals.h"

/* SHIFT is the power of two used as multiplier
   in hash function  */
#define SHIFT 4

/* the hash function */
static int hash ( char * key )
{ int temp = 0;
  int i = 0;
  while (key[i] != '\0')
  { temp = ((temp << SHIFT) + key[i]) % SIZE;
    ++i;
  }
  return temp;
}

/* The record in the bucket lists for
 * each variable, including name, 
 * assigned memory location, and
 * the list of line numbers in which
 * it appears in the source code
 */

/* the hash table */
static BucketList hashTable[SIZE];
static ScopeList scopeTable[SIZE];
static ScopeList scopeStack[SIZE];

int scopeTableIndex = 0;
int scopeStackIndex = 0;
int locArray[SIZE];

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
void st_insert( char * name, int lineno, int loc, TreeNode * node )
{ int h = hash(name);
  ScopeList scopeNow = scope_now();
  BucketList l =  scopeNow->bucket[h];
  while ((l != NULL) && (strcmp(name,l->name) != 0))
    l = l->next;
  if (l == NULL) /* variable not yet in table */
  { l = (BucketList) malloc(sizeof(struct BucketListRec));
    l->name = name;
    l->node = node;
    l->lines = (LineList) malloc(sizeof(struct LineListRec));
    l->lines->lineno = lineno;
    l->memloc = loc;
    l->lines->next= NULL;
    l->next = scopeNow->bucket[h];
    scopeNow->bucket[h] = l; }
} /* st_insert */

/* Function st_lookup returns the memory 
 * location of a variable or -1 if not found
 */
int st_lookup ( char * name )
{ 
  int h = hash(name);
  ScopeList scopeNow = scope_now();
  while (scopeNow != NULL)
  { 
    BucketList l =  scopeNow->bucket[h];
    while ((l != NULL) && (strcmp(name,l->name) != 0))
      l = l->next;
    if (l != NULL)
      return l->memloc;
    scopeNow = scopeNow->parent;
  }
  return -1;
}

int st_lookup_scope_now(char * name) 
{
  int h = hash(name);
  ScopeList scopeNow = scope_now();
  BucketList l =  scopeNow->bucket[h];
  while ((l != NULL) && (strcmp(name,l->name) != 0))
    l = l->next;
  if (l != NULL)
      return l->memloc;
  return -1;
}

int loc_add() {
  return locArray[scopeStackIndex-1]++;
}

BucketList bk_lookup(char * name)
{
  int h = hash(name);
  ScopeList scopeNow = scope_now();
  while(scopeNow != NULL)
  {
    BucketList l =  scopeNow->bucket[h];
    while ((l != NULL) && (strcmp(name,l->name) != 0))
      l = l->next;
    if (l != NULL)
      return l;
    scopeNow = scopeNow->parent;
  }
  return NULL;
}

void scope_push(ScopeList sc)
{
  scopeStack[scopeStackIndex] = sc;
  locArray[scopeStackIndex++] = 0;
}
void scope_pop()
{
  if(scopeStackIndex > 0) {
    scopeStackIndex--;
  }
}

ScopeList scope_now()
{
  if(scopeStackIndex > 0) {
    return scopeStack[scopeStackIndex-1];
  }
  else return NULL;
}

void line_add(char * name, int lineno) 
{
  int h = hash(name);
  ScopeList scopeNow = scope_now();
  while (scopeNow != NULL) 
  {
    BucketList l =  scopeNow->bucket[h];
    while ((l != NULL) && (strcmp(name,l->name) != 0))
      l = l->next;
    if (l != NULL) 
    {
      LineList linelist = l->lines;
      while( linelist-> next != NULL)
        linelist = linelist->next;
      linelist->next = (LineList) malloc(sizeof(struct LineListRec));
      linelist->next->lineno = lineno;
      linelist->next->next = NULL;
    }
    scopeNow = scopeNow->parent;
  }
}

ScopeList create_scope(char * name)
{
  ScopeList sc;
  sc = (ScopeList) malloc(sizeof(struct ScopeListRec));
  sc->name = name;
  sc->depth = scopeStackIndex;
  sc->parent = scope_now();
  scopeTable[scopeTableIndex++] = sc;
  return sc;
}

/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
void printSymTab(FILE * listing)
{ 
  int scope_idx, bk_idx;
  fprintf(listing, "< Symbol Table >\n");
  fprintf(listing," Symbol Name   Symbol Kind   Symbol Type    Scope Name   Location  Line Numbers\n");
  fprintf(listing,"-------------  -----------  -------------  ------------  --------  ------------\n");
  for(scope_idx=0; scope_idx<SIZE; scope_idx++) {
    ScopeList scopeNow = scope_now();
    BucketList * l = scopeNow->bucket;
    for(bk_idx=0; bk_idx<SIZE; bk_idx++) {
      if(l[bk_idx] != NULL) {
        BucketList bkNow = l[bk_idx];
        while(bkNow != NULL)
        {
          fprintf(listing, "%-15s", bkNow->name);
          TreeNode * node = bkNow->node;
          if(node->nodekind == DclK) {
            switch (node->kind.dcl)
            {
            case VarK:
              switch (node->type)
              {
              case Integer:
                fprintf(listing, "%-13s%-15s", "Variable", "int");
                break;
              case IntArray:
                fprintf(listing, "%-13s%-15s", "Variable", "int[]");
                break;
              default:
                break;
              }
              break;
            case FuncK:
              switch (node->type)
              {
              case Integer:
                fprintf(listing, "%-13s%-15s", "Function", "int");
                break;
              case Void:
                fprintf(listing, "%-13s%-15s", "Function", "void");
                break;
              default:
                break;
              }
              break;
            }
          }
          fprintf(listing, "%-14s", scopeNow->name);
          fprintf(listing, "%-10d", bkNow->memloc);
          LineList ll = bkNow->lines;
          while(ll != NULL) {
            fprintf(listing, "%4d", ll->lineno);
            ll = ll->next;
          }
          fprintf(listing, "\n");
          bkNow = bkNow->next;
        }
      }
    }
  }
} /* printSymTab */

void printFuncTab(FILE * listing) {
  int scope_idx, bk_idx, scope_param_idx, bk_param_idx;
  fprintf(listing, "< Functions >\n");
  fprintf(listing, "Function Name   Return Type   Parameter Name  Parameter Type");
  fprintf(listing, "-------------  -------------  --------------  --------------");
  for(scope_idx=0; scope_idx<SIZE; scope_idx++) {
    ScopeList scopeNow = scope_now();
    BucketList * l = scopeNow->bucket;
    for(bk_idx=0; bk_idx<SIZE; bk_idx++) {
      if(l[bk_idx] != NULL) {
        BucketList bkNow = l[bk_idx];
        TreeNode * node = bkNow->node;
        if(node->nodekind == DclK) {
          if(node->kind.dcl == FuncK) {
            fprintf(listing, "%-15s", bkNow->name);
            switch(node->type) {
              case Integer:
                fprintf(listing, "%-15s", "int");
                break;
              case Void:
                fprintf(listing, "%-15s", "void");
                break;
              default:
                break;
            }
          }
        }
        int param_num = 1;
        for(scope_param_idx=0; scope_param_idx<scopeStackIndex; scope_param_idx++) {
          ScopeList paramScope = scopeTable[scope_param_idx];
          if(strcmp(paramScope->name, bkNow->name) != 0)
            continue;
          BucketList * l_param = paramScope->bucket;
          for(bk_param_idx=0; bk_param_idx<SIZE; bk_param_idx++) {
            if(l_param[bk_param_idx] != NULL) {
              BucketList bk_param = l_param[bk_param_idx];
              TreeNode * node = bk_param->node;
              while(bk_param != NULL)
              {
                switch(node->nodekind) {
                  case ParamK:
                    param_num = 0;
                    fprintf(listing, "%-15s", bk_param->name);
                    switch(node->type) {
                      case Integer:
                        fprintf(listing, "%-15s", "int");
                        break;
                      case IntArray:
                        fprintf(listing, "%-15s", "int[]");
                        break;
                      default:
                        break;
                    }
                    break;
                  default:
                    break;
                }
                bk_param = bk_param->next;
              }
            }
          }
        }
        /*
        if(param_num) {
          if(strcmp(bkNow->name, "output") != 0)
            fprintf(listing, "%-15s", "output");
        }
        */
        fprintf(listing, "\n");
        bkNow = bkNow->next;
      }
    }
  }
}

void printGlobSym(FILE * listing) {
  int scope_idx, bk_idx;
  fprintf(listing, "< Global Symbols >\n");
  fprintf(listing, " Symbol Name   Symbol Kind   Symbol Type\n");
  fprintf(listing, "-------------  -----------  -------------\n");
  
  for(scope_idx=0; scope_idx<scopeStackIndex; scope_idx++) {
    ScopeList scopeNow = scopeTable[scope_idx];
    if(strcmp(scopeNow->name, "global") != 0)
      continue;
    BucketList * l = scopeNow->bucket;
    for(bk_idx=0; bk_idx<SIZE; bk_idx++) {
      if(l[bk_idx] != NULL) {
        BucketList bkNow = l[bk_idx];
        while(bkNow != NULL) 
        {
          TreeNode * node = bkNow->node;
          if(node->nodekind == DclK) {
            fprintf(listing, "%-15s", bkNow->name);
            switch (node->kind.dcl)
            {
            case VarK:
              switch (node->type)
              {
              case Integer:
                fprintf(listing, "%-13s%-13s", "Variable", "int");
                break;
              case IntArray:
                fprintf(listing, "%-13s%-13s", "Variable", "int[]");
                break;
              default:
                break;
              }
              break;
            case FuncK:
              switch (node->type)
                {
                case Integer:
                  fprintf(listing, "%-13s%-13s", "Function", "int");
                  break;
                case Void:
                  fprintf(listing, "%-13s%-13s", "Function", "void");
                  break;
                default:
                  break;
                }
              break;
            default:
              break;
            }
          }
          bkNow = bkNow->next;
        }
      }
    }
  }
}