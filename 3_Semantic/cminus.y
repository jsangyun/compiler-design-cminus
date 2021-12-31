/****************************************************/
/* File: tiny.y                                     */
/* The TINY Yacc/Bison specification file           */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

%{
static int yylex();
int yyerror(char * message);
%}

%{
#define YYPARSER /* distinguishes Yacc output from other code files */

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

#define YYSTYPE TreeNode *
static char * savedName[100]; /* for use in assignments */
static int savedNameIndex = 1;  /* ditto */
static int savedNum; /* for saving array const */
static TreeNode * savedTree; /* stores syntax tree for later return */

%}

%token IF ELSE INT VOID WHILE RETURN
%token ID NUM
%token ASSIGN EQ LT PLUS MINUS TIMES OVER LPAREN RPAREN SEMI NE LE GE GT LBRACE RBRACE LCURLY RCURLY COMMA
%token ERROR

%nonassoc RPAREN
%nonassoc ELSE

%% /* Grammar for TINY */

program     : declaration_list
                { savedTree = $1;} 
            ;

declaration_list : declaration_list declaration {
    YYSTYPE t = $1;
    if (t != NULL) 
    {
        while(t->sibling != NULL)
        {
            t = t->sibling;
        }
        t->sibling = $2;
        $$ = $1;
    } else {
        $$ = $2;
    }
}
|   declaration {$$ = $1;};


id : ID {
    savedName[savedNameIndex] = copyString(tokenString);
    savedNameIndex++;
}

num : NUM {
    savedNum = atoi(tokenString);
}

declaration : var_declaration {$$ = $1;} | fun_declaration {$$ = $1;};

var_declaration : INT id SEMI {
    $$ = newDclNode(VarK);
    $$->attr.name = savedName[savedNameIndex-1];
    $$->lineno = lineno;
    $$->type = Integer;
} 
| VOID id SEMI {
    $$ = newDclNode(VarK);
    $$->attr.name = savedName[savedNameIndex-1];
    $$->lineno = lineno;
    $$->type = Void;
} 
| INT id LBRACE num RBRACE SEMI {
    $$ = newDclNode(VarK);
    $$->type = IntArray;
    $$->attr.name = savedName[savedNameIndex-1];
    $$->lineno = lineno;
    $$->child[0] = newExpNode(ConstK);
    $$->child[0]->attr.val = savedNum;
}
| VOID id LBRACE num RBRACE SEMI {
    $$ = newDclNode(VarK);
    $$->type = VoidArray;
    $$->attr.name = savedName[savedNameIndex-1];
    $$->lineno = lineno;
    $$->child[0] = newExpNode(ConstK);
    $$->child[0]->attr.val = savedNum;
};

fun_declaration : INT id {
    $$ = newDclNode(FuncK);
    $$->attr.name = savedName[savedNameIndex-1];
    $$->type = Integer;
    $$->lineno = lineno;
} LPAREN params RPAREN compound_stmt {
    $$ = $3;
    $$->child[1] = $5;
    $$->child[2] = $7;
}
| VOID id {
    $$ = newDclNode(FuncK);
    $$->attr.name = savedName[savedNameIndex-1];
    $$->type = Void;
    $$->lineno = lineno;
} LPAREN params RPAREN compound_stmt {
    $$ = $3;
    $$->child[1] = $5;
    $$->child[2] = $7;
};

params : param_list {$$ = $1;}
        | VOID {
            $$ = newPrmNode(VoidParamK);
            $$->lineno = lineno;
        };

param_list : param_list COMMA param {
    YYSTYPE t = $1;
    if (t != NULL) 
    {
        while(t->sibling != NULL)
        {
            t = t->sibling;
        }
        t->sibling = $3;
        $$ = $1;
    } else {
        $$ = $3;
    }
} | param {$$ = $1;};

param : INT id {
    $$ = newPrmNode(ParamK);
    $$->type = Integer;
    $$->attr.name = savedName[savedNameIndex-1];
} | VOID id {
    $$ = newPrmNode(ParamK);
    $$->type = Void;
    $$->attr.name = savedName[savedNameIndex-1];
} | INT id LBRACE RBRACE {
    $$ = newPrmNode(ParamK);
    $$->type = IntArray;
    $$->attr.name = savedName[savedNameIndex-1];
} | VOID id LBRACE RBRACE {
    $$ = newPrmNode(ParamK);
    $$->type = VoidArray;
    $$->attr.name = savedName[savedNameIndex-1];
};

compound_stmt : LCURLY local_declarations statement_list RCURLY {
    $$ = newStmtNode(CompK);
    $$->child[0] = $2;
    $$->child[1] = $3;
};

local_declarations : local_declarations var_declaration {
    YYSTYPE t = $1;
    if (t != NULL) 
    {
        while(t->sibling != NULL)
        {
            t = t->sibling;
        }
        t->sibling = $2;
        $$ = $1;
    } else {
        $$ = $2;
    }
} | {$$ = NULL;};

statement_list : statement_list statement {
    YYSTYPE t = $1;
    if (t != NULL) 
    {
        while(t->sibling != NULL)
        {
            t = t->sibling;
        }
        t->sibling = $2;
        $$ = $1;
    } else {
        $$ = $2;
    } 
} | {$$ = NULL;};

statement : expression_stmt {$$ = $1;} | compound_stmt {$$ = $1;} | selection_stmt {$$ = $1;}
    | iteration_stmt {$$ = $1;} | return_stmt {$$ = $1;};

expression_stmt : expression SEMI {$$ = $1;}
    | SEMI {$$ = NULL;};

selection_stmt : IF LPAREN expression RPAREN statement {
    $$=newStmtNode(IfK);
    $$->child[0] = $3;
    $$->child[1] = $5;
    $$->child[2] = NULL;
} 
| IF LPAREN expression RPAREN statement ELSE statement {
    $$ = newStmtNode(IfElseK);
    $$->child[0] = $3;
    $$->child[1] = $5;
    $$->child[2] = $7;
};

iteration_stmt : WHILE LPAREN expression RPAREN statement {
    $$ = newStmtNode(IterK);
    $$->child[0] = $3;
    $$->child[1] = $5;
};

return_stmt : RETURN SEMI {
    $$ = newStmtNode(RetK);
    $$->child[0] = NULL;
}
| RETURN expression SEMI {
    $$ = newStmtNode(RetK);
    $$->child[0] = $2;
};

expression : var ASSIGN expression {
    $$ = newExpNode(AssignK);
    $$->child[0] = $1;
    $$->child[1] = $3;
}
| simple_expression {$$ = $1;};

var : id {
    $$ = newExpNode(IdK);
    $$->attr.name = savedName[savedNameIndex-1];
} | id LBRACE expression RBRACE {
    $$ = newExpNode(IdK);
    $$->attr.name = savedName[savedNameIndex-2];
    $$->child[0] = $3;
};

simple_expression : additive_expression EQ additive_expression {
    $$ = newExpNode(OpK);
    $$->child[0] = $1;
    $$->child[1] = $3;
    $$->attr.op = EQ;
}
| additive_expression NE additive_expression {
    $$ = newExpNode(OpK);
    $$->child[0] = $1;
    $$->child[1] = $3;
    $$->attr.op = NE;
}
| additive_expression GT additive_expression {
    $$ = newExpNode(OpK);
    $$->child[0] = $1;
    $$->child[1] = $3;
    $$->attr.op = GT;
}
| additive_expression GE additive_expression {
    $$ = newExpNode(OpK);
    $$->child[0] = $1;
    $$->child[1] = $3;
    $$->attr.op = GE;
}
| additive_expression LT additive_expression {
    $$ = newExpNode(OpK);
    $$->child[0] = $1;
    $$->child[1] = $3;
    $$->attr.op = LT;
}
| additive_expression LE additive_expression {
    $$ = newExpNode(OpK);
    $$->child[0] = $1;
    $$->child[1] = $3;
    $$->attr.op = LE;
}
| additive_expression {$$ = $1;};

additive_expression : additive_expression PLUS term {
    $$ = newExpNode(OpK);
    $$->child[0] = $1;
    $$->child[1] = $3;
    $$->attr.op = PLUS;
}
| additive_expression MINUS term {
    $$ = newExpNode(OpK);
    $$->child[0] = $1;
    $$->child[1] = $3;
    $$->attr.op = MINUS;
}
| term {$$ = $1;};

term : term TIMES factor {
    $$ = newExpNode(OpK);
    $$->child[0] = $1;
    $$->child[1] = $3;
    $$->attr.op = TIMES;
}
| term OVER factor {
    $$ = newExpNode(OpK);
    $$->child[0] = $1;
    $$->child[1] = $3;
    $$->attr.op = OVER;
}
| factor {$$ = $1;};

factor : LPAREN expression RPAREN {
    $$ = $2;
}
| var {$$ = $1;} | call {$$ = $1;} | num {
    $$ = newExpNode(ConstK);
    $$->attr.val = savedNum;
};

call : id {
    $$ = newExpNode(CallK);
    $$->attr.name = savedName[savedNameIndex-1];
} LPAREN args RPAREN {
    $$ = $2;
    $$->child[0] = $4;
};

args : arg_list {$$ = $1;}
    | {$$ = NULL;};

arg_list : arg_list COMMA expression {
    YYSTYPE t = $1;
    if (t != NULL) 
    {
        while(t->sibling != NULL)
        {
            t = t->sibling;
        }
        t->sibling = $3;
        $$ = $1;
    } else {
        $$ = $3;
    } 
}
| expression {$$ = $1;};

%%

int yyerror(char * message)
{ fprintf(listing,"Syntax error at line %d: %s\n",lineno,message);
  fprintf(listing,"Current token: ");
  printToken(yychar,tokenString);
  Error = TRUE;
  return 0;
}

/* yylex calls getToken to make Yacc/Bison output
 * compatible with ealier versions of the TINY scanner
 */
static int yylex(void)
{ return getToken(); }

TreeNode * parse(void)
{ yyparse();
  return savedTree;
}

