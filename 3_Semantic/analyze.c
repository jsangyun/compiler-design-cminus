/****************************************************/
/* File: analyze.c                                  */
/* Semantic analyzer implementation                 */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "symtab.h"
#include "analyze.h"
#include "util.h"

/* counter for variable memory locations */
static int location = 0;

static ScopeList globalScope = NULL;
static char * funcName;
static int inScope = 0;

/* Procedure traverse is a generic recursive 
 * syntax tree traversal routine:
 * it applies preProc in preorder and postProc 
 * in postorder to tree pointed to by t
 */
static void traverse( TreeNode * t,
               void (* preProc) (TreeNode *),
               void (* postProc) (TreeNode *) )
{ if (t != NULL)
  { preProc(t);
    { int i;
      for (i=0; i < MAXCHILDREN; i++)
        traverse(t->child[i],preProc,postProc);
    }
    postProc(t);
    traverse(t->sibling,preProc,postProc);
  }
}

/* nullProc is a do-nothing procedure to 
 * generate preorder-only or postorder-only
 * traversals from traverse
 */
static void nullProc(TreeNode * t)
{ if (t==NULL) return;
  else return;
}
static void typeError(TreeNode * t, char * error) {
  fprintf(listing, "Error: Type error at line %d: %s\n", t->lineno, error);
  Error = TRUE;
}

static void undefinedError(TreeNode * t) {
  if(t->kind.exp == CallK) {
    fprintf(listing, "Undeclared Function \"%s\" at line %d\n", t->attr.name, t->lineno);
  }
  else if(t->kind.exp == IdK) {
    fprintf(listing, "Undeclared Function \"%s\" at line %d\n", t->attr.name, t->lineno);
  }
  Error = TRUE;
}

static void redefinedError(TreeNode * t) {
  if(t->nodekind == DclK) {
    if(t->kind.dcl == VarK) {
      fprintf(listing,"redefined variable \"%s\" at line %d\n",t->attr.name,t->lineno);
    }
    else if(t->kind.dcl == FuncK) {
      fprintf(listing,"redefined function \"%s\" at line %d\n",t->attr.name,t->lineno);
    }
  }
  Error = TRUE;
}

static void invalidArrayIndexError(TreeNode * t) {
  fprintf(listing, "Invalid array indexing at line %d (name : %s). indices should be integer\n", t->lineno, t->attr.name);
}

static void voidVariableError(TreeNode * t, char * name) {
  fprintf(listing, "Error: Variable Type cannot be Void at line %d (name : %s)\n", t->lineno, name);
  Error = TRUE;
}

static void func(TreeNode * t, char * name) {
  fprintf(listing, "Error: Variable Type cannot be Void at line %d (name : %s)\n", t->lineno, name);
  Error = TRUE;
}

/* Procedure insertNode inserts 
 * identifiers stored in t into 
 * the symbol table 
 */
static void insertNode( TreeNode * t)
{ switch (t->nodekind)
  { case StmtK:
      switch (t->kind.stmt)
      {
        case CompK:
          if(inScope) {
            inScope = FALSE;
          } else {
            ScopeList sc = create_scope(funcName);
            scope_push(sc);
            location++;
          }
          t->sc = scope_now();
          break;
        default:
          break;
      }
      break;
    case ExpK:
      switch (t->kind.exp)
      { case IdK:
          break;
        case CallK:
          if(st_lookup(t->attr.name) == -1) {
            undefinedError(t);
          } else {
            line_add(t->attr.name, t->lineno);
          }
          break;
        default:
          break;
      }
      break;
    case DclK:
      switch (t->kind.dcl)
      {
      case VarK:
        break;
      case FuncK:
        funcName = t->attr.name;
        if(st_lookup_scope_now(funcName) >= 0) {
          redefinedError(t);
        }
        st_insert(funcName, t, t->lineno, loc_add());
        ScopeList sc = create_scope(funcName);
        scope_push(sc);
        inScope = TRUE;

        switch (t->child[0]->type)
        {
        case Integer:
          break;
        default:
          break;
        }
        break;
      default:
        break;
      }
    default:
      break;
  }
}

/* Function buildSymtab constructs the symbol 
 * table by preorder traversal of the syntax tree
 */
void buildSymtab(TreeNode * syntaxTree)
{ traverse(syntaxTree,insertNode,nullProc);
  if (TraceAnalyze)
  { fprintf(listing,"\nSymbol table:\n\n");
    printSymTab(listing);
  }
}
/* Procedure checkNode performs
 * type checking at a single tree node
 */
static void checkNode(TreeNode * t)
{ switch (t->nodekind)
  { case ExpK:
      switch (t->kind.exp)
      { case OpK:
          if ((t->child[0]->type != Integer) ||
              (t->child[1]->type != Integer))
            typeError(t,"Op applied to non-integer");
          break;
        case ConstK:
        case IdK:
          t->type = Integer;
          break;
        default:
          break;
      }
      break;
    case StmtK:
      switch (t->kind.stmt)
      { case IfK:
          if (t->child[0]->type == Integer)
            typeError(t->child[0],"if test is not Boolean");
          break;
        case AssignK:
          if (t->child[0]->type != Integer)
            typeError(t->child[0],"assignment of non-integer value");
          break;
        default:
          break;
      }
      break;
    default:
      break;

  }
}

/* Procedure typeCheck performs type checking 
 * by a postorder syntax tree traversal
 */
void typeCheck(TreeNode * syntaxTree)
{ traverse(syntaxTree,nullProc,checkNode);
}
