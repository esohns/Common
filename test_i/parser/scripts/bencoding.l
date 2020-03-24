%top{
//  #include "ace/Synch.h"
  #include "common_iparser.h"

  #include "bencoding_parser.h"

#if defined (Bencoding_IN_HEADER)
class Bencoding_IParser;
#else
#define YY_STRUCT_YY_BUFFER_STATE
  #include "bencoding_scanner.h"

  #include "bencoding_iparser.h"
#undef YY_STRUCT_YY_BUFFER_STATE
#endif

#define YY_DECL                                     \
int                                                 \
Bencoding_lex (yy::parser::semantic_type* yylval,   \
               yy::parser::location_type* location, \
               Bencoding_IParser* parser,           \
               yyscan_t yyscanner)
// ... and declare it for the parser's sake
extern YY_DECL;

/*#define YY_EXTRA_TYPE*/
}

%{
  // *WORKAROUND*
//  #include <iostream>
  #include <sstream>
  //using namespace std;
  //// *IMPORTANT NOTE*: several ACE headers inclue ace/iosfwd.h, which introduces
  ////                   a problem in conjunction with the standard include headers
  ////                   when ACE_USES_OLD_IOSTREAMS is defined
  ////                   --> include the necessary headers manually (see above), and
  ////                       prevent ace/iosfwd.h from causing any harm
  //#define ACE_IOSFWD_H

  #include "ace/Synch.h"
  #include "bencoding_scanner.h"
  #include "bencoding_parser.h"
  #include "bencoding_parser_driver.h"

  // the original yyterminate() macro returns int. Since this uses Bison 3
  // variants as tokens, redefine it to change type to `Parser::semantic_type`
  #define yyterminate() return yy::parser::token::END;

  // this tracks the current scanner location. Action is called when length of
  // the token is known
  #define YY_USER_ACTION location->columns (yyleng);
%}

%option backup
%option batch
%option never-interactive
%option stack
%option pointer

%option nodefault
%option nomain
%option nostdinit
%option nounput
%option noyywrap

%option 8bit
%option align
/* *TODO*: find out why 'read' does not compile (on Linux, flex 2.5.39) */
%option read
%option full
%option ecs
%option reentrant

%option debug
%option perf-report
%option verbose
%option warn
%option yylineno

/*%option ansi-definitions ansi-prototypes*/
/*%option c++*/
%option header-file="bencoding_scanner.h"
%option outfile="bencoding_scanner.cpp"
%option prefix="Bencoding_"
/*%option yyclass="Common_BencodingScanner"*/

/* *NOTE*: for protcol specification, see:
           - http://bittorrent.org/beps/bep_0003.html
           - https://wiki.theory.org/BitTorrentSpecification */

OCTET                             [\x00-\xFF]
DIGIT                             [[:digit:]]

/* this is called 'bencoding' in BitTorrent lingo */
STRING                            ({DIGIT}+:{OCTET}*)
INTEGER                           (i{DIGIT}+e)
LIST                              (l({INTEGER}|{STRING})*e)
DICTIONARY                        (d({STRING}({INTEGER}|{STRING}|{LIST}|{DICTIONARY}))*e)

%s state_string
%s state_integer
%s state_list
%s state_dictionary_key
%s state_dictionary_value

%{
/* handle locations */
/*#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno; \
                       yylloc.first_column = yycolumn; yylloc.last_column = yycolumn+yyleng-1; \
                       yycolumn += yyleng; */
//#define YY_USER_ACTION yylloc->columns (yyleng);
%}

%% /* end of definitions */

%{
  /* code to place at the beginning of yylex() */

  // reset location
  location->step ();

  unsigned int      string_length = 0;
  bool              in_structure = false;
  bool              string_is_key = false;
  std::stringstream converter;

  // sanity check(s)
  ACE_ASSERT (parser);
  ACE_Message_Block* message_block_p = parser->buffer ();
  ACE_ASSERT (message_block_p);
  ACE_ASSERT (yyscanner);

//  location.columns (yyleng);
%}

<INITIAL>{
{DIGIT}{1}             { yyless (0);
                         BEGIN(state_string); }
"i"                    { yyless (0);
                         BEGIN(state_integer); }
"l"                    { ACE_ASSERT (yyleng == 1);
                         parser->offset (1);
                         BEGIN(state_list);
                         ACE_NEW_NORETURN (yylval->lval,
                                           Bencoding_List_t);
                         ACE_ASSERT (yylval->lval);
                         return yy::parser::token::LIST; }
"d"                    { ACE_ASSERT (yyleng == 1);
                         parser->offset (1);
                         BEGIN(state_dictionary_key);
                         ACE_NEW_NORETURN (yylval->dval,
                                           Bencoding_Dictionary_t);
                         ACE_ASSERT (yylval->dval);
                         return yy::parser::token::DICTIONARY; }
} // end <INITIAL>
<state_string>{
{DIGIT}+               {
                         parser->offset (yyleng);
                         converter.str (ACE_TEXT_ALWAYS_CHAR (""));
                         converter.clear ();
                         converter << yytext;
                         converter >> string_length;
                         ACE_NEW_NORETURN (yylval->sval,
                                           std::string);
                         ACE_ASSERT (yylval->sval); }
":"                    { ACE_ASSERT (yyleng == 1);
                         parser->offset (1);
                         if (!string_length)
                         { // --> found an empty string
                           ACE_ASSERT (yylval->sval);
                           if (string_is_key)
                           {
                             string_is_key = false;
                             BEGIN(state_dictionary_value);
                           } // end IF
                           else if (in_structure)
                             yy_pop_state (yyscanner); // return to structure
                           return yy::parser::token::STRING;
                         } // END IF
                       }
{OCTET}{1}             { ACE_ASSERT (string_length != 0);
                         ACE_ASSERT (yylval->sval && yylval->sval->empty ());
                         parser->offset (string_length);
                         yylval->sval->push_back (yytext[0]);
                         for (unsigned int i = 0; i < (string_length - 1); ++i)
                           yylval->sval->push_back (static_cast<char> (yyinput (yyscanner)));
                         if (string_is_key)
                         {
                           string_is_key = false;
                           BEGIN(state_dictionary_value);
                         } // end IF
                         else if (in_structure)
                           yy_pop_state (yyscanner); // return to structure
                         string_length = 0;
                         return yy::parser::token::STRING; }
} // end <state_string>
<state_integer>{
"e"                    { ACE_ASSERT (yyleng == 1);
                         parser->offset (1);
                         if (in_structure)
                           yy_pop_state (yyscanner); // return to structure
                         return yy::parser::token::INTEGER; }
{DIGIT}+               {
                         parser->offset (yyleng);
                         converter.str (ACE_TEXT_ALWAYS_CHAR (""));
                         converter.clear ();
                         converter << yytext;
                         converter >> yylval->ival; }
"i"                    { ACE_ASSERT (yyleng == 1);
                         parser->offset (1); }
} // end <state_integer>
<state_list>{
"e"                    { ACE_ASSERT (yyleng == 1);
                         parser->offset (1);
                         if (in_structure)
                           yy_pop_state (yyscanner); } // return to structure
{DIGIT}{1}             { yyless (0);
                         in_structure = true;
                         yy_push_state (state_string, yyscanner); }
"i"                    { yyless (0);
                         in_structure = true;
                         yy_push_state (state_integer, yyscanner); }
"l"                    { yyless (0);
                         in_structure = true;
                         yy_push_state (state_list, yyscanner);
                         ACE_NEW_NORETURN (yylval->lval,
                                           Bencoding_List_t);
                         ACE_ASSERT (yylval->lval);
                         return yy::parser::token::LIST; }
"d"                    { yyless (0);
                         in_structure = true;
                         yy_push_state (state_dictionary_key, yyscanner);
                         ACE_NEW_NORETURN (yylval->dval,
                                           Bencoding_Dictionary_t);
                         ACE_ASSERT (yylval->dval);
                         return yy::parser::token::DICTIONARY; }
} // end <state_list>
<state_dictionary_key>{
"e"                    { ACE_ASSERT (yyleng == 1);
                         parser->offset (1);
                         if (in_structure)
                           yy_pop_state (yyscanner); } // return to structure
{DIGIT}{1}             { yyless (0);
                         string_is_key = true;
                         BEGIN(state_string); }
} // end <state_dictionary_key>
<state_dictionary_value>{
{DIGIT}{1}             { yyless (0);
                         in_structure = true;
                         yy_push_state (state_string, yyscanner); }
"i"                    { yyless (0);
                         in_structure = true;
                         yy_push_state (state_integer, yyscanner); }
"l"                    { yyless (0);
                         in_structure = true;
                         yy_push_state (state_list, yyscanner);
                         ACE_NEW_NORETURN (yylval->lval,
                                           Bencoding_List_t);
                         ACE_ASSERT (yylval->lval);
                         return yy::parser::token::LIST; }
"d"                    { yyless (0);
                         in_structure = true;
                         yy_push_state (state_dictionary_key, yyscanner);
                         ACE_NEW_NORETURN (yylval->dval,
                                           Bencoding_Dictionary_t);
                         ACE_ASSERT (yylval->dval);
                         return yy::parser::token::DICTIONARY; }
} // end <state_dictionary_value>
<<EOF>>                { return yy::parser::token::END; }
<*>{OCTET}             { /* *TODO*: use (?s:.) ? */
                         if (!parser->isBlocking ())
                           return yy::parser::token::END_OF_FRAGMENT;
                         // wait for more data fragment(s)
                         if (!parser->switchBuffer ())
                         { // *NOTE*: most probable reason: connection
                           //         has been closed --> session end
                           ACE_DEBUG ((LM_DEBUG,
                                       ACE_TEXT ("failed to Common_IParser::switchBuffer(), returning\n")));
                           yyterminate(); // not enough data, cannot proceed
                         } // end IF
                         yyless (0); }
%% /* end of rules */
