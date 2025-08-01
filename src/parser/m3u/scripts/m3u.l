%top{
  #include "common_iparser.h"

  #include "m3u_parser.h"

#if defined (M3U_IN_HEADER)
class M3U_IParser;
#else
#define YY_STRUCT_YY_BUFFER_STATE
  #include "m3u_scanner.h"

  #include "common_parser_m3u_iparser.h"
#undef YY_STRUCT_YY_BUFFER_STATE
#endif

#define YY_DECL                                     \
int                                                 \
M3U_lex (yy::parser::semantic_type* yylval,   \
         yy::parser::location_type* location, \
         M3U_IParser_t* parser,               \
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

  #include "m3u_scanner.h"
  #include "m3u_parser.h"
  #include "common_parser_m3u_parser_driver.h"

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
%option pointer
%option stack

/* %option nodefault */
%option nomain
%option nostdinit
%option nounput

%option 8bit
%option align
/* *TODO*: find out why 'read' does not compile (on Linux, flex 2.5.39) */
%option read
/* %option full */
%option ecs
%option reentrant

%option debug
%option perf-report
%option verbose
%option warn
/* %option yylineno */

/*%option ansi-definitions ansi-prototypes*/
/*%option c++*/
%option header-file="m3u_scanner.h"
%option outfile="m3u_scanner.cpp"
%option prefix="M3U_"
/*%option yyclass="Common_M3UScanner"*/

/* *NOTE*: for protcol specification, see e.g.:
           - https://en.wikipedia.org/wiki/M3U */

DIGIT                             [[:digit:]]
OCTET                             [\x00-\xFF]
TO_COMMA                          [^,]
TO_QUOTATION                      [^\"]
TO_END_OF_LINE                    [^\x0A]
TO_EQUALS                         [^=]
LF                                \x0A|\x0D\x0A

%s state_comment
%s state_no_value
%s state_value
%s state_date_time
%s state_ext_inf
%s state_ext_inf_title
%s state_ext_stream_inf_key_value
%s state_ext_stream_inf_next_value_comma
%s state_ext_stream_inf_next_value_quotation
%s state_ext_stream_inf_is_last_value
%s state_media_key_value
%s state_media_next_value_comma
%s state_media_next_value_quotation
%s state_media_is_last_value
%s state_url_1 state_url_2

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

  std::stringstream converter;

  // sanity check(s)
  ACE_ASSERT (parser);
  ACE_Message_Block* message_block_p = parser->buffer ();
  ACE_ASSERT (message_block_p);
  ACE_ASSERT (yyscanner);

//  location.columns (yyleng);
%}

<INITIAL>{
"##"                              { parser->offset (yyleng);
                                    BEGIN (state_comment); }
"#EXTM3U"                         { parser->offset (yyleng);
                                    ACE_NEW_NORETURN (yylval->lval,
                                                      struct M3U_Playlist);
                                    ACE_ASSERT (yylval->lval);
                                    return yy::parser::token::EXT3MU; }
"#EXT-X-INDEPENDENT-SEGMENTS"     { parser->offset (yyleng);
                                    ACE_NEW_NORETURN (yylval->sval,
                                                     std::string);
                                    ACE_ASSERT (yylval->sval);
                                    *(yylval->sval) = ACE_TEXT_ALWAYS_CHAR ("INDEPENDENT-SEGMENTS");
                                    BEGIN (state_no_value);
                                    return yy::parser::token::KEY; }
"#EXT-X-TARGETDURATION:"          { parser->offset (yyleng);
                                    ACE_NEW_NORETURN (yylval->sval,
                                                     std::string);
                                    ACE_ASSERT (yylval->sval);
                                    *(yylval->sval) = ACE_TEXT_ALWAYS_CHAR ("TARGETDURATION");
                                    BEGIN (state_value);
                                    return yy::parser::token::KEY; }
"#EXT-X-ALLOW-CACHE:"             { parser->offset (yyleng);
                                    ACE_NEW_NORETURN (yylval->sval,
                                                     std::string);
                                    ACE_ASSERT (yylval->sval);
                                    *(yylval->sval) = ACE_TEXT_ALWAYS_CHAR ("ALLOW-CACHE");
                                    BEGIN (state_value);
                                    return yy::parser::token::KEY; }
"#EXT-X-VERSION:"                 { parser->offset (yyleng);
                                    ACE_NEW_NORETURN (yylval->sval,
                                                     std::string);
                                    ACE_ASSERT (yylval->sval);
                                    *(yylval->sval) = ACE_TEXT_ALWAYS_CHAR ("VERSION");
                                    BEGIN (state_value);
                                    return yy::parser::token::KEY; }
"#EXT-X-MEDIA-SEQUENCE:"          { parser->offset (yyleng);
                                    ACE_NEW_NORETURN (yylval->sval,
                                                     std::string);
                                    ACE_ASSERT (yylval->sval);
                                    *(yylval->sval) = ACE_TEXT_ALWAYS_CHAR ("MEDIA-SEQUENCE");
                                    BEGIN (state_value);
                                    return yy::parser::token::KEY; }
"#EXT-X-MEDIA:"                   { parser->offset (yyleng);
                                    BEGIN (state_media_key_value);
                                    ACE_NEW_NORETURN (yylval->emval,
                                                      struct M3U_Media_Element);
                                    ACE_ASSERT (yylval->emval);
                                    return yy::parser::token::BEGIN_MEDIA; }
"#EXT-X-DISCONTINUITY"            { parser->offset (yyleng);
                                    ACE_NEW_NORETURN (yylval->sval,
                                                     std::string);
                                    ACE_ASSERT (yylval->sval);
                                    *(yylval->sval) = ACE_TEXT_ALWAYS_CHAR ("DISCONTINUITY");
                                    BEGIN (state_no_value);
                                    return yy::parser::token::KEY; }
"#EXT-X-DISCONTINUITY-SEQUENCE:"  { parser->offset (yyleng);
                                    ACE_NEW_NORETURN (yylval->sval,
                                                     std::string);
                                    ACE_ASSERT (yylval->sval);
                                    *(yylval->sval) = ACE_TEXT_ALWAYS_CHAR ("DISCONTINUITY-SEQUENCE");
                                    BEGIN (state_value);
                                    return yy::parser::token::KEY; }
"#EXT-X-PROGRAM-DATE-TIME:"       { parser->offset (yyleng);
                                    BEGIN (state_date_time); }
"#EXT-X-STREAM-INF:"              { parser->offset (yyleng);
                                    BEGIN (state_ext_stream_inf_key_value);
                                    ACE_NEW_NORETURN (yylval->esval,
                                                      struct M3U_StreamInf_Element);
                                    ACE_ASSERT (yylval->esval);
                                    return yy::parser::token::BEGIN_EXT_STREAM_INF; }
"#EXTINF:"                        { parser->offset (yyleng);
                                    BEGIN (state_ext_inf);
                                    ACE_NEW_NORETURN (yylval->eeval,
                                                      struct M3U_ExtInf_Element);
                                    ACE_ASSERT (yylval->eeval);
                                    return yy::parser::token::BEGIN_EXTINF; }
{LF}                              { parser->offset (yyleng);
                                    if (parser->hasFinished ()) // ignore spurious LFs
                                      return yy::parser::token::LF;
                                  }
} // end <INITIAL>

<state_comment>{
{TO_END_OF_LINE}+/{LF}            { parser->offset (yyleng); }
{LF}                              { parser->offset (yyleng);
                                    BEGIN (INITIAL); }
} // end <state_comment>

<state_no_value>{
{LF}                              { parser->offset (yyleng);
                                    ACE_NEW_NORETURN (yylval->sval,
                                                     std::string);
                                    ACE_ASSERT (yylval->sval);
                                    *(yylval->sval) = ACE_TEXT_ALWAYS_CHAR ("");
                                    BEGIN (INITIAL);
                                    return yy::parser::token::VALUE; }
} // end <state_no_value>
<state_value>{
{TO_END_OF_LINE}+/{LF}            { parser->offset (yyleng);
                                    ACE_NEW_NORETURN (yylval->sval,
                                                     std::string);
                                    ACE_ASSERT (yylval->sval);
                                    *(yylval->sval) = yytext;
                                    return yy::parser::token::VALUE; }
{LF}                              { parser->offset (yyleng);
                                    BEGIN (INITIAL); }
} // end <state_value>

<state_date_time>{
{TO_END_OF_LINE}+/{LF}            { parser->offset (yyleng);
                                    ACE_NEW_NORETURN (yylval->sval,
                                                      std::string);
                                    ACE_ASSERT (yylval->sval);
                                    *(yylval->sval) = yytext;
                                    return yy::parser::token::DATETIME; }
{LF}                              { parser->offset (yyleng);
                                    if (parser->pop_state ())
                                      yy_pop_state (yyscanner);
                                    else
                                      BEGIN (INITIAL); }
} // end <state_date_time>

<state_ext_inf>{
{TO_COMMA}+/,                     { parser->offset (yyleng);
                                    converter.str (ACE_TEXT_ALWAYS_CHAR (""));
                                    converter.clear ();
                                    converter << yytext;
                                    converter >> yylval->ival;
                                    return yy::parser::token::LENGTH; }
","                               { parser->offset (yyleng);
                                    BEGIN (state_ext_inf_title); }
} // end <state_ext_inf>

<state_ext_inf_title>{
{TO_END_OF_LINE}+/{LF}            { parser->offset (yyleng);
                                    ACE_NEW_NORETURN (yylval->sval,
                                                      std::string);
                                    ACE_ASSERT (yylval->sval);
                                    *(yylval->sval) = yytext;
                                    return yy::parser::token::TITLE; }
{LF}                              { parser->offset (yyleng);
                                    BEGIN (state_url_1); }
} // end <state_key_value>

<state_ext_stream_inf_key_value>{
{TO_EQUALS}+/=                    { parser->offset (yyleng);
                                    ACE_NEW_NORETURN (yylval->sval,
                                                     std::string);
                                    ACE_ASSERT (yylval->sval);
                                    *(yylval->sval) = yytext;
                                    return yy::parser::token::KEY; }
"="                               { parser->offset (yyleng);
                                    BEGIN (state_ext_stream_inf_is_last_value); }
} // end <state_ext_stream_inf_key_value>

<state_ext_stream_inf_is_last_value>{
{TO_END_OF_LINE}+/{LF}            {
                                    std::string test_string = yytext;
                                    // *TODO*: actually, need to test whether ','
                                    // does NOT appear between '"'s
                                    if (test_string.find_first_of (',') == std::string::npos)
                                    { parser->offset (yyleng);
                                      ACE_NEW_NORETURN (yylval->sval,
                                                        std::string);
                                      ACE_ASSERT (yylval->sval);
                                      *(yylval->sval) = yytext;
                                      return yy::parser::token::VALUE;
                                    } // end IF
                                    if (test_string[0] == '"')
                                    { parser->offset (1);
                                      BEGIN (state_ext_stream_inf_next_value_quotation);
                                      yyless (1);
                                    } // end IF
                                    else
                                    {
                                      BEGIN (state_ext_stream_inf_next_value_comma);
                                      yyless (0);
                                    } // end IF
                                  }
{LF}                              { parser->offset (yyleng);
                                    BEGIN (state_url_1); }
} // end <state_ext_stream_inf_is_last_value>

<state_ext_stream_inf_next_value_comma>{
{TO_COMMA}+/,                     { parser->offset (yyleng);
                                    ACE_NEW_NORETURN (yylval->sval,
                                                      std::string);
                                    ACE_ASSERT (yylval->sval);
                                    *(yylval->sval) = yytext;
                                    return yy::parser::token::VALUE; }
","                                { parser->offset (yyleng);
                                     BEGIN (state_ext_stream_inf_key_value); }
} // end <state_inf_next_value_comma>

<state_ext_stream_inf_next_value_quotation>{
{TO_QUOTATION}+/\"                { parser->offset (yyleng);
                                    ACE_NEW_NORETURN (yylval->sval,
                                                      std::string);
                                    ACE_ASSERT (yylval->sval);
                                    *(yylval->sval) = yytext;
                                    return yy::parser::token::VALUE; }
"\","                              { parser->offset (yyleng);
                                     BEGIN (state_ext_stream_inf_key_value); }
} // end <state_inf_next_value_quotation>

<state_media_key_value>{
{TO_EQUALS}+/=                    { parser->offset (yyleng);
                                    ACE_NEW_NORETURN (yylval->sval,
                                                      std::string);
                                    ACE_ASSERT (yylval->sval);
                                    *(yylval->sval) = yytext;
                                    return yy::parser::token::KEY; }
"="                               { parser->offset (yyleng);
                                    BEGIN (state_media_is_last_value); }
} // end <state_media_key_value>

<state_media_is_last_value>{
{TO_END_OF_LINE}+/{LF}            { std::string test_string = yytext;
                                    // *TODO*: actually, need to test whether ','
                                    // does NOT appear between '"'s
                                    if (test_string.find_first_of (',') == std::string::npos)
                                    { parser->offset (yyleng);
                                      ACE_NEW_NORETURN (yylval->sval,
                                                       std::string);
                                      ACE_ASSERT (yylval->sval);
                                      *(yylval->sval) = yytext;
                                      return yy::parser::token::VALUE;
                                    } // end IF
                                    if (test_string[0] == '"')
                                    { parser->offset (1);
                                      BEGIN (state_media_next_value_quotation);
                                      yyless (1);
                                    } // end IF
                                    else
                                    {
                                      BEGIN (state_media_next_value_comma);
                                      yyless (0);
                                    } // end IF
                                  }
{LF}                              { parser->offset (yyleng);
                                    BEGIN (INITIAL);
                                    return yy::parser::token::END_OF_ELEMENT; }
} // end <state_media_is_last_value>

<state_media_next_value_comma>{
{TO_COMMA}+/,                     { parser->offset (yyleng);
                                    ACE_NEW_NORETURN (yylval->sval,
                                                      std::string);
                                    ACE_ASSERT (yylval->sval);
                                    *(yylval->sval) = yytext;
                                    return yy::parser::token::VALUE; }
","                               { parser->offset (yyleng);
                                    BEGIN (state_media_key_value); }
} // end <state_media_next_value_comma>

<state_media_next_value_quotation>{
{TO_QUOTATION}+/\"                { parser->offset (yyleng);
                                    ACE_NEW_NORETURN (yylval->sval,
                                                      std::string);
                                    ACE_ASSERT (yylval->sval);
                                    *(yylval->sval) = yytext;
                                    return yy::parser::token::VALUE; }
"\","                             { parser->offset (yyleng);
                                    BEGIN (state_media_key_value); }
} // end <state_media_next_value_quotation>

<state_url_1>{
"#EXT-X-PROGRAM-DATE-TIME:"       { parser->offset (yyleng);
                                    parser->pop_next ();
                                    yy_push_state (state_date_time, yyscanner); }
{TO_END_OF_LINE}{1}               { yyless (0);
                                    BEGIN (state_url_2); }
} // end <state_url_1>

<state_url_2>{
{TO_END_OF_LINE}+/{LF}            { parser->offset (yyleng);
                                    ACE_NEW_NORETURN (yylval->sval,
                                                     std::string);
                                    ACE_ASSERT (yylval->sval);
                                    *(yylval->sval) = yytext;
                                    return yy::parser::token::URL; }
{LF}                              { parser->offset (yyleng);
                                    BEGIN (INITIAL);
                                    return yy::parser::token::END_OF_ELEMENT; }
} // end <state_url_2>

<<EOF>>                           { return yy::parser::token::END; }
<*>{OCTET}                        { /* *TODO*: use (?s:.) ? */
                                    if (!parser->isBlocking ())
                                      yyterminate ();
                                    // wait for more data fragment(s)
                                    if (!parser->switchBuffer ())
                                    { // *NOTE*: most probable reason: connection
                                      //         has been closed --> session end
                                      ACE_DEBUG ((LM_DEBUG,
                                                  ACE_TEXT ("failed to Common_IParser::switchBuffer(), returning\n")));
                                      yyterminate (); // not enough data, cannot proceed
                                    } // end IF
                                    yyless (0); }
%% /* end of rules */

#include "common_macros.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
int
M3U_wrap (yyscan_t yyscanner)
{
  COMMON_TRACE (ACE_TEXT ("::M3U_wrap"));

  // sanity check(s)
//  struct yyguts_t* yyg = static_cast<struct yyguts_t*> (yyscanner);
  Common_Parser_M3U_ParserDriver* driver =
    static_cast<Common_Parser_M3U_ParserDriver*> (M3U_get_extra (yyscanner));
  ACE_ASSERT (driver);
//  if (driver->hasFinished ())
//    return 1; // done
//  ACE_ASSERT (driver->fragment_);

  // *NOTE*: there is more data
  // 1. switch buffers
  // 2. continue scanning

  // step1
  if (!driver->switchBuffer ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_ParserBase_T::switchBuffer(), aborting\n")));
    return 1;
  } // end IF

  // step2
  return 0;
}
#ifdef __cplusplus
}
#endif /* __cplusplus */
