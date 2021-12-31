/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     IF = 258,
     ELSE = 259,
     INT = 260,
     VOID = 261,
     WHILE = 262,
     RETURN = 263,
     ID = 264,
     NUM = 265,
     ASSIGN = 266,
     EQ = 267,
     LT = 268,
     PLUS = 269,
     MINUS = 270,
     TIMES = 271,
     OVER = 272,
     LPAREN = 273,
     RPAREN = 274,
     SEMI = 275,
     NE = 276,
     LE = 277,
     GE = 278,
     GT = 279,
     LBRACE = 280,
     RBRACE = 281,
     LCURLY = 282,
     RCURLY = 283,
     COMMA = 284,
     ERROR = 285
   };
#endif
/* Tokens.  */
#define IF 258
#define ELSE 259
#define INT 260
#define VOID 261
#define WHILE 262
#define RETURN 263
#define ID 264
#define NUM 265
#define ASSIGN 266
#define EQ 267
#define LT 268
#define PLUS 269
#define MINUS 270
#define TIMES 271
#define OVER 272
#define LPAREN 273
#define RPAREN 274
#define SEMI 275
#define NE 276
#define LE 277
#define GE 278
#define GT 279
#define LBRACE 280
#define RBRACE 281
#define LCURLY 282
#define RCURLY 283
#define COMMA 284
#define ERROR 285




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

