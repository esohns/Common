/*%require                          "3.4.1"*/
%require                          "3.0"
/* %file-prefix                      "" */
/*%language                         "c++"*/
%language                         "C"
%locations
%no-lines
/*%skeleton                         "glr.c"*/
%skeleton                         "lalr1.cc"
%verbose
/* %yacc */

%defines                          "m3u_parser.h"
%output                           "m3u_parser.cpp"

/*%define "parser_class_name"       "Bencoding_Parser"*/
/* *NOTE*: this is the namespace AND the (f)lex prefix */
/*%name-prefix                      "y"*/
/*%pure-parser*/
/*%token-table*/
%define parse.error               verbose
%debug

%code top {
#include "stdafx.h"

#include "m3u_parser.h"
}

/* %define location_type */
/* %define api.location.type         {} */
/* %define api.namespace             {yy} */
/* %define api.prefix                {yy} */
//%define api.pure                    true
/* *TODO*: implement a push parser */
/* %define api.push-pull             push */
/*%define api.token.constructor*/
%define api.token.prefix            {}
/*%define api.value.type            variant*/
/* %define api.value.union.name      YYSTYPE */
/* %define lr.default-reduction      most */
/* %define lr.default-reduction      accepting */
/* %define lr.keep-unreachable-state false */
/* %define lr.type                   lalr */

/*%define parse.assert              {true}*/
/*%define parse.error               verbose*/
/* %define parse.lac                 {full} */
/* %define parse.lac                 {none} */
/*%define parser_class_name         {BitTorrent_Bencoding_Parser}*/
/* *NOTE*: enabling debugging functionality implies inclusion of <iostream> (see
           below). This interferes with ACE (version 6.2.3), when compiled with
           support for traditional iostreams */
/*%define parse.trace               {true}*/

%code requires {
// *NOTE*: add double include protection, required for GNU Bison 2.4.2
// *TODO*: remove this ASAP
//#ifndef M3U_Parser_H
//#define M3U_Parser_H

#include "common_parser_m3u_iparser.h"

// *NOTE*: on current versions of bison, this needs to be inserted into the
//         header manually; apparently, there is no easy way to add the export
//         symbol to the declaration
#define YYDEBUG 1
/*extern int BitTorrent_Export yydebug;*/
#define YYERROR_VERBOSE 1
}

// calling conventions / parameter passing
%parse-param              { M3U_IParser_t* iparser }
%parse-param              { M3U_IScanner_t* iscanner }
// *NOTE*: cannot use %initial-action, as it is scoped
%lex-param                { M3U_IParser_t* iparser }
%lex-param                { yyscan_t iscanner->getR ().context }

%initial-action
{
  // initialize the location
  @$.initialize (NULL);
}

// symbols
%union
{
  ACE_INT32                     ival;
  std::string*                  sval;
  struct M3U_ExtInf_Element*    eeval;
  struct M3U_Media_Element*     emval;
  struct M3U_StreamInf_Element* esval;
  struct M3U_Playlist*          lval;
}
//%token <ACE_INT32>   INTEGER;
//%token <std::string> STRING;
//%token <M3U_Element> ELEMENT;
//%token EOF 0;

%code {
// *NOTE*: necessary only if %debug is set in the definition file (see: parser.y)
/*#if defined (YYDEBUG)
#include <iostream>
#endif*/
/*#include <regex>*/
#include <sstream>
#include <string>

// *WORKAROUND*
/*using namespace std;*/
// *IMPORTANT NOTE*: several ACE headers include ace/iosfwd.h, which introduces
//                   a problem in conjunction with the standard include headers
//                   when ACE_USES_OLD_IOSTREAMS is defined
//                   --> include the necessary headers manually (see above), and
//                       prevent ace/iosfwd.h from causing any harm
/*#define ACE_IOSFWD_H*/

#include "ace/Log_Msg.h"
#include "ace/OS.h"

#include "common_macros.h"

#include "m3u_scanner.h"
#include "common_parser_m3u_defines.h"
#include "common_parser_m3u_parser_driver.h"
#include "common_parser_m3u_tools.h"

// *TODO*: this shouldn't be necessary
#define yylex M3U_lex
//#define yylex iscanner->lex

//#define YYPRINT(file, type, value) yyprint (file, type, value)
}

// terminals
%token
               LF                   "lf"
               END_OF_ELEMENT       "element_end"
               END 0                "end"
;
%token <sval>  DATETIME             "date_time"
%token <sval>  KEY                  "key"
%token <ival>  LENGTH               "length"
%token <sval>  TITLE                "title"
%token <sval>  URL                  "URL"
%token <sval>  VALUE                "value"
%token <eeval> BEGIN_EXTINF         "begin_ext_inf"
%token <emval> BEGIN_MEDIA          "begin_media"
%token <esval> BEGIN_EXT_STREAM_INF "begin_ext_stream_inf"
%token <lval>  EXT3MU               "extm3u"

// non-terminals
%nterm  <ival> elements ext_x_key_values ext_x_key_value program_date_time
%nterm  <ival> ext_media_key_values ext_media_key_value
%nterm  <ival> ext_stream_inf_key_values ext_stream_inf_key_value
%nterm  <eval> element
%nterm  <ival> ext_stream_inf_rest_1 ext_stream_inf_rest_3 ext_stream_inf_rest_4
%nterm  <ival> ext_inf_rest_1 ext_inf_rest_2 ext_inf_rest_3 ext_inf_rest_4
%nterm  <ival> ext_media_rest_1 ext_media_rest_4
%nterm  <ival> playlist

/*%precedence element inf_rest_1 inf_rest_2 inf_rest_3 inf_rest_4
%precedence playlist elements*/

%code provides {
/*void BitTorrent_Export yysetdebug (int);
void yyerror (YYLTYPE*, M3U_IParser*, yyscan_t, const char*);
int yyparse (M3U_IParser*, yyscan_t);
void yyprint (FILE*, yytokentype, YYSTYPE);*/

// *NOTE*: add double include protection, required for GNU Bison 2.4.2
// *TODO*: remove this ASAP
//#endif // M3U_Parser_H
}

/*%printer    { ACE_OS::fprintf (yyoutput, ACE_TEXT (" %s"), $$->c_str ()); } <sval>
%printer    { ACE_OS::fprintf (yyoutput, ACE_TEXT (" %d"), $$); } <ival>
%destructor { delete $$; $$ = NULL; } <sval>
%destructor { $$ = 0; } <ival>*/
/*%destructor { $$ = 0; } <eval>
%destructor { $$ = 0; } <lval>*/
/* %destructor               { ACE_DEBUG ((LM_DEBUG,
                                        ACE_TEXT ("discarding tagless symbol...\n"))); } <> */
/*%printer    { yyoutput << $$; } <*>;*/
%printer    { debug_stream () << $$; } <ival>
%printer    { debug_stream () << *$$; } <sval>
/*%printer    { yyoutput << Test_I_M3U_Tools::ElementToString (*$$); } <eval>*/
%printer    { if ($$) debug_stream () << Common_Parser_M3U_Tools::ElementToString (*$$); } <eeval>
%printer    { if ($$) debug_stream () << Common_Parser_M3U_Tools::ElementToString (*$$); } <emval>
%printer    { if ($$) debug_stream () << Common_Parser_M3U_Tools::ElementToString (*$$); } <esval>
%printer    { if ($$) debug_stream () << Common_Parser_M3U_Tools::PlaylistToString (*$$); } <lval>

%%
%start            playlist;
playlist:         "extm3u" {
                    iparser->setP ($1);
                  } ext_x_key_values elements "lf" {
                    struct M3U_Playlist& playlist_r = iparser->current ();
                    struct M3U_Playlist* playlist_p = &playlist_r;
                    try {
                      iparser->record (playlist_p);
                    } catch (...) {
                      ACE_DEBUG ((LM_ERROR,
                                  ACE_TEXT ("caught exception in M3U_IParser::record(), continuing\n")));
                    }
                    YYACCEPT;
                  }
ext_x_key_values: ext_x_key_values ext_x_key_value
/*                |                    { }*/
                  | %empty             { }
ext_x_key_value:  "key" {
                    struct M3U_Playlist& playlist_r = iparser->current ();
                    playlist_r.key = *$1; } "value" {
                    struct M3U_Playlist& playlist_r = iparser->current ();
                    playlist_r.keyValues.push_back (std::make_pair (playlist_r.key, *$3));
                    playlist_r.key.clear ();
                  }

elements:         elements element
/*                  |                    { }*/
                  | %empty             { }
element:          program_date_time "begin_ext_inf" {
                    iparser->setP_2 ($2);
                    struct M3U_Playlist& playlist_r = iparser->current ();
                    if (!playlist_r.key.empty ())
                    {
                      struct M3U_ExtInf_Element& element_r = iparser->current_2 ();
                      element_r.keyValues.push_back (std::make_pair (ACE_TEXT_ALWAYS_CHAR (COMMON_PARSER_M3U_EXT_X_PROGRAM_DATE_TIME), playlist_r.key));
                      playlist_r.key.clear ();
                    } // end IF
                  } ext_inf_rest_1 { }
                  | "begin_media" {
                    iparser->setP_3 ($1);
                  } ext_media_rest_1 { }
                  | "begin_ext_stream_inf" {
                    iparser->setP_4 ($1);
                    struct M3U_Playlist& playlist_r = iparser->current ();
                    if (!playlist_r.key.empty ())
                    {
                      struct M3U_StreamInf_Element& element_r = iparser->current_4 ();
                      element_r.keyValues.push_back (std::make_pair (ACE_TEXT_ALWAYS_CHAR (COMMON_PARSER_M3U_EXT_X_PROGRAM_DATE_TIME), playlist_r.key));
                      playlist_r.key.clear ();
                    } // end IF
                  } ext_stream_inf_rest_1 { }
program_date_time: "date_time" {
                    struct M3U_Playlist& playlist_r = iparser->current ();
                    playlist_r.key = *$1; }
/*                  |                    { }*/
                   | %empty             { }

ext_inf_rest_1:   "length" {
                    struct M3U_ExtInf_Element& element_r = iparser->current_2 ();
                    element_r.Length = $1; } ext_inf_rest_2 { }
ext_inf_rest_2:   "title" {
                    struct M3U_ExtInf_Element& element_r = iparser->current_2 ();
                    element_r.Title = *$1; } ext_inf_rest_3 { }
                  | ext_inf_rest_3 { }
ext_inf_rest_3:   "URL" {
                    struct M3U_ExtInf_Element& element_r = iparser->current_2 ();
                    element_r.URL = *$1; } ext_inf_rest_4 { }
ext_inf_rest_4:   "element_end" {
                    struct M3U_Playlist& playlist_r = iparser->current ();
                    struct M3U_ExtInf_Element& element_r = iparser->current_2 ();
                    playlist_r.ext_inf_elements.push_back (element_r); }

ext_media_rest_1: ext_media_key_values ext_media_rest_4 { }
ext_media_key_values: ext_media_key_values ext_media_key_value
             /*            |                    { }*/
                      | %empty             { }
ext_media_key_value:   "key" {
                    struct M3U_Media_Element& element_r = iparser->current_3 ();
                    element_r.key = *$1; } "value" {
                    struct M3U_Media_Element& element_r = iparser->current_3 ();
                    element_r.keyValues.push_back (std::make_pair (element_r.key, *$3)); element_r.key.clear (); }
ext_media_rest_4:   "element_end" {
  struct M3U_Playlist& playlist_r = iparser->current ();
  struct M3U_Media_Element& element_r = iparser->current_3 ();
  playlist_r.media_elements.push_back (element_r); }

ext_stream_inf_rest_1: ext_stream_inf_key_values ext_stream_inf_rest_3 { }
ext_stream_inf_key_values: ext_stream_inf_key_values ext_stream_inf_key_value
/*            |                    { }*/
            | %empty             { }
ext_stream_inf_key_value:   "key" {
  struct M3U_StreamInf_Element& element_r = iparser->current_4 ();
  element_r.key = *$1; } "value" {
  struct M3U_StreamInf_Element& element_r = iparser->current_4 ();
  element_r.keyValues.push_back (std::make_pair (element_r.key, *$3)); element_r.key.clear (); }
ext_stream_inf_rest_3:   "URL" {
  struct M3U_StreamInf_Element& element_r = iparser->current_4 ();
  element_r.URL = *$1; } ext_stream_inf_rest_4 { }
ext_stream_inf_rest_4:   "element_end" {
  struct M3U_Playlist& playlist_r = iparser->current ();
  struct M3U_StreamInf_Element& element_r = iparser->current_4 ();
  playlist_r.stream_inf_elements.push_back (element_r); }
%%

void
yy::parser::error (const location_type& location_in,
                   const std::string& message_in)
{
  COMMON_TRACE (ACE_TEXT ("yy::parser::error"));

  // sanity check(s)
  ACE_ASSERT (iparser);

  try {
    iparser->error (location_in, message_in);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Bencoding_IParser_t::error(), continuing\n")));
  }
}

void
yyerror (yy::location* location_in,
         M3U_IParser_t* iparser_in,
         yyscan_t context_in,
         const char* message_in)
{
  COMMON_TRACE (ACE_TEXT ("::yyerror"));

//  ACE_UNUSED_ARG (location_in);
  ACE_UNUSED_ARG (context_in);

  // sanity check(s)
  ACE_ASSERT (iparser_in);

  try {
    iparser_in->error (*location_in, message_in);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Bencoding_IParser::error(), continuing\n")));
  }
}

void
yyprint (FILE* file_in,
         yy::parser::token::yytokentype type_in,
         yy::parser::semantic_type value_in)
{
  COMMON_TRACE (ACE_TEXT ("::yyprint"));

  int result = -1;

  std::string format_string;
  switch (type_in)
  {
    case yy::parser::token::TITLE:
    case yy::parser::token::URL:
    {
      format_string = ACE_TEXT_ALWAYS_CHAR (" %s");
      break;
    }
    case yy::parser::token::LENGTH:
    case yy::parser::token::END:
    {
      format_string = ACE_TEXT_ALWAYS_CHAR (" %d");
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown token type (was: %d), returning\n"),
                  type_in));
      return;
    }
  } // end SWITCH

  result = ACE_OS::fprintf (file_in,
                            ACE_TEXT (format_string.c_str ()),
                            value_in);
  if (result < 0)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fprintf(): \"%m\", returning\n")));
}
