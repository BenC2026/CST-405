/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "parser.y"

/* SYNTAX ANALYZER (PARSER) - WITH FUNCTION SUPPORT
 * This is the second phase of compilation - checking grammar rules
 * Bison generates a parser that builds an Abstract Syntax Tree (AST)
 * Now supports functions, control flow, and more operators
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* External declarations for lexer interface */
extern int yylex();
extern int yyparse();
extern FILE* yyin;
extern int yylineno;  /* Line number from scanner */

void yyerror(const char* s);
ASTNode* root = NULL;

#line 92 "parser.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_NUM = 3,                        /* NUM  */
  YYSYMBOL_ID = 4,                         /* ID  */
  YYSYMBOL_INT = 5,                        /* INT  */
  YYSYMBOL_FLOAT = 6,                      /* FLOAT  */
  YYSYMBOL_PRINT = 7,                      /* PRINT  */
  YYSYMBOL_RETURN = 8,                     /* RETURN  */
  YYSYMBOL_IF = 9,                         /* IF  */
  YYSYMBOL_ELSE = 10,                      /* ELSE  */
  YYSYMBOL_WHILE = 11,                     /* WHILE  */
  YYSYMBOL_LE = 12,                        /* LE  */
  YYSYMBOL_GE = 13,                        /* GE  */
  YYSYMBOL_EQ = 14,                        /* EQ  */
  YYSYMBOL_NE = 15,                        /* NE  */
  YYSYMBOL_16_ = 16,                       /* '<'  */
  YYSYMBOL_17_ = 17,                       /* '>'  */
  YYSYMBOL_18_ = 18,                       /* '+'  */
  YYSYMBOL_19_ = 19,                       /* '-'  */
  YYSYMBOL_20_ = 20,                       /* '*'  */
  YYSYMBOL_21_ = 21,                       /* '/'  */
  YYSYMBOL_UMINUS = 22,                    /* UMINUS  */
  YYSYMBOL_23_ = 23,                       /* '('  */
  YYSYMBOL_24_ = 24,                       /* ')'  */
  YYSYMBOL_25_ = 25,                       /* '['  */
  YYSYMBOL_26_ = 26,                       /* ']'  */
  YYSYMBOL_27_ = 27,                       /* ','  */
  YYSYMBOL_28_ = 28,                       /* '{'  */
  YYSYMBOL_29_ = 29,                       /* '}'  */
  YYSYMBOL_30_ = 30,                       /* ';'  */
  YYSYMBOL_31_ = 31,                       /* '='  */
  YYSYMBOL_YYACCEPT = 32,                  /* $accept  */
  YYSYMBOL_program = 33,                   /* program  */
  YYSYMBOL_decl_or_func_list = 34,         /* decl_or_func_list  */
  YYSYMBOL_decl_or_func = 35,              /* decl_or_func  */
  YYSYMBOL_func_def = 36,                  /* func_def  */
  YYSYMBOL_params = 37,                    /* params  */
  YYSYMBOL_param_list = 38,                /* param_list  */
  YYSYMBOL_param = 39,                     /* param  */
  YYSYMBOL_block = 40,                     /* block  */
  YYSYMBOL_stmt_list = 41,                 /* stmt_list  */
  YYSYMBOL_stmt = 42,                      /* stmt  */
  YYSYMBOL_decl = 43,                      /* decl  */
  YYSYMBOL_assign = 44,                    /* assign  */
  YYSYMBOL_return_stmt = 45,               /* return_stmt  */
  YYSYMBOL_if_stmt = 46,                   /* if_stmt  */
  YYSYMBOL_while_stmt = 47,                /* while_stmt  */
  YYSYMBOL_expr = 48,                      /* expr  */
  YYSYMBOL_func_call = 49,                 /* func_call  */
  YYSYMBOL_args = 50,                      /* args  */
  YYSYMBOL_arg_list = 51,                  /* arg_list  */
  YYSYMBOL_print_stmt = 52                 /* print_stmt  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  10
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   272

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  32
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  21
/* YYNRULES -- Number of rules.  */
#define YYNRULES  62
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  137

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   271


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      23,    24,    20,    18,    27,    19,     2,    21,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    30,
      16,    31,    17,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    25,     2,    26,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    28,     2,    29,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    22
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    53,    53,    60,    63,    70,    71,    76,    80,    84,
      88,    92,    96,   104,   108,   111,   117,   121,   125,   133,
     136,   143,   146,   153,   154,   155,   156,   157,   158,   159,
     160,   165,   170,   175,   184,   188,   196,   199,   206,   209,
     216,   223,   226,   230,   234,   237,   240,   243,   246,   249,
     252,   255,   258,   261,   264,   267,   270,   277,   281,   289,
     293,   296,   303
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "NUM", "ID", "INT",
  "FLOAT", "PRINT", "RETURN", "IF", "ELSE", "WHILE", "LE", "GE", "EQ",
  "NE", "'<'", "'>'", "'+'", "'-'", "'*'", "'/'", "UMINUS", "'('", "')'",
  "'['", "']'", "','", "'{'", "'}'", "';'", "'='", "$accept", "program",
  "decl_or_func_list", "decl_or_func", "func_def", "params", "param_list",
  "param", "block", "stmt_list", "stmt", "decl", "assign", "return_stmt",
  "if_stmt", "while_stmt", "expr", "func_call", "args", "arg_list",
  "print_stmt", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-62)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     116,    23,    39,    66,   116,   -62,   -62,   -62,   111,   -11,
     -62,   -62,    41,    19,   -62,    50,   -62,    49,    72,    59,
      -1,    51,   -62,    81,    75,    59,    85,    88,   -62,    53,
     -62,    59,   132,   109,    84,   -62,    59,   129,    60,   152,
     153,   135,     2,   136,   137,   -62,   -62,    64,   -62,   -62,
     -62,   -62,   -62,   -62,   142,   -62,   -62,   -62,   -62,    59,
     149,   -62,   -62,     5,    91,    91,   110,   144,    91,   -62,
      61,    91,    91,   -62,   112,   -62,    91,    91,   -62,   -62,
     -62,   -62,    59,   -62,   251,   151,   164,   169,   131,   173,
     199,    91,   -62,   212,    91,    91,    91,    91,    91,    91,
      91,    91,    91,    91,   -62,   225,   238,   -62,   -62,    91,
     146,   -62,   148,   184,   -62,    31,    31,    99,    99,    31,
      31,   133,   133,   -62,   -62,    95,    95,   251,    91,   -62,
     -62,   182,   -62,   150,    95,   -62,   -62
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,     2,     3,     5,     6,     0,     0,
       1,     4,     0,     0,    31,     0,    32,     0,     0,     0,
       0,    13,    14,     0,     0,     0,     0,    16,    17,     0,
       9,     0,     0,     0,     0,    10,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    20,    29,     0,    21,    23,
      24,    26,    27,    28,     0,    25,     7,    15,    33,     0,
       0,     8,    18,     0,     0,     0,     0,     0,     0,    41,
      42,     0,     0,    37,     0,    44,     0,     0,    19,    22,
      30,    12,     0,    58,    60,     0,    59,     0,     0,     0,
       0,     0,    55,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    36,     0,     0,    11,    57,     0,
       0,    34,     0,     0,    56,    51,    52,    53,    54,    49,
      50,    45,    46,    47,    48,     0,     0,    61,     0,    62,
      43,    38,    40,     0,     0,    35,    39
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -62,   -62,   -62,   175,   -62,    29,   -62,   161,    -5,   -62,
     -46,    13,   -62,   -62,   -62,   -62,   -61,   -29,   -62,   -62,
     -62
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     3,     4,     5,     6,    20,    21,    22,    46,    47,
      48,    49,    50,    51,    52,    53,    74,    75,    85,    86,
      55
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      54,    79,    84,    87,    88,    69,    70,    90,    69,    70,
      92,    93,    15,     7,    30,   105,   106,     7,    54,    16,
      35,    71,    23,    31,    71,    72,    56,     8,    72,    83,
     113,    61,    73,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,     9,    26,    24,    17,    18,   127,   100,
     101,   102,   103,    27,    81,    17,    18,    38,    39,    40,
      41,    42,    43,    60,    44,    19,    10,   133,    38,    39,
      40,    41,    42,    43,    25,    44,    28,   107,    32,   131,
     132,    29,    45,    63,    63,    64,    91,    29,   136,    17,
      18,    65,    29,    78,    69,    70,    54,    54,    34,    38,
      39,    40,    41,    42,    43,    54,    44,    33,    59,    36,
      71,    94,    95,    37,    72,    98,    99,   100,   101,   102,
     103,     1,     2,    29,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,    12,    89,    13,    17,    18,    58,
      14,    14,   104,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   102,   103,    62,    66,    67,    68,    76,
      77,   111,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,    80,    82,    16,   108,    23,   128,   129,    11,
     135,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   109,   134,    57,     0,   110,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,     0,     0,     0,     0,
     130,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,     0,     0,   112,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,     0,     0,   114,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,     0,     0,   125,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
       0,     0,   126,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103
};

static const yytype_int16 yycheck[] =
{
      29,    47,    63,    64,    65,     3,     4,    68,     3,     4,
      71,    72,    23,     0,    19,    76,    77,     4,    47,    30,
      25,    19,     3,    24,    19,    23,    31,     4,    23,    24,
      91,    36,    30,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,     4,    15,    26,     5,     6,   109,    18,
      19,    20,    21,     4,    59,     5,     6,     4,     5,     6,
       7,     8,     9,    34,    11,    24,     0,   128,     4,     5,
       6,     7,     8,     9,    24,    11,     4,    82,    27,   125,
     126,    28,    29,    23,    23,    25,    25,    28,   134,     5,
       6,    31,    28,    29,     3,     4,   125,   126,    23,     4,
       5,     6,     7,     8,     9,   134,    11,    26,    24,    24,
      19,    12,    13,    25,    23,    16,    17,    18,    19,    20,
      21,     5,     6,    28,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    23,    25,    25,     5,     6,    30,
      30,    30,    30,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    20,    21,    26,     4,     4,    23,    23,
      23,    30,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    30,    24,    30,    24,     3,    31,    30,     4,
      30,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    27,    10,    32,    -1,    26,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    -1,    -1,    -1,    -1,
      26,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    -1,    -1,    24,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    -1,    -1,    24,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    -1,    -1,    24,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      -1,    -1,    24,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     5,     6,    33,    34,    35,    36,    43,     4,     4,
       0,    35,    23,    25,    30,    23,    30,     5,     6,    24,
      37,    38,    39,     3,    26,    24,    37,     4,     4,    28,
      40,    24,    27,    26,    23,    40,    24,    25,     4,     5,
       6,     7,     8,     9,    11,    29,    40,    41,    42,    43,
      44,    45,    46,    47,    49,    52,    40,    39,    30,    24,
      37,    40,    26,    23,    25,    31,     4,     4,    23,     3,
       4,    19,    23,    30,    48,    49,    23,    23,    29,    42,
      30,    40,    24,    24,    48,    50,    51,    48,    48,    25,
      48,    25,    48,    48,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    30,    48,    48,    40,    24,    27,
      26,    30,    24,    48,    24,    48,    48,    48,    48,    48,
      48,    48,    48,    48,    48,    24,    24,    48,    31,    30,
      26,    42,    42,    48,    10,    30,    42
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    32,    33,    34,    34,    35,    35,    36,    36,    36,
      36,    36,    36,    37,    38,    38,    39,    39,    39,    40,
      40,    41,    41,    42,    42,    42,    42,    42,    42,    42,
      42,    43,    43,    43,    44,    44,    45,    45,    46,    46,
      47,    48,    48,    48,    48,    48,    48,    48,    48,    48,
      48,    48,    48,    48,    48,    48,    48,    49,    49,    50,
      51,    51,    52
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     2,     1,     1,     6,     6,     5,
       5,     8,     7,     1,     1,     3,     2,     2,     4,     3,
       2,     1,     2,     1,     1,     1,     1,     1,     1,     1,
       2,     3,     3,     6,     4,     7,     3,     2,     5,     7,
       5,     1,     1,     4,     1,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     3,     4,     3,     1,
       1,     3,     5
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* program: decl_or_func_list  */
#line 53 "parser.y"
                      {
        root = (yyvsp[0].node);
    }
#line 1242 "parser.tab.c"
    break;

  case 3: /* decl_or_func_list: decl_or_func  */
#line 60 "parser.y"
                 {
        (yyval.node) = (yyvsp[0].node);
    }
#line 1250 "parser.tab.c"
    break;

  case 4: /* decl_or_func_list: decl_or_func_list decl_or_func  */
#line 63 "parser.y"
                                     {
        (yyval.node) = createStmtList((yyvsp[-1].node), (yyvsp[0].node));
    }
#line 1258 "parser.tab.c"
    break;

  case 5: /* decl_or_func: func_def  */
#line 70 "parser.y"
             { (yyval.node) = (yyvsp[0].node); }
#line 1264 "parser.tab.c"
    break;

  case 6: /* decl_or_func: decl  */
#line 71 "parser.y"
           { (yyval.node) = (yyvsp[0].node); }
#line 1270 "parser.tab.c"
    break;

  case 7: /* func_def: INT ID '(' params ')' block  */
#line 76 "parser.y"
                                {
        (yyval.node) = createFuncDef((yyvsp[-4].str), (yyvsp[-2].node), (yyvsp[0].node));
        free((yyvsp[-4].str));
    }
#line 1279 "parser.tab.c"
    break;

  case 8: /* func_def: FLOAT ID '(' params ')' block  */
#line 80 "parser.y"
                                    {
        (yyval.node) = createFuncDef((yyvsp[-4].str), (yyvsp[-2].node), (yyvsp[0].node));
        free((yyvsp[-4].str));
    }
#line 1288 "parser.tab.c"
    break;

  case 9: /* func_def: INT ID '(' ')' block  */
#line 84 "parser.y"
                           {
        (yyval.node) = createFuncDef((yyvsp[-3].str), NULL, (yyvsp[0].node));
        free((yyvsp[-3].str));
    }
#line 1297 "parser.tab.c"
    break;

  case 10: /* func_def: FLOAT ID '(' ')' block  */
#line 88 "parser.y"
                             {
        (yyval.node) = createFuncDef((yyvsp[-3].str), NULL, (yyvsp[0].node));
        free((yyvsp[-3].str));
    }
#line 1306 "parser.tab.c"
    break;

  case 11: /* func_def: INT ID '[' ']' '(' params ')' block  */
#line 92 "parser.y"
                                          {
        (yyval.node) = createFuncDef((yyvsp[-6].str), (yyvsp[-2].node), (yyvsp[0].node));
        free((yyvsp[-6].str));
    }
#line 1315 "parser.tab.c"
    break;

  case 12: /* func_def: INT ID '[' ']' '(' ')' block  */
#line 96 "parser.y"
                                   {
        (yyval.node) = createFuncDef((yyvsp[-5].str), NULL, (yyvsp[0].node));
        free((yyvsp[-5].str));
    }
#line 1324 "parser.tab.c"
    break;

  case 13: /* params: param_list  */
#line 104 "parser.y"
               { (yyval.node) = (yyvsp[0].node); }
#line 1330 "parser.tab.c"
    break;

  case 14: /* param_list: param  */
#line 108 "parser.y"
          {
        (yyval.node) = (yyvsp[0].node);
    }
#line 1338 "parser.tab.c"
    break;

  case 15: /* param_list: param_list ',' param  */
#line 111 "parser.y"
                           {
        (yyval.node) = createParamList((yyvsp[-2].node), (yyvsp[0].node));
    }
#line 1346 "parser.tab.c"
    break;

  case 16: /* param: INT ID  */
#line 117 "parser.y"
           {
        (yyval.node) = createParam((yyvsp[0].str));
        free((yyvsp[0].str));
    }
#line 1355 "parser.tab.c"
    break;

  case 17: /* param: FLOAT ID  */
#line 121 "parser.y"
               {
        (yyval.node) = createParam((yyvsp[0].str));
        free((yyvsp[0].str));
    }
#line 1364 "parser.tab.c"
    break;

  case 18: /* param: INT ID '[' ']'  */
#line 125 "parser.y"
                     {
        (yyval.node) = createParam((yyvsp[-2].str));
        free((yyvsp[-2].str));
    }
#line 1373 "parser.tab.c"
    break;

  case 19: /* block: '{' stmt_list '}'  */
#line 133 "parser.y"
                      {
        (yyval.node) = createBlock((yyvsp[-1].node));
    }
#line 1381 "parser.tab.c"
    break;

  case 20: /* block: '{' '}'  */
#line 136 "parser.y"
              {
        (yyval.node) = createBlock(NULL);
    }
#line 1389 "parser.tab.c"
    break;

  case 21: /* stmt_list: stmt  */
#line 143 "parser.y"
         {
        (yyval.node) = (yyvsp[0].node);
    }
#line 1397 "parser.tab.c"
    break;

  case 22: /* stmt_list: stmt_list stmt  */
#line 146 "parser.y"
                     {
        (yyval.node) = createStmtList((yyvsp[-1].node), (yyvsp[0].node));
    }
#line 1405 "parser.tab.c"
    break;

  case 30: /* stmt: func_call ';'  */
#line 160 "parser.y"
                    { (yyval.node) = (yyvsp[-1].node); }
#line 1411 "parser.tab.c"
    break;

  case 31: /* decl: INT ID ';'  */
#line 165 "parser.y"
               {
        (yyval.node) = createDecl((yyvsp[-1].str), "int");
        addVar((yyvsp[-1].str), "int");
        free((yyvsp[-1].str));
    }
#line 1421 "parser.tab.c"
    break;

  case 32: /* decl: FLOAT ID ';'  */
#line 170 "parser.y"
                   {
        (yyval.node) = createDecl((yyvsp[-1].str), "float");
        addVar((yyvsp[-1].str), "float");
        free((yyvsp[-1].str));
    }
#line 1431 "parser.tab.c"
    break;

  case 33: /* decl: INT ID '[' NUM ']' ';'  */
#line 175 "parser.y"
                             {
        (yyval.node) = createArrayDecl((yyvsp[-4].str), "int", (yyvsp[-2].num));
        addArrayVar((yyvsp[-4].str), "int", (yyvsp[-2].num));
        free((yyvsp[-4].str));
    }
#line 1441 "parser.tab.c"
    break;

  case 34: /* assign: ID '=' expr ';'  */
#line 184 "parser.y"
                    {
        (yyval.node) = createAssign((yyvsp[-3].str), (yyvsp[-1].node));
        free((yyvsp[-3].str));
    }
#line 1450 "parser.tab.c"
    break;

  case 35: /* assign: ID '[' expr ']' '=' expr ';'  */
#line 188 "parser.y"
                                   {
        (yyval.node) = createArrayAssign((yyvsp[-6].str), (yyvsp[-4].node), (yyvsp[-1].node));
        free((yyvsp[-6].str));
    }
#line 1459 "parser.tab.c"
    break;

  case 36: /* return_stmt: RETURN expr ';'  */
#line 196 "parser.y"
                    {
        (yyval.node) = createReturn((yyvsp[-1].node));
    }
#line 1467 "parser.tab.c"
    break;

  case 37: /* return_stmt: RETURN ';'  */
#line 199 "parser.y"
                 {
        (yyval.node) = createReturn(NULL);
    }
#line 1475 "parser.tab.c"
    break;

  case 38: /* if_stmt: IF '(' expr ')' stmt  */
#line 206 "parser.y"
                         {
        (yyval.node) = createIf((yyvsp[-2].node), (yyvsp[0].node), NULL);
    }
#line 1483 "parser.tab.c"
    break;

  case 39: /* if_stmt: IF '(' expr ')' stmt ELSE stmt  */
#line 209 "parser.y"
                                     {
        (yyval.node) = createIf((yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 1491 "parser.tab.c"
    break;

  case 40: /* while_stmt: WHILE '(' expr ')' stmt  */
#line 216 "parser.y"
                            {
        (yyval.node) = createWhile((yyvsp[-2].node), (yyvsp[0].node));
    }
#line 1499 "parser.tab.c"
    break;

  case 41: /* expr: NUM  */
#line 223 "parser.y"
        {
        (yyval.node) = createNum((yyvsp[0].num));
    }
#line 1507 "parser.tab.c"
    break;

  case 42: /* expr: ID  */
#line 226 "parser.y"
         {
        (yyval.node) = createVar((yyvsp[0].str));
        free((yyvsp[0].str));
    }
#line 1516 "parser.tab.c"
    break;

  case 43: /* expr: ID '[' expr ']'  */
#line 230 "parser.y"
                      {
        (yyval.node) = createArrayAccess((yyvsp[-3].str), (yyvsp[-1].node));
        free((yyvsp[-3].str));
    }
#line 1525 "parser.tab.c"
    break;

  case 44: /* expr: func_call  */
#line 234 "parser.y"
                {
        (yyval.node) = (yyvsp[0].node);
    }
#line 1533 "parser.tab.c"
    break;

  case 45: /* expr: expr '+' expr  */
#line 237 "parser.y"
                    {
        (yyval.node) = createBinOp('+', (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 1541 "parser.tab.c"
    break;

  case 46: /* expr: expr '-' expr  */
#line 240 "parser.y"
                    {
        (yyval.node) = createBinOp('-', (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 1549 "parser.tab.c"
    break;

  case 47: /* expr: expr '*' expr  */
#line 243 "parser.y"
                    {
        (yyval.node) = createBinOp('*', (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 1557 "parser.tab.c"
    break;

  case 48: /* expr: expr '/' expr  */
#line 246 "parser.y"
                    {
        (yyval.node) = createBinOp('/', (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 1565 "parser.tab.c"
    break;

  case 49: /* expr: expr '<' expr  */
#line 249 "parser.y"
                    {
        (yyval.node) = createBinOp('<', (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 1573 "parser.tab.c"
    break;

  case 50: /* expr: expr '>' expr  */
#line 252 "parser.y"
                    {
        (yyval.node) = createBinOp('>', (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 1581 "parser.tab.c"
    break;

  case 51: /* expr: expr LE expr  */
#line 255 "parser.y"
                   {
        (yyval.node) = createBinOp('l', (yyvsp[-2].node), (yyvsp[0].node));  /* 'l' for <= */
    }
#line 1589 "parser.tab.c"
    break;

  case 52: /* expr: expr GE expr  */
#line 258 "parser.y"
                   {
        (yyval.node) = createBinOp('g', (yyvsp[-2].node), (yyvsp[0].node));  /* 'g' for >= */
    }
#line 1597 "parser.tab.c"
    break;

  case 53: /* expr: expr EQ expr  */
#line 261 "parser.y"
                   {
        (yyval.node) = createBinOp('e', (yyvsp[-2].node), (yyvsp[0].node));  /* 'e' for == */
    }
#line 1605 "parser.tab.c"
    break;

  case 54: /* expr: expr NE expr  */
#line 264 "parser.y"
                   {
        (yyval.node) = createBinOp('n', (yyvsp[-2].node), (yyvsp[0].node));  /* 'n' for != */
    }
#line 1613 "parser.tab.c"
    break;

  case 55: /* expr: '-' expr  */
#line 267 "parser.y"
                            {
        (yyval.node) = createBinOp('u', (yyvsp[0].node), NULL);  /* 'u' for unary minus */
    }
#line 1621 "parser.tab.c"
    break;

  case 56: /* expr: '(' expr ')'  */
#line 270 "parser.y"
                   {
        (yyval.node) = (yyvsp[-1].node);
    }
#line 1629 "parser.tab.c"
    break;

  case 57: /* func_call: ID '(' args ')'  */
#line 277 "parser.y"
                    {
        (yyval.node) = createFuncCall((yyvsp[-3].str), (yyvsp[-1].node));
        free((yyvsp[-3].str));
    }
#line 1638 "parser.tab.c"
    break;

  case 58: /* func_call: ID '(' ')'  */
#line 281 "parser.y"
                 {
        (yyval.node) = createFuncCall((yyvsp[-2].str), NULL);
        free((yyvsp[-2].str));
    }
#line 1647 "parser.tab.c"
    break;

  case 59: /* args: arg_list  */
#line 289 "parser.y"
             { (yyval.node) = (yyvsp[0].node); }
#line 1653 "parser.tab.c"
    break;

  case 60: /* arg_list: expr  */
#line 293 "parser.y"
         {
        (yyval.node) = (yyvsp[0].node);  /* Single argument becomes the arg node */
    }
#line 1661 "parser.tab.c"
    break;

  case 61: /* arg_list: arg_list ',' expr  */
#line 296 "parser.y"
                        {
        (yyval.node) = createArgList((yyvsp[-2].node), (yyvsp[0].node));
    }
#line 1669 "parser.tab.c"
    break;

  case 62: /* print_stmt: PRINT '(' expr ')' ';'  */
#line 303 "parser.y"
                           {
        (yyval.node) = createPrint((yyvsp[-2].node));
    }
#line 1677 "parser.tab.c"
    break;


#line 1681 "parser.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 308 "parser.y"


/* ERROR HANDLING */
void yyerror(const char* s) {
    fprintf(stderr, "Syntax Error at line %d: %s\n", yylineno, s);
}
