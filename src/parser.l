%{

#define YY_NO_UNPUT

#include <parser_tab.h>
#include <cstdint>
#include <cstdio>

uint64_t lines = 1;

static int lex(int token);
static int lex_num(int token);
static int lex_var(int token);

#ifdef LEX_DEBUG
#define pr_dbg(...) fprintf(stderr, __VA_ARGS__)
#else
#define pr_dbg(...)
#endif


#define lex_dbg(token) \
    ({ \
        pr_dbg("LEX: " #token "\n"); \
        lex(token); \
    })

#define lex_num_dbg(token) \
    ({ \
        pr_dbg("LEX: " #token "\n"); \
        lex_num(token); \
    })

#define lex_var_dbg(token) \
    ({ \
        pr_dbg("LEX: " #token "\n"); \
        lex_var(token); \
    })

%}

WHITE_C                         [ \t]

%x COMMENT

%%

\[                          {   pr_dbg("LEX: Begin Comment\n"); BEGIN(COMMENT);   }

<COMMENT>\]                 {   pr_dbg("LEX: End Comment\n"); BEGIN(INITIAL);     }

<COMMENT>\n                 {   pr_dbg("LEX: New line\n"); ++lines;               }

<COMMENT>[^\[]|[^\]]|[^\n]  {   pr_dbg("LEX: Inside Comment\n");                  }



{WHITE_C}+                  {   pr_dbg("LEX: White Character\n");                 }
^{WHITE_C}*\n               {   pr_dbg("LEX: New line\n"); ++lines;               }

\n                          {   pr_dbg("LEX: New line\n"); ++lines;               }

\;                          {   return lex_dbg(YY_SEMICOLON);                     }
\,                          {   return lex_dbg(YY_COMMA);                         }

\+                          {   return lex_dbg(YY_ADD);                           }
\-                          {   return lex_dbg(YY_SUB);                           }
\/                          {   return lex_dbg(YY_DIV);                           }
\%                          {   return lex_dbg(YY_MOD);                           }
\*                          {   return lex_dbg(YY_MUL);                           }

\:\=                        {   return lex_dbg(YY_ASSIGN);                        }

\=                          {   return lex_dbg(YY_EQ);                            }
\!\=                        {   return lex_dbg(YY_NE);                            }
\<                          {   return lex_dbg(YY_LT);                            }
\>                          {   return lex_dbg(YY_GT);                            }
\<\=                        {   return lex_dbg(YY_LE);                            }
\>\=                        {   return lex_dbg(YY_GE);                            }


DECLARE                     {   return lex_dbg(YY_DECLARE_VAR);                   }


"BEGIN"                     {   return lex_dbg(YY_BEGIN);                         }
END                         {   return lex_dbg(YY_END);                           }


READ                        {   return lex_dbg(YY_READ);                          }
WRITE                       {   return lex_dbg(YY_WRITE);                         }


FOR                         {   return lex_dbg(YY_FOR);                           }
FROM                        {   return lex_dbg(YY_FROM);                          }
TO                          {   return lex_dbg(YY_TO);                            }
DOWNTO                      {   return lex_dbg(YY_DOWNTO);                        }
ENDFOR                      {   return lex_dbg(YY_ENDFOR);                        }


WHILE                       {   return lex_dbg(YY_WHILE);                         }
DO                          {   return lex_dbg(YY_DO);                            }
ENDWHILE                    {   return lex_dbg(YY_ENDWHILE);                      }

REPEAT                      {   return lex_dbg(YY_REPEAT);                        }
UNTIL                       {   return lex_dbg(YY_UNTIL);                         }

IF                          {   return lex_dbg(YY_IF);                            }
THEN                        {   return lex_dbg(YY_THEN);                          }
ELSE                        {   return lex_dbg(YY_ELSE);                          }
ENDIF                       {   return lex_dbg(YY_ENDIF);                         }

\(                          {   return lex_dbg(YY_L_BRACKET);                     }
\)                          {   return lex_dbg(YY_R_BRACKET);                     }
\:                          {   return lex_dbg(YY_ARRAY_RANGE);                   }


[_a-z]+                     {   return lex_var_dbg(YY_VARIABLE);                  }

[0-9]+                      {   return lex_num_dbg(YY_NUM);                       }

.                           {   return lex_dbg(YY_ERROR);                         }

%%

static int lex(int token)
{
    yylval.ptoken.line = lines;
    yylval.ptoken.val = 0;
    yylval.ptoken.str = nullptr;

    return token;
}

static int lex_num(int token)
{
    yylval.ptoken.line = lines;
    yylval.ptoken.val = strtoull(yytext, nullptr, 10);
    yylval.ptoken.str = nullptr;

    return token;
}

static int lex_var(int token)
{
    yylval.ptoken.val = 0;
    yylval.ptoken.line = lines;
    yylval.ptoken.str = new std::string(yytext);

    return token;
}

int yywrap()
{
    return 1;
}