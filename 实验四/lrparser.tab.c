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
#line 1 "lrparser.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "ast.h"
#include "lrparser.tab.h"

int yylex(void);
void yyerror(const char *s);

past ast_root = NULL;


/* 复制字符串，避免依赖非标准的 strdup */
static char *copy_text(const char *s)
{
    size_t len;
    char *p;

    if (s == NULL)
        return NULL;

    len = strlen(s);
    p = (char *)malloc(len + 1);

    if (p == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    memcpy(p, s, len + 1);
    return p;
}

/* 构造二元运算节点 */
static past make_binary(int op, past left, past right)
{
    past node = newAstNode(BINARY_OPERATOR);
    node->ivalue = op;
    node->left = left;
    node->right = right;
    return node;
}

/* 构造一元运算节点 */
static past make_unary(int op, past child)
{
    past node = newAstNode(UNARY_OPERATOR);
    node->ivalue = op;
    node->left = child;
    return node;
}

/* 构造变量声明节点 */
static past make_var_decl(char *name, past dims, past init)
{
    past var = newAstNode(VAR_DECL);
    var->svalue = copy_text(name);
    /* Array dimensions belong to the declared type, not to the AST children
       shown for a VarDecl. Keep the initializer as its first AST child. */
    var->left = init;
    var->right = dims;
    return var;
}

/* 构造声明语句节点 */
static past make_decl_stmt(past declarations, int is_const)
{
    past node = newAstNode(DECL_STMT);
    node->ivalue = is_const;
    node->left = declarations;
    return node;
}

/* 构造函数节点 */
static past make_func(char *name, past params, past body)
{
    past node = newAstNode(FUNCTION_DECL);
    node->svalue = copy_text(name);
    node->left = params;
    node->right = body;
    return node;
}

/* 构造函数参数节点 */
static past make_param(char *name, past dimensions)
{
    past node = newAstNode(PARM_DECL);
    node->svalue = copy_text(name);
    if (dimensions) dimensions->right = NULL;
    node->right = dimensions;
    return node;
}

/* 构造初始化列表 */
static past make_init_list(past values)
{
    past node = newAstNode(INIT_LIST_EXPR);
    node->left = values;
    return node;
}

/* 构造复合语句 */
static past make_block(past items)
{
    past node = newAstNode(COMPOUND_STMT);
    node->left = items;
    return node;
}

/* 构造函数调用 */
static past make_call(char *name, past args)
{
    past id = newID(name);
    past node = newAstNode(CALL_EXPR);
    node->left = id;
    node->right = args;
    return node;
}

#line 193 "lrparser.tab.c"

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

#include "lrparser.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_Y_INT = 3,                      /* Y_INT  */
  YYSYMBOL_Y_FLOAT = 4,                    /* Y_FLOAT  */
  YYSYMBOL_Y_VOID = 5,                     /* Y_VOID  */
  YYSYMBOL_Y_CONST = 6,                    /* Y_CONST  */
  YYSYMBOL_Y_IF = 7,                       /* Y_IF  */
  YYSYMBOL_Y_ELSE = 8,                     /* Y_ELSE  */
  YYSYMBOL_Y_WHILE = 9,                    /* Y_WHILE  */
  YYSYMBOL_Y_BREAK = 10,                   /* Y_BREAK  */
  YYSYMBOL_Y_CONTINUE = 11,                /* Y_CONTINUE  */
  YYSYMBOL_Y_RETURN = 12,                  /* Y_RETURN  */
  YYSYMBOL_Y_ID = 13,                      /* Y_ID  */
  YYSYMBOL_num_INT = 14,                   /* num_INT  */
  YYSYMBOL_num_FLOAT = 15,                 /* num_FLOAT  */
  YYSYMBOL_Y_ADD = 16,                     /* Y_ADD  */
  YYSYMBOL_Y_SUB = 17,                     /* Y_SUB  */
  YYSYMBOL_Y_MUL = 18,                     /* Y_MUL  */
  YYSYMBOL_Y_DIV = 19,                     /* Y_DIV  */
  YYSYMBOL_Y_MODULO = 20,                  /* Y_MODULO  */
  YYSYMBOL_Y_LESS = 21,                    /* Y_LESS  */
  YYSYMBOL_Y_LESSEQ = 22,                  /* Y_LESSEQ  */
  YYSYMBOL_Y_GREAT = 23,                   /* Y_GREAT  */
  YYSYMBOL_Y_GREATEQ = 24,                 /* Y_GREATEQ  */
  YYSYMBOL_Y_NOTEQ = 25,                   /* Y_NOTEQ  */
  YYSYMBOL_Y_EQ = 26,                      /* Y_EQ  */
  YYSYMBOL_Y_NOT = 27,                     /* Y_NOT  */
  YYSYMBOL_Y_AND = 28,                     /* Y_AND  */
  YYSYMBOL_Y_OR = 29,                      /* Y_OR  */
  YYSYMBOL_Y_ASSIGN = 30,                  /* Y_ASSIGN  */
  YYSYMBOL_Y_LPAR = 31,                    /* Y_LPAR  */
  YYSYMBOL_Y_RPAR = 32,                    /* Y_RPAR  */
  YYSYMBOL_Y_LBRACKET = 33,                /* Y_LBRACKET  */
  YYSYMBOL_Y_RBRACKET = 34,                /* Y_RBRACKET  */
  YYSYMBOL_Y_LSQUARE = 35,                 /* Y_LSQUARE  */
  YYSYMBOL_Y_RSQUARE = 36,                 /* Y_RSQUARE  */
  YYSYMBOL_Y_COMMA = 37,                   /* Y_COMMA  */
  YYSYMBOL_Y_SEMICOLON = 38,               /* Y_SEMICOLON  */
  YYSYMBOL_LOWER_THAN_ELSE = 39,           /* LOWER_THAN_ELSE  */
  YYSYMBOL_YYACCEPT = 40,                  /* $accept  */
  YYSYMBOL_CompUnits = 41,                 /* CompUnits  */
  YYSYMBOL_CompUnit = 42,                  /* CompUnit  */
  YYSYMBOL_Decl = 43,                      /* Decl  */
  YYSYMBOL_ConstDecl = 44,                 /* ConstDecl  */
  YYSYMBOL_ConstDefs = 45,                 /* ConstDefs  */
  YYSYMBOL_ConstDef = 46,                  /* ConstDef  */
  YYSYMBOL_ConstExps = 47,                 /* ConstExps  */
  YYSYMBOL_ConstInitVal = 48,              /* ConstInitVal  */
  YYSYMBOL_ConstInitVals = 49,             /* ConstInitVals  */
  YYSYMBOL_VarDecl = 50,                   /* VarDecl  */
  YYSYMBOL_VarDecls = 51,                  /* VarDecls  */
  YYSYMBOL_VarDef = 52,                    /* VarDef  */
  YYSYMBOL_InitVal = 53,                   /* InitVal  */
  YYSYMBOL_InitVals = 54,                  /* InitVals  */
  YYSYMBOL_FuncDef = 55,                   /* FuncDef  */
  YYSYMBOL_FuncParams = 56,                /* FuncParams  */
  YYSYMBOL_FuncParam = 57,                 /* FuncParam  */
  YYSYMBOL_Block = 58,                     /* Block  */
  YYSYMBOL_BlockItems = 59,                /* BlockItems  */
  YYSYMBOL_BlockItem = 60,                 /* BlockItem  */
  YYSYMBOL_Stmt = 61,                      /* Stmt  */
  YYSYMBOL_Exp = 62,                       /* Exp  */
  YYSYMBOL_Cond = 63,                      /* Cond  */
  YYSYMBOL_LVal = 64,                      /* LVal  */
  YYSYMBOL_ArraySubscripts = 65,           /* ArraySubscripts  */
  YYSYMBOL_PrimaryExp = 66,                /* PrimaryExp  */
  YYSYMBOL_UnaryExp = 67,                  /* UnaryExp  */
  YYSYMBOL_FuncCall = 68,                  /* FuncCall  */
  YYSYMBOL_CallParams = 69,                /* CallParams  */
  YYSYMBOL_MulExp = 70,                    /* MulExp  */
  YYSYMBOL_AddExp = 71,                    /* AddExp  */
  YYSYMBOL_RelExp = 72,                    /* RelExp  */
  YYSYMBOL_EqExp = 73,                     /* EqExp  */
  YYSYMBOL_LAndExp = 74,                   /* LAndExp  */
  YYSYMBOL_LOrExp = 75,                    /* LOrExp  */
  YYSYMBOL_Type = 76                       /* Type  */
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
#define YYFINAL  14
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   276

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  40
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  37
/* YYNRULES -- Number of rules.  */
#define YYNRULES  100
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  188

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   294


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
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   174,   174,   179,   187,   193,   200,   204,   213,   221,
     225,   232,   236,   245,   250,   259,   263,   267,   274,   278,
     287,   295,   299,   306,   310,   314,   318,   325,   329,   333,
     340,   344,   353,   357,   361,   365,   370,   375,   383,   387,
     394,   398,   403,   408,   420,   424,   431,   435,   442,   448,
     457,   461,   465,   469,   473,   479,   485,   492,   496,   500,
     504,   514,   521,   528,   532,   551,   556,   565,   570,   574,
     578,   585,   589,   593,   597,   601,   608,   612,   619,   623,
     630,   634,   638,   642,   649,   653,   657,   664,   668,   672,
     676,   680,   687,   691,   695,   702,   706,   713,   717,   724,
     725
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
  "\"end of file\"", "error", "\"invalid token\"", "Y_INT", "Y_FLOAT",
  "Y_VOID", "Y_CONST", "Y_IF", "Y_ELSE", "Y_WHILE", "Y_BREAK",
  "Y_CONTINUE", "Y_RETURN", "Y_ID", "num_INT", "num_FLOAT", "Y_ADD",
  "Y_SUB", "Y_MUL", "Y_DIV", "Y_MODULO", "Y_LESS", "Y_LESSEQ", "Y_GREAT",
  "Y_GREATEQ", "Y_NOTEQ", "Y_EQ", "Y_NOT", "Y_AND", "Y_OR", "Y_ASSIGN",
  "Y_LPAR", "Y_RPAR", "Y_LBRACKET", "Y_RBRACKET", "Y_LSQUARE", "Y_RSQUARE",
  "Y_COMMA", "Y_SEMICOLON", "LOWER_THAN_ELSE", "$accept", "CompUnits",
  "CompUnit", "Decl", "ConstDecl", "ConstDefs", "ConstDef", "ConstExps",
  "ConstInitVal", "ConstInitVals", "VarDecl", "VarDecls", "VarDef",
  "InitVal", "InitVals", "FuncDef", "FuncParams", "FuncParam", "Block",
  "BlockItems", "BlockItem", "Stmt", "Exp", "Cond", "LVal",
  "ArraySubscripts", "PrimaryExp", "UnaryExp", "FuncCall", "CallParams",
  "MulExp", "AddExp", "RelExp", "EqExp", "LAndExp", "LOrExp", "Type", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-77)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      68,   -77,   -77,     0,    24,    63,   -77,   -77,   -77,   -77,
     -77,    32,   -11,    74,   -77,   -77,     7,    -2,   -77,    14,
      82,    20,   -77,   211,    21,   237,    98,    76,   -77,    90,
     103,    19,   -77,    96,   232,    99,    74,   -77,    64,   -77,
     -77,   237,   237,   237,   237,   184,   -77,   -77,   -77,   -77,
     -77,   -77,   186,    48,   141,   147,   114,   109,   119,   103,
     107,   131,   211,   237,   100,   -77,   103,   143,   -77,   103,
      24,   140,   206,   -77,   -77,   232,   -77,    16,   237,   149,
     -77,   -77,   -77,   146,   -77,   -77,    84,   237,   237,   237,
     237,   237,   237,   237,   237,   237,   237,   237,   237,   237,
     103,   -77,   103,   -77,   -77,   133,   -77,   165,   171,   170,
     178,   110,   -77,   -77,   -77,   -77,   176,   -77,   -77,   196,
     205,    76,   -77,   -77,    77,   149,   -77,   -77,   134,   -77,
     -77,   -77,   129,   200,   237,   -77,   -77,   211,   -77,   -77,
     -77,   186,   186,    48,    48,    48,    48,   141,   141,   147,
     114,   -77,   -77,   -77,   237,   237,   -77,   -77,   -77,   203,
     -77,   -77,   -77,   237,   208,   -77,   232,   -77,   237,   -77,
     219,   -77,   -77,   224,   225,   -77,   220,   149,   -77,   -77,
     -77,    69,    69,   -77,   252,   -77,    69,   -77
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,    99,   100,     0,     0,     0,     2,     4,     6,     7,
       5,     0,     0,     0,     1,     3,    23,     0,    21,     0,
       0,     0,     9,     0,     0,     0,    25,     0,    20,     0,
       0,     0,    38,     0,     0,     0,     0,     8,    63,    69,
      70,     0,     0,     0,     0,     0,    24,    27,    68,    71,
      80,    72,    84,    87,    92,    95,    97,    61,     0,     0,
       0,     0,     0,     0,    23,    22,     0,     0,    35,     0,
       0,    40,     0,    11,    15,     0,    10,     0,     0,    64,
      73,    74,    75,     0,    28,    30,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    32,     0,    13,    26,     0,    36,     0,     0,     0,
       0,     0,    44,    51,    48,    53,     0,    46,    49,     0,
      68,     0,    37,    39,     0,    41,    16,    18,     0,    12,
      76,    78,     0,     0,     0,    67,    29,     0,    81,    82,
      83,    85,    86,    88,    89,    90,    91,    94,    93,    96,
      98,    33,    34,    14,     0,     0,    57,    58,    59,     0,
      45,    47,    52,     0,    42,    17,     0,    77,     0,    65,
       0,    31,    62,     0,     0,    60,     0,    43,    19,    79,
      66,     0,     0,    50,    55,    54,     0,    56
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -77,   -77,   256,   -57,   -77,   -77,   226,   246,   -63,   -77,
     -77,   -77,   240,   -39,   -77,   -77,   245,   201,   -25,   -77,
     154,   -76,   -23,   117,   -66,   -67,   -77,   -27,   -77,   -77,
     104,   137,   116,   175,   177,   -77,     3
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     5,     6,     7,     8,    21,    22,    26,    73,   128,
       9,    17,    18,    46,    86,    10,    31,    32,   115,   116,
     117,   118,   119,   173,    48,    79,    49,    50,    51,   132,
      52,    53,    54,    55,    56,    57,    33
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      47,   120,    61,    11,   125,    68,    85,    13,    11,   127,
     114,    74,   129,    12,    80,    81,    82,     1,     2,    29,
      19,    83,    47,   104,     1,     2,    58,     1,     2,    38,
      39,    40,    41,    42,   101,    27,    28,    23,    24,    47,
     105,   106,    25,    43,   122,    16,    30,    44,   130,    74,
     120,    69,    74,    59,   131,   133,    70,    36,    37,   114,
     138,   139,   140,    14,    90,    91,     1,     2,     3,     4,
     121,     1,     2,     3,     4,   151,   107,   152,   108,   109,
     110,   111,    38,    39,    40,    41,    42,    20,   159,    64,
      38,    39,    40,    41,    42,    77,    43,   177,   171,    78,
      44,   133,    67,   178,    43,   184,   185,   113,    44,    71,
     187,   170,    34,   164,    47,   120,   120,    25,   136,   121,
     120,   137,    66,    38,    39,    40,    41,    42,    62,    75,
      23,   172,   172,    63,    63,    25,    67,    43,    99,   102,
     176,    44,    98,    74,    70,   179,     1,     2,   158,     4,
     107,   100,   108,   109,   110,   111,    38,    39,    40,    41,
      42,   167,    92,    93,    94,    95,   168,   103,   165,   153,
      43,   166,    96,    97,    44,   124,    67,   112,   135,     1,
       2,   113,     4,   107,   134,   108,   109,   110,   111,    38,
      39,    40,    41,    42,   141,   142,   154,    38,    39,    40,
      41,    42,   155,    43,    87,    88,    89,    44,   156,    67,
     160,    43,   147,   148,   113,    44,   157,    45,    84,    38,
      39,    40,    41,    42,    38,    39,    40,    41,    42,   143,
     144,   145,   146,    43,   162,   163,   169,    44,    43,    72,
     126,   175,    44,    78,    45,    38,    39,    40,    41,    42,
      38,    39,    40,    41,    42,   180,   181,   182,   183,    43,
     186,    15,    76,    44,    43,    72,    35,    65,    44,    60,
     161,   123,   174,   149,     0,     0,   150
};

static const yytype_int16 yycheck[] =
{
      23,    67,    25,     0,    71,    30,    45,     4,     5,    72,
      67,    34,    75,    13,    41,    42,    43,     3,     4,     5,
      31,    44,    45,    62,     3,     4,     5,     3,     4,    13,
      14,    15,    16,    17,    59,    37,    38,    30,    31,    62,
      63,    66,    35,    27,    69,    13,    32,    31,    32,    72,
     116,    32,    75,    32,    77,    78,    37,    37,    38,   116,
      87,    88,    89,     0,    16,    17,     3,     4,     5,     6,
      67,     3,     4,     5,     6,   100,     7,   102,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    13,   111,    13,
      13,    14,    15,    16,    17,    31,    27,   164,   137,    35,
      31,   124,    33,   166,    27,   181,   182,    38,    31,    13,
     186,   134,    30,    36,   137,   181,   182,    35,    34,   116,
     186,    37,    32,    13,    14,    15,    16,    17,    30,    30,
      30,   154,   155,    35,    35,    35,    33,    27,    29,    32,
     163,    31,    28,   166,    37,   168,     3,     4,    38,     6,
       7,    32,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    32,    21,    22,    23,    24,    37,    36,    34,    36,
      27,    37,    25,    26,    31,    35,    33,    34,    32,     3,
       4,    38,     6,     7,    35,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    90,    91,    31,    13,    14,    15,
      16,    17,    31,    27,    18,    19,    20,    31,    38,    33,
      34,    27,    96,    97,    38,    31,    38,    33,    34,    13,
      14,    15,    16,    17,    13,    14,    15,    16,    17,    92,
      93,    94,    95,    27,    38,    30,    36,    31,    27,    33,
      34,    38,    31,    35,    33,    13,    14,    15,    16,    17,
      13,    14,    15,    16,    17,    36,    32,    32,    38,    27,
       8,     5,    36,    31,    27,    33,    20,    27,    31,    24,
     116,    70,   155,    98,    -1,    -1,    99
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     4,     5,     6,    41,    42,    43,    44,    50,
      55,    76,    13,    76,     0,    42,    13,    51,    52,    31,
      13,    45,    46,    30,    31,    35,    47,    37,    38,     5,
      32,    56,    57,    76,    30,    47,    37,    38,    13,    14,
      15,    16,    17,    27,    31,    33,    53,    62,    64,    66,
      67,    68,    70,    71,    72,    73,    74,    75,     5,    32,
      56,    62,    30,    35,    13,    52,    32,    33,    58,    32,
      37,    13,    33,    48,    62,    30,    46,    31,    35,    65,
      67,    67,    67,    62,    34,    53,    54,    18,    19,    20,
      16,    17,    21,    22,    23,    24,    25,    26,    28,    29,
      32,    58,    32,    36,    53,    62,    58,     7,     9,    10,
      11,    12,    34,    38,    43,    58,    59,    60,    61,    62,
      64,    76,    58,    57,    35,    65,    34,    48,    49,    48,
      32,    62,    69,    62,    35,    32,    34,    37,    67,    67,
      67,    70,    70,    71,    71,    71,    71,    72,    72,    73,
      74,    58,    58,    36,    31,    31,    38,    38,    38,    62,
      34,    60,    38,    30,    36,    34,    37,    32,    37,    36,
      62,    53,    62,    63,    63,    38,    62,    65,    48,    62,
      36,    32,    32,    38,    61,    61,     8,    61
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    40,    41,    41,    42,    42,    43,    43,    44,    45,
      45,    46,    46,    47,    47,    48,    48,    48,    49,    49,
      50,    51,    51,    52,    52,    52,    52,    53,    53,    53,
      54,    54,    55,    55,    55,    55,    55,    55,    56,    56,
      57,    57,    57,    57,    58,    58,    59,    59,    60,    60,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
      61,    62,    63,    64,    64,    65,    65,    66,    66,    66,
      66,    67,    67,    67,    67,    67,    68,    68,    69,    69,
      70,    70,    70,    70,    71,    71,    71,    72,    72,    72,
      72,    72,    73,    73,    73,    74,    74,    75,    75,    76,
      76
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     1,     1,     4,     1,
       3,     3,     4,     3,     4,     1,     2,     3,     1,     3,
       3,     1,     3,     1,     3,     2,     4,     1,     2,     3,
       1,     3,     5,     6,     6,     5,     6,     6,     1,     3,
       2,     3,     4,     5,     2,     3,     1,     2,     1,     1,
       4,     1,     2,     1,     5,     5,     7,     2,     2,     2,
       3,     1,     1,     1,     2,     3,     4,     3,     1,     1,
       1,     1,     1,     2,     2,     2,     3,     4,     1,     3,
       1,     3,     3,     3,     1,     3,     3,     1,     3,     3,
       3,     3,     1,     3,     3,     1,     3,     1,     3,     1,
       1
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
  case 2: /* CompUnits: CompUnit  */
#line 175 "lrparser.y"
      {
          (yyval.node) = (yyvsp[0].node);
          ast_root = (yyval.node);
      }
#line 1404 "lrparser.tab.c"
    break;

  case 3: /* CompUnits: CompUnits CompUnit  */
#line 180 "lrparser.y"
      {
          (yyval.node) = appendAst((yyvsp[-1].node), (yyvsp[0].node));
          ast_root = (yyval.node);
      }
#line 1413 "lrparser.tab.c"
    break;

  case 4: /* CompUnit: Decl  */
#line 188 "lrparser.y"
      {
          /* At translation-unit scope Clang exposes each VarDecl directly;
             DECL_STMT is used only for declarations inside a compound stmt. */
          (yyval.node) = (yyvsp[0].node) != NULL ? (yyvsp[0].node)->left : NULL;
      }
#line 1423 "lrparser.tab.c"
    break;

  case 5: /* CompUnit: FuncDef  */
#line 194 "lrparser.y"
      {
          (yyval.node) = (yyvsp[0].node);
      }
#line 1431 "lrparser.tab.c"
    break;

  case 6: /* Decl: ConstDecl  */
#line 201 "lrparser.y"
      {
          (yyval.node) = (yyvsp[0].node);
      }
#line 1439 "lrparser.tab.c"
    break;

  case 7: /* Decl: VarDecl  */
#line 205 "lrparser.y"
      {
          (yyval.node) = (yyvsp[0].node);
      }
#line 1447 "lrparser.tab.c"
    break;

  case 8: /* ConstDecl: Y_CONST Type ConstDefs Y_SEMICOLON  */
#line 214 "lrparser.y"
      {
          (yyval.node) = make_decl_stmt((yyvsp[-1].node), 1);
          for (past var = (yyvsp[-1].node); var; var = var->next) var->is_float = (yyvsp[-2].ivalue) == Y_FLOAT;
      }
#line 1456 "lrparser.tab.c"
    break;

  case 9: /* ConstDefs: ConstDef  */
#line 222 "lrparser.y"
      {
          (yyval.node) = (yyvsp[0].node);
      }
#line 1464 "lrparser.tab.c"
    break;

  case 10: /* ConstDefs: ConstDefs Y_COMMA ConstDef  */
#line 226 "lrparser.y"
      {
          (yyval.node) = appendAst((yyvsp[-2].node), (yyvsp[0].node));
      }
#line 1472 "lrparser.tab.c"
    break;

  case 11: /* ConstDef: Y_ID Y_ASSIGN ConstInitVal  */
#line 233 "lrparser.y"
      {
          (yyval.node) = make_var_decl((yyvsp[-2].svalue), NULL, (yyvsp[0].node)); (yyval.node)->is_const = 1;
      }
#line 1480 "lrparser.tab.c"
    break;

  case 12: /* ConstDef: Y_ID ConstExps Y_ASSIGN ConstInitVal  */
#line 237 "lrparser.y"
      {
          /* The accepted ConstDef action keeps the written brace structure;
             only VarDef performs scalar-run grouping. Bounds are type data. */
          (yyval.node) = make_var_decl((yyvsp[-3].svalue), (yyvsp[-2].node), (yyvsp[0].node)); (yyval.node)->is_const = 1;
      }
#line 1490 "lrparser.tab.c"
    break;

  case 13: /* ConstExps: Y_LSQUARE Exp Y_RSQUARE  */
#line 246 "lrparser.y"
      {
          (yyval.node) = newAstNode(ARRAY_SUBSCRIPT_EXPR);
          (yyval.node)->right = (yyvsp[-1].node);
      }
#line 1499 "lrparser.tab.c"
    break;

  case 14: /* ConstExps: ConstExps Y_LSQUARE Exp Y_RSQUARE  */
#line 251 "lrparser.y"
      {
          past dim = newAstNode(ARRAY_SUBSCRIPT_EXPR);
          dim->right = (yyvsp[-1].node);
          (yyval.node) = appendAst((yyvsp[-3].node), dim);
      }
#line 1509 "lrparser.tab.c"
    break;

  case 15: /* ConstInitVal: Exp  */
#line 260 "lrparser.y"
      {
          (yyval.node) = (yyvsp[0].node);
      }
#line 1517 "lrparser.tab.c"
    break;

  case 16: /* ConstInitVal: Y_LBRACKET Y_RBRACKET  */
#line 264 "lrparser.y"
      {
          (yyval.node) = make_init_list(NULL);
      }
#line 1525 "lrparser.tab.c"
    break;

  case 17: /* ConstInitVal: Y_LBRACKET ConstInitVals Y_RBRACKET  */
#line 268 "lrparser.y"
      {
          (yyval.node) = make_init_list((yyvsp[-1].node));
      }
#line 1533 "lrparser.tab.c"
    break;

  case 18: /* ConstInitVals: ConstInitVal  */
#line 275 "lrparser.y"
      {
          (yyval.node) = (yyvsp[0].node);
      }
#line 1541 "lrparser.tab.c"
    break;

  case 19: /* ConstInitVals: ConstInitVals Y_COMMA ConstInitVal  */
#line 279 "lrparser.y"
      {
          (yyval.node) = appendAst((yyvsp[-2].node), (yyvsp[0].node));
      }
#line 1549 "lrparser.tab.c"
    break;

  case 20: /* VarDecl: Type VarDecls Y_SEMICOLON  */
#line 288 "lrparser.y"
      {
          (yyval.node) = make_decl_stmt((yyvsp[-1].node), 0);
          for (past var = (yyvsp[-1].node); var; var = var->next) var->is_float = (yyvsp[-2].ivalue) == Y_FLOAT;
      }
#line 1558 "lrparser.tab.c"
    break;

  case 21: /* VarDecls: VarDef  */
#line 296 "lrparser.y"
      {
          (yyval.node) = (yyvsp[0].node);
      }
#line 1566 "lrparser.tab.c"
    break;

  case 22: /* VarDecls: VarDecls Y_COMMA VarDef  */
#line 300 "lrparser.y"
      {
          (yyval.node) = appendAst((yyvsp[-2].node), (yyvsp[0].node));
      }
#line 1574 "lrparser.tab.c"
    break;

  case 23: /* VarDef: Y_ID  */
#line 307 "lrparser.y"
      {
          (yyval.node) = make_var_decl((yyvsp[0].svalue), NULL, NULL);
      }
#line 1582 "lrparser.tab.c"
    break;

  case 24: /* VarDef: Y_ID Y_ASSIGN InitVal  */
#line 311 "lrparser.y"
      {
          (yyval.node) = make_var_decl((yyvsp[-2].svalue), NULL, (yyvsp[0].node));
      }
#line 1590 "lrparser.tab.c"
    break;

  case 25: /* VarDef: Y_ID ConstExps  */
#line 315 "lrparser.y"
      {
          (yyval.node) = make_var_decl((yyvsp[-1].svalue), (yyvsp[0].node), NULL);
      }
#line 1598 "lrparser.tab.c"
    break;

  case 26: /* VarDef: Y_ID ConstExps Y_ASSIGN InitVal  */
#line 319 "lrparser.y"
      {
          (yyval.node) = make_var_decl((yyvsp[-3].svalue), (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 1606 "lrparser.tab.c"
    break;

  case 27: /* InitVal: Exp  */
#line 326 "lrparser.y"
      {
          (yyval.node) = (yyvsp[0].node);
      }
#line 1614 "lrparser.tab.c"
    break;

  case 28: /* InitVal: Y_LBRACKET Y_RBRACKET  */
#line 330 "lrparser.y"
      {
          (yyval.node) = make_init_list(NULL);
      }
#line 1622 "lrparser.tab.c"
    break;

  case 29: /* InitVal: Y_LBRACKET InitVals Y_RBRACKET  */
#line 334 "lrparser.y"
      {
          (yyval.node) = make_init_list((yyvsp[-1].node));
      }
#line 1630 "lrparser.tab.c"
    break;

  case 30: /* InitVals: InitVal  */
#line 341 "lrparser.y"
      {
          (yyval.node) = (yyvsp[0].node);
      }
#line 1638 "lrparser.tab.c"
    break;

  case 31: /* InitVals: InitVals Y_COMMA InitVal  */
#line 345 "lrparser.y"
      {
          (yyval.node) = appendAst((yyvsp[-2].node), (yyvsp[0].node));
      }
#line 1646 "lrparser.tab.c"
    break;

  case 32: /* FuncDef: Type Y_ID Y_LPAR Y_RPAR Block  */
#line 354 "lrparser.y"
      {
          (yyval.node) = make_func((yyvsp[-3].svalue), NULL, (yyvsp[0].node)); (yyval.node)->is_float = (yyvsp[-4].ivalue) == Y_FLOAT;
      }
#line 1654 "lrparser.tab.c"
    break;

  case 33: /* FuncDef: Type Y_ID Y_LPAR Y_VOID Y_RPAR Block  */
#line 358 "lrparser.y"
      {
          (yyval.node) = make_func((yyvsp[-4].svalue), NULL, (yyvsp[0].node)); (yyval.node)->is_float = (yyvsp[-5].ivalue) == Y_FLOAT;
      }
#line 1662 "lrparser.tab.c"
    break;

  case 34: /* FuncDef: Type Y_ID Y_LPAR FuncParams Y_RPAR Block  */
#line 362 "lrparser.y"
      {
          (yyval.node) = make_func((yyvsp[-4].svalue), (yyvsp[-2].node), (yyvsp[0].node)); (yyval.node)->is_float = (yyvsp[-5].ivalue) == Y_FLOAT;
      }
#line 1670 "lrparser.tab.c"
    break;

  case 35: /* FuncDef: Y_VOID Y_ID Y_LPAR Y_RPAR Block  */
#line 366 "lrparser.y"
      {
          (yyval.node) = make_func((yyvsp[-3].svalue), NULL, (yyvsp[0].node));
          (yyval.node)->ivalue = Y_VOID;
      }
#line 1679 "lrparser.tab.c"
    break;

  case 36: /* FuncDef: Y_VOID Y_ID Y_LPAR Y_VOID Y_RPAR Block  */
#line 371 "lrparser.y"
      {
          (yyval.node) = make_func((yyvsp[-4].svalue), NULL, (yyvsp[0].node));
          (yyval.node)->ivalue = Y_VOID;
      }
#line 1688 "lrparser.tab.c"
    break;

  case 37: /* FuncDef: Y_VOID Y_ID Y_LPAR FuncParams Y_RPAR Block  */
#line 376 "lrparser.y"
      {
          (yyval.node) = make_func((yyvsp[-4].svalue), (yyvsp[-2].node), (yyvsp[0].node));
          (yyval.node)->ivalue = Y_VOID;
      }
#line 1697 "lrparser.tab.c"
    break;

  case 38: /* FuncParams: FuncParam  */
#line 384 "lrparser.y"
      {
          (yyval.node) = (yyvsp[0].node);
      }
#line 1705 "lrparser.tab.c"
    break;

  case 39: /* FuncParams: FuncParams Y_COMMA FuncParam  */
#line 388 "lrparser.y"
      {
          (yyval.node) = appendAst((yyvsp[-2].node), (yyvsp[0].node));
      }
#line 1713 "lrparser.tab.c"
    break;

  case 40: /* FuncParam: Type Y_ID  */
#line 395 "lrparser.y"
      {
          (yyval.node) = make_param((yyvsp[0].svalue), NULL); (yyval.node)->is_float = (yyvsp[-1].ivalue) == Y_FLOAT;
      }
#line 1721 "lrparser.tab.c"
    break;

  case 41: /* FuncParam: Type Y_ID ArraySubscripts  */
#line 399 "lrparser.y"
      {
          if ((yyvsp[-2].ivalue) == Y_FLOAT) { yyerror("float array parameters are not supported"); YYABORT; }
          (yyval.node) = make_param((yyvsp[-1].svalue), (yyvsp[0].node));
      }
#line 1730 "lrparser.tab.c"
    break;

  case 42: /* FuncParam: Type Y_ID Y_LSQUARE Y_RSQUARE  */
#line 404 "lrparser.y"
      {
          if ((yyvsp[-3].ivalue) == Y_FLOAT) { yyerror("float array parameters are not supported"); YYABORT; }
          (yyval.node) = make_param((yyvsp[-2].svalue), newAstNode(ARRAY_SUBSCRIPT_EXPR));
      }
#line 1739 "lrparser.tab.c"
    break;

  case 43: /* FuncParam: Type Y_ID Y_LSQUARE Y_RSQUARE ArraySubscripts  */
#line 409 "lrparser.y"
      {
          past first = newAstNode(ARRAY_SUBSCRIPT_EXPR);
          first->next = (yyvsp[0].node);
          if ((yyvsp[-4].ivalue) == Y_FLOAT) { yyerror("float array parameters are not supported"); YYABORT; }
          (yyval.node) = make_param((yyvsp[-3].svalue), first);
      }
#line 1750 "lrparser.tab.c"
    break;

  case 44: /* Block: Y_LBRACKET Y_RBRACKET  */
#line 421 "lrparser.y"
      {
          (yyval.node) = make_block(NULL);
      }
#line 1758 "lrparser.tab.c"
    break;

  case 45: /* Block: Y_LBRACKET BlockItems Y_RBRACKET  */
#line 425 "lrparser.y"
      {
          (yyval.node) = make_block((yyvsp[-1].node));
      }
#line 1766 "lrparser.tab.c"
    break;

  case 46: /* BlockItems: BlockItem  */
#line 432 "lrparser.y"
      {
          (yyval.node) = (yyvsp[0].node);
      }
#line 1774 "lrparser.tab.c"
    break;

  case 47: /* BlockItems: BlockItems BlockItem  */
#line 436 "lrparser.y"
      {
          (yyval.node) = appendAst((yyvsp[-1].node), (yyvsp[0].node));
      }
#line 1782 "lrparser.tab.c"
    break;

  case 48: /* BlockItem: Decl  */
#line 443 "lrparser.y"
      {
          /* The accepted lab implementation emits local const declarations
             directly, while ordinary local declarations retain DECL_STMT. */
          (yyval.node) = (yyvsp[0].node) != NULL && (yyvsp[0].node)->ivalue == 1 ? (yyvsp[0].node)->left : (yyvsp[0].node);
      }
#line 1792 "lrparser.tab.c"
    break;

  case 49: /* BlockItem: Stmt  */
#line 449 "lrparser.y"
      {
          (yyval.node) = (yyvsp[0].node);
      }
#line 1800 "lrparser.tab.c"
    break;

  case 50: /* Stmt: LVal Y_ASSIGN Exp Y_SEMICOLON  */
#line 458 "lrparser.y"
      {
          (yyval.node) = make_binary(Y_ASSIGN, (yyvsp[-3].node), (yyvsp[-1].node));
      }
#line 1808 "lrparser.tab.c"
    break;

  case 51: /* Stmt: Y_SEMICOLON  */
#line 462 "lrparser.y"
      {
          (yyval.node) = newAstNode(NULL_STMT);
      }
#line 1816 "lrparser.tab.c"
    break;

  case 52: /* Stmt: Exp Y_SEMICOLON  */
#line 466 "lrparser.y"
      {
          (yyval.node) = (yyvsp[-1].node);
      }
#line 1824 "lrparser.tab.c"
    break;

  case 53: /* Stmt: Block  */
#line 470 "lrparser.y"
      {
          (yyval.node) = (yyvsp[0].node);
      }
#line 1832 "lrparser.tab.c"
    break;

  case 54: /* Stmt: Y_WHILE Y_LPAR Cond Y_RPAR Stmt  */
#line 474 "lrparser.y"
      {
          (yyval.node) = newAstNode(WHILE_STMT);
          (yyval.node)->left = (yyvsp[-2].node);
          (yyval.node)->right = (yyvsp[0].node);
      }
#line 1842 "lrparser.tab.c"
    break;

  case 55: /* Stmt: Y_IF Y_LPAR Cond Y_RPAR Stmt  */
#line 480 "lrparser.y"
      {
          (yyval.node) = newAstNode(IF_STMT);
          (yyval.node)->if_cond = (yyvsp[-2].node);
          (yyval.node)->left = (yyvsp[0].node);
      }
#line 1852 "lrparser.tab.c"
    break;

  case 56: /* Stmt: Y_IF Y_LPAR Cond Y_RPAR Stmt Y_ELSE Stmt  */
#line 486 "lrparser.y"
      {
          (yyval.node) = newAstNode(IF_STMT);
          (yyval.node)->if_cond = (yyvsp[-4].node);
          (yyval.node)->left = (yyvsp[-2].node);
          (yyval.node)->right = (yyvsp[0].node);
      }
#line 1863 "lrparser.tab.c"
    break;

  case 57: /* Stmt: Y_BREAK Y_SEMICOLON  */
#line 493 "lrparser.y"
      {
          (yyval.node) = newAstNode(BREAK_STMT);
      }
#line 1871 "lrparser.tab.c"
    break;

  case 58: /* Stmt: Y_CONTINUE Y_SEMICOLON  */
#line 497 "lrparser.y"
      {
          (yyval.node) = newAstNode(CONTINUE_STMT);
      }
#line 1879 "lrparser.tab.c"
    break;

  case 59: /* Stmt: Y_RETURN Y_SEMICOLON  */
#line 501 "lrparser.y"
      {
          (yyval.node) = newAstNode(RETURN_STMT);
      }
#line 1887 "lrparser.tab.c"
    break;

  case 60: /* Stmt: Y_RETURN Exp Y_SEMICOLON  */
#line 505 "lrparser.y"
      {
          (yyval.node) = newAstNode(RETURN_STMT);
          (yyval.node)->left = (yyvsp[-1].node);
      }
#line 1896 "lrparser.tab.c"
    break;

  case 61: /* Exp: LOrExp  */
#line 515 "lrparser.y"
      {
          (yyval.node) = (yyvsp[0].node);
      }
#line 1904 "lrparser.tab.c"
    break;

  case 62: /* Cond: Exp  */
#line 522 "lrparser.y"
      {
          (yyval.node) = (yyvsp[0].node);
      }
#line 1912 "lrparser.tab.c"
    break;

  case 63: /* LVal: Y_ID  */
#line 529 "lrparser.y"
      {
          (yyval.node) = newID((yyvsp[0].svalue));
      }
#line 1920 "lrparser.tab.c"
    break;

  case 64: /* LVal: Y_ID ArraySubscripts  */
#line 533 "lrparser.y"
      {
          past base = newID((yyvsp[-1].svalue));
          past subs = (yyvsp[0].node);
          past cur = subs;

          while (cur != NULL) {
              past next = cur->next;
              cur->left = base;
              cur->next = NULL;
              base = cur;
              cur = next;
          }

          (yyval.node) = base;
      }
#line 1940 "lrparser.tab.c"
    break;

  case 65: /* ArraySubscripts: Y_LSQUARE Exp Y_RSQUARE  */
#line 552 "lrparser.y"
      {
          (yyval.node) = newAstNode(ARRAY_SUBSCRIPT_EXPR);
          (yyval.node)->right = (yyvsp[-1].node);
      }
#line 1949 "lrparser.tab.c"
    break;

  case 66: /* ArraySubscripts: ArraySubscripts Y_LSQUARE Exp Y_RSQUARE  */
#line 557 "lrparser.y"
      {
          past dim = newAstNode(ARRAY_SUBSCRIPT_EXPR);
          dim->right = (yyvsp[-1].node);
          (yyval.node) = appendAst((yyvsp[-3].node), dim);
      }
#line 1959 "lrparser.tab.c"
    break;

  case 67: /* PrimaryExp: Y_LPAR Exp Y_RPAR  */
#line 566 "lrparser.y"
      {
          (yyval.node) = newAstNode(PAREN_EXPR);
          (yyval.node)->left = (yyvsp[-1].node);
      }
#line 1968 "lrparser.tab.c"
    break;

  case 68: /* PrimaryExp: LVal  */
#line 571 "lrparser.y"
      {
          (yyval.node) = (yyvsp[0].node);
      }
#line 1976 "lrparser.tab.c"
    break;

  case 69: /* PrimaryExp: num_INT  */
#line 575 "lrparser.y"
      {
          (yyval.node) = newInt((yyvsp[0].ivalue));
      }
#line 1984 "lrparser.tab.c"
    break;

  case 70: /* PrimaryExp: num_FLOAT  */
#line 579 "lrparser.y"
      {
          (yyval.node) = newFloat((yyvsp[0].fvalue));
      }
#line 1992 "lrparser.tab.c"
    break;

  case 71: /* UnaryExp: PrimaryExp  */
#line 586 "lrparser.y"
      {
          (yyval.node) = (yyvsp[0].node);
      }
#line 2000 "lrparser.tab.c"
    break;

  case 72: /* UnaryExp: FuncCall  */
#line 590 "lrparser.y"
      {
          (yyval.node) = (yyvsp[0].node);
      }
#line 2008 "lrparser.tab.c"
    break;

  case 73: /* UnaryExp: Y_ADD UnaryExp  */
#line 594 "lrparser.y"
      {
          (yyval.node) = make_unary(Y_ADD, (yyvsp[0].node));
      }
#line 2016 "lrparser.tab.c"
    break;

  case 74: /* UnaryExp: Y_SUB UnaryExp  */
#line 598 "lrparser.y"
      {
          (yyval.node) = make_unary(Y_SUB, (yyvsp[0].node));
      }
#line 2024 "lrparser.tab.c"
    break;

  case 75: /* UnaryExp: Y_NOT UnaryExp  */
#line 602 "lrparser.y"
      {
          (yyval.node) = make_unary(Y_NOT, (yyvsp[0].node));
      }
#line 2032 "lrparser.tab.c"
    break;

  case 76: /* FuncCall: Y_ID Y_LPAR Y_RPAR  */
#line 609 "lrparser.y"
      {
          (yyval.node) = make_call((yyvsp[-2].svalue), NULL);
      }
#line 2040 "lrparser.tab.c"
    break;

  case 77: /* FuncCall: Y_ID Y_LPAR CallParams Y_RPAR  */
#line 613 "lrparser.y"
      {
          (yyval.node) = make_call((yyvsp[-3].svalue), (yyvsp[-1].node));
      }
#line 2048 "lrparser.tab.c"
    break;

  case 78: /* CallParams: Exp  */
#line 620 "lrparser.y"
      {
          (yyval.node) = (yyvsp[0].node);
      }
#line 2056 "lrparser.tab.c"
    break;

  case 79: /* CallParams: CallParams Y_COMMA Exp  */
#line 624 "lrparser.y"
      {
          (yyval.node) = appendAst((yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2064 "lrparser.tab.c"
    break;

  case 80: /* MulExp: UnaryExp  */
#line 631 "lrparser.y"
      {
          (yyval.node) = (yyvsp[0].node);
      }
#line 2072 "lrparser.tab.c"
    break;

  case 81: /* MulExp: MulExp Y_MUL UnaryExp  */
#line 635 "lrparser.y"
      {
          (yyval.node) = make_binary(Y_MUL, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2080 "lrparser.tab.c"
    break;

  case 82: /* MulExp: MulExp Y_DIV UnaryExp  */
#line 639 "lrparser.y"
      {
          (yyval.node) = make_binary(Y_DIV, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2088 "lrparser.tab.c"
    break;

  case 83: /* MulExp: MulExp Y_MODULO UnaryExp  */
#line 643 "lrparser.y"
      {
          (yyval.node) = make_binary(Y_MODULO, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2096 "lrparser.tab.c"
    break;

  case 84: /* AddExp: MulExp  */
#line 650 "lrparser.y"
      {
          (yyval.node) = (yyvsp[0].node);
      }
#line 2104 "lrparser.tab.c"
    break;

  case 85: /* AddExp: AddExp Y_ADD MulExp  */
#line 654 "lrparser.y"
      {
          (yyval.node) = make_binary(Y_ADD, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2112 "lrparser.tab.c"
    break;

  case 86: /* AddExp: AddExp Y_SUB MulExp  */
#line 658 "lrparser.y"
      {
          (yyval.node) = make_binary(Y_SUB, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2120 "lrparser.tab.c"
    break;

  case 87: /* RelExp: AddExp  */
#line 665 "lrparser.y"
      {
          (yyval.node) = (yyvsp[0].node);
      }
#line 2128 "lrparser.tab.c"
    break;

  case 88: /* RelExp: RelExp Y_LESS AddExp  */
#line 669 "lrparser.y"
      {
          (yyval.node) = make_binary(Y_LESS, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2136 "lrparser.tab.c"
    break;

  case 89: /* RelExp: RelExp Y_LESSEQ AddExp  */
#line 673 "lrparser.y"
      {
          (yyval.node) = make_binary(Y_LESSEQ, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2144 "lrparser.tab.c"
    break;

  case 90: /* RelExp: RelExp Y_GREAT AddExp  */
#line 677 "lrparser.y"
      {
          (yyval.node) = make_binary(Y_GREAT, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2152 "lrparser.tab.c"
    break;

  case 91: /* RelExp: RelExp Y_GREATEQ AddExp  */
#line 681 "lrparser.y"
      {
          (yyval.node) = make_binary(Y_GREATEQ, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2160 "lrparser.tab.c"
    break;

  case 92: /* EqExp: RelExp  */
#line 688 "lrparser.y"
      {
          (yyval.node) = (yyvsp[0].node);
      }
#line 2168 "lrparser.tab.c"
    break;

  case 93: /* EqExp: EqExp Y_EQ RelExp  */
#line 692 "lrparser.y"
      {
          (yyval.node) = make_binary(Y_EQ, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2176 "lrparser.tab.c"
    break;

  case 94: /* EqExp: EqExp Y_NOTEQ RelExp  */
#line 696 "lrparser.y"
      {
          (yyval.node) = make_binary(Y_NOTEQ, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2184 "lrparser.tab.c"
    break;

  case 95: /* LAndExp: EqExp  */
#line 703 "lrparser.y"
      {
          (yyval.node) = (yyvsp[0].node);
      }
#line 2192 "lrparser.tab.c"
    break;

  case 96: /* LAndExp: LAndExp Y_AND EqExp  */
#line 707 "lrparser.y"
      {
          (yyval.node) = make_binary(Y_AND, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2200 "lrparser.tab.c"
    break;

  case 97: /* LOrExp: LAndExp  */
#line 714 "lrparser.y"
      {
          (yyval.node) = (yyvsp[0].node);
      }
#line 2208 "lrparser.tab.c"
    break;

  case 98: /* LOrExp: LOrExp Y_OR LAndExp  */
#line 718 "lrparser.y"
      {
          (yyval.node) = make_binary(Y_OR, (yyvsp[-2].node), (yyvsp[0].node));
      }
#line 2216 "lrparser.tab.c"
    break;

  case 99: /* Type: Y_INT  */
#line 724 "lrparser.y"
              { (yyval.ivalue) = Y_INT; }
#line 2222 "lrparser.tab.c"
    break;

  case 100: /* Type: Y_FLOAT  */
#line 725 "lrparser.y"
              { (yyval.ivalue) = Y_FLOAT; }
#line 2228 "lrparser.tab.c"
    break;


#line 2232 "lrparser.tab.c"

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

#line 728 "lrparser.y"


void yyerror(const char *s)
{
    fprintf(stderr, "Syntax error: %s\n", s);
}








