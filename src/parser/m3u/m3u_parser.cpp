// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.

// "%code top" blocks.

#include "stdafx.h"

#include "m3u_parser.h"





#include "m3u_parser.h"


// Unqualified %code blocks.

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



#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif


// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (false)
# endif


// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yy_stack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YY_USE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

namespace yy {

  /// Build a parser object.
  parser::parser (M3U_IParser_t* iparser_yyarg, M3U_IScanner_t* iscanner_yyarg)
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      iparser (iparser_yyarg),
      iscanner (iscanner_yyarg)
  {}

  parser::~parser ()
  {}

  parser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------.
  | symbol.  |
  `---------*/

  // basic_symbol.
  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value (that.value)
    , location (that.location)
  {}


  /// Constructor for valueless symbols.
  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, YY_MOVE_REF (location_type) l)
    : Base (t)
    , value ()
    , location (l)
  {}

  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, YY_RVREF (value_type) v, YY_RVREF (location_type) l)
    : Base (t)
    , value (YY_MOVE (v))
    , location (YY_MOVE (l))
  {}


  template <typename Base>
  parser::symbol_kind_type
  parser::basic_symbol<Base>::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


  template <typename Base>
  bool
  parser::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return this->kind () == symbol_kind::S_YYEMPTY;
  }

  template <typename Base>
  void
  parser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    value = YY_MOVE (s.value);
    location = YY_MOVE (s.location);
  }

  // by_kind.
  parser::by_kind::by_kind () YY_NOEXCEPT
    : kind_ (symbol_kind::S_YYEMPTY)
  {}

#if 201103L <= YY_CPLUSPLUS
  parser::by_kind::by_kind (by_kind&& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {
    that.clear ();
  }
#endif

  parser::by_kind::by_kind (const by_kind& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {}

  parser::by_kind::by_kind (token_kind_type t) YY_NOEXCEPT
    : kind_ (yytranslate_ (t))
  {}



  void
  parser::by_kind::clear () YY_NOEXCEPT
  {
    kind_ = symbol_kind::S_YYEMPTY;
  }

  void
  parser::by_kind::move (by_kind& that)
  {
    kind_ = that.kind_;
    that.clear ();
  }

  parser::symbol_kind_type
  parser::by_kind::kind () const YY_NOEXCEPT
  {
    return kind_;
  }


  parser::symbol_kind_type
  parser::by_kind::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }



  // by_state.
  parser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  parser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  parser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  parser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  parser::symbol_kind_type
  parser::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  parser::stack_symbol_type::stack_symbol_type ()
  {}

  parser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.value), YY_MOVE (that.location))
  {
#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  parser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.value), YY_MOVE (that.location))
  {
    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    location = that.location;
    return *this;
  }

  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    location = that.location;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);

    // User destructor.
    YY_USE (yysym.kind ());
  }

#if YYDEBUG
  template <typename Base>
  void
  parser::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YY_USE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " ("
            << yysym.location << ": ";
        switch (yykind)
    {
      case symbol_kind::S_DATETIME: // "date_time"
                    { debug_stream () << *(yysym.value.sval); }
        break;

      case symbol_kind::S_KEY: // "key"
                    { debug_stream () << *(yysym.value.sval); }
        break;

      case symbol_kind::S_LENGTH: // "length"
                    { debug_stream () << (yysym.value.ival); }
        break;

      case symbol_kind::S_TITLE: // "title"
                    { debug_stream () << *(yysym.value.sval); }
        break;

      case symbol_kind::S_URL: // "URL"
                    { debug_stream () << *(yysym.value.sval); }
        break;

      case symbol_kind::S_VALUE: // "value"
                    { debug_stream () << *(yysym.value.sval); }
        break;

      case symbol_kind::S_BEGIN_EXTINF: // "begin_ext_inf"
                    { if ((yysym.value.eeval)) debug_stream () << Common_Parser_M3U_Tools::ElementToString (*(yysym.value.eeval)); }
        break;

      case symbol_kind::S_BEGIN_MEDIA: // "begin_media"
                    { if ((yysym.value.emval)) debug_stream () << Common_Parser_M3U_Tools::ElementToString (*(yysym.value.emval)); }
        break;

      case symbol_kind::S_BEGIN_EXT_STREAM_INF: // "begin_ext_stream_inf"
                    { if ((yysym.value.esval)) debug_stream () << Common_Parser_M3U_Tools::ElementToString (*(yysym.value.esval)); }
        break;

      case symbol_kind::S_EXT3MU: // "extm3u"
                    { if ((yysym.value.lval)) debug_stream () << Common_Parser_M3U_Tools::PlaylistToString (*(yysym.value.lval)); }
        break;

      case symbol_kind::S_playlist: // playlist
                    { debug_stream () << (yysym.value.ival); }
        break;

      case symbol_kind::S_ext_x_key_values: // ext_x_key_values
                    { debug_stream () << (yysym.value.ival); }
        break;

      case symbol_kind::S_ext_x_key_value: // ext_x_key_value
                    { debug_stream () << (yysym.value.ival); }
        break;

      case symbol_kind::S_ext_x_key_value_2: // ext_x_key_value_2
                    { debug_stream () << (yysym.value.ival); }
        break;

      case symbol_kind::S_elements: // elements
                    { debug_stream () << (yysym.value.ival); }
        break;

      case symbol_kind::S_program_date_time: // program_date_time
                    { debug_stream () << (yysym.value.ival); }
        break;

      case symbol_kind::S_ext_inf_rest_1: // ext_inf_rest_1
                    { debug_stream () << (yysym.value.ival); }
        break;

      case symbol_kind::S_ext_inf_rest_2: // ext_inf_rest_2
                    { debug_stream () << (yysym.value.ival); }
        break;

      case symbol_kind::S_ext_inf_rest_3: // ext_inf_rest_3
                    { debug_stream () << (yysym.value.ival); }
        break;

      case symbol_kind::S_ext_inf_rest_4: // ext_inf_rest_4
                    { debug_stream () << (yysym.value.ival); }
        break;

      case symbol_kind::S_ext_media_rest_1: // ext_media_rest_1
                    { debug_stream () << (yysym.value.ival); }
        break;

      case symbol_kind::S_ext_media_key_values: // ext_media_key_values
                    { debug_stream () << (yysym.value.ival); }
        break;

      case symbol_kind::S_ext_media_key_value: // ext_media_key_value
                    { debug_stream () << (yysym.value.ival); }
        break;

      case symbol_kind::S_ext_media_rest_4: // ext_media_rest_4
                    { debug_stream () << (yysym.value.ival); }
        break;

      case symbol_kind::S_ext_stream_inf_rest_1: // ext_stream_inf_rest_1
                    { debug_stream () << (yysym.value.ival); }
        break;

      case symbol_kind::S_ext_stream_inf_key_values: // ext_stream_inf_key_values
                    { debug_stream () << (yysym.value.ival); }
        break;

      case symbol_kind::S_ext_stream_inf_key_value: // ext_stream_inf_key_value
                    { debug_stream () << (yysym.value.ival); }
        break;

      case symbol_kind::S_ext_stream_inf_rest_3: // ext_stream_inf_rest_3
                    { debug_stream () << (yysym.value.ival); }
        break;

      case symbol_kind::S_ext_stream_inf_rest_4: // ext_stream_inf_rest_4
                    { debug_stream () << (yysym.value.ival); }
        break;

      default:
        break;
    }
        yyo << ')';
      }
  }
#endif

  void
  parser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  parser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  parser::yypop_ (int n) YY_NOEXCEPT
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  parser::debug_level_type
  parser::debug_level () const
  {
    return yydebug_;
  }

  void
  parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  parser::state_type
  parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  parser::yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  parser::yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yytable_ninf_;
  }

  int
  parser::operator() ()
  {
    return parse ();
  }

  int
  parser::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    // User initialization code.
{
  // initialize the location
  yyla.location.initialize (NULL);
}



    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            yyla.kind_ = yytranslate_ (yylex (&yyla.value, &yyla.location, iparser, iscanner->getR_2 ().context));
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* If YYLEN is nonzero, implement the default value of the
         action: '$$ = $1'.  Otherwise, use the top of the stack.

         Otherwise, the following line sets YYLHS.VALUE to garbage.
         This behavior is undocumented and Bison users should not rely
         upon it.  */
      if (yylen)
        yylhs.value = yystack_[yylen - 1].value;
      else
        yylhs.value = yystack_[0].value;

      // Default location.
      {
        stack_type::slice range (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, range, yylen);
        yyerror_range[1].location = yylhs.location;
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2: // $@1: %empty
                           {
                    iparser->setP ((yystack_[0].value.lval));
                  }
    break;

  case 3: // playlist: "extm3u" $@1 ext_x_key_values elements "lf"
                                                   {
                    struct M3U_Playlist& playlist_r = iparser->current ();
                    struct M3U_Playlist* playlist_p = &playlist_r;
                    try {
                      iparser->record (playlist_p);
                    } catch (...) {
                      ACE_DEBUG ((LM_ERROR,
                                  ACE_TEXT ("caught exception in M3U_IParser::record(), continuing\n")));
                    }
                    iparser->reset ();
                    YYACCEPT;
                  }
    break;

  case 4: // ext_x_key_values: ext_x_key_values ext_x_key_value
                  { (yylhs.value.ival) = (yystack_[1].value.ival); }
    break;

  case 5: // ext_x_key_values: %empty
                                       { }
    break;

  case 6: // $@2: %empty
                        {
                    iparser->setKey (*(yystack_[0].value.sval)); }
    break;

  case 7: // ext_x_key_value: "key" $@2 "value"
                                                     {
                    struct M3U_Playlist& playlist_r = iparser->current ();
                    playlist_r.keyValues.push_back (std::make_pair (iparser->lastKey (), *(yystack_[0].value.sval)));
                  }
    break;

  case 8: // ext_x_key_value_2: ext_x_key_value
                   { (yylhs.value.ival) = (yystack_[0].value.ival); }
    break;

  case 9: // ext_x_key_value_2: %empty
                                        { }
    break;

  case 10: // elements: elements element
                  { (yylhs.value.ival) = (yystack_[1].value.ival); }
    break;

  case 11: // elements: %empty
                                       { }
    break;

  case 12: // $@3: %empty
                                                                      {
                    iparser->setP_2 ((yystack_[0].value.eeval));
                    std::string date_time_value = iparser->dateTimeValue ();
                    if (!date_time_value.empty ())
                    {
                      struct M3U_ExtInf_Element& element_r = iparser->current_2 ();
                      element_r.keyValues.push_back (std::make_pair (ACE_TEXT_ALWAYS_CHAR (COMMON_PARSER_M3U_EXT_X_PROGRAM_DATE_TIME), date_time_value));
                      iparser->setDateTimeValue (ACE_TEXT_ALWAYS_CHAR (""));
                    } // end IF
                  }
    break;

  case 13: // element: program_date_time ext_x_key_value_2 "begin_ext_inf" $@3 ext_inf_rest_1
                                   { }
    break;

  case 14: // $@4: %empty
                                  {
                    iparser->setP_3 ((yystack_[0].value.emval));
                  }
    break;

  case 15: // element: "begin_media" $@4 ext_media_rest_1
                                     { }
    break;

  case 16: // $@5: %empty
                                           {
                    iparser->setP_4 ((yystack_[0].value.esval));
                    std::string date_time_value = iparser->dateTimeValue ();
                    if (!date_time_value.empty ())
                    {
                      struct M3U_StreamInf_Element& element_r = iparser->current_4 ();
                      element_r.keyValues.push_back (std::make_pair (ACE_TEXT_ALWAYS_CHAR (COMMON_PARSER_M3U_EXT_X_PROGRAM_DATE_TIME), date_time_value));
                      iparser->setDateTimeValue (ACE_TEXT_ALWAYS_CHAR (""));
                    } // end IF
                  }
    break;

  case 17: // element: "begin_ext_stream_inf" $@5 ext_stream_inf_rest_1
                                          { }
    break;

  case 18: // program_date_time: "date_time"
                               {
                    iparser->setDateTimeValue (*(yystack_[0].value.sval)); }
    break;

  case 19: // program_date_time: %empty
                                        { }
    break;

  case 20: // $@6: %empty
                           {
                    struct M3U_ExtInf_Element& element_r = iparser->current_2 ();
                    element_r.Length = (yystack_[0].value.ival); }
    break;

  case 21: // ext_inf_rest_1: "length" $@6 ext_inf_rest_2
                                                            { }
    break;

  case 22: // $@7: %empty
                          {
                    struct M3U_ExtInf_Element& element_r = iparser->current_2 ();
                    element_r.Title = *(yystack_[0].value.sval); }
    break;

  case 23: // ext_inf_rest_2: "title" $@7 ext_inf_rest_3
                                                            { }
    break;

  case 24: // ext_inf_rest_2: ext_inf_rest_3
                                   { }
    break;

  case 25: // $@8: %empty
                                          {
                    struct M3U_ExtInf_Element& element_r = iparser->current_2 ();
                    element_r.URL = *(yystack_[0].value.sval);
                    std::string date_time_value = iparser->dateTimeValue ();
                    if (!date_time_value.empty ())
                    {
                      element_r.keyValues.push_back (std::make_pair (ACE_TEXT_ALWAYS_CHAR (COMMON_PARSER_M3U_EXT_X_PROGRAM_DATE_TIME), date_time_value));
                      iparser->setDateTimeValue (ACE_TEXT_ALWAYS_CHAR (""));
                    } // end IF
                  }
    break;

  case 26: // ext_inf_rest_3: program_date_time "URL" $@8 ext_inf_rest_4
                                   { }
    break;

  case 27: // ext_inf_rest_4: "element_end"
                                {
                    struct M3U_Playlist& playlist_r = iparser->current ();
                    struct M3U_ExtInf_Element& element_r = iparser->current_2 ();
                    playlist_r.ext_inf_elements.push_back (element_r); }
    break;

  case 28: // ext_media_rest_1: ext_media_key_values ext_media_rest_4
                                                        { }
    break;

  case 29: // ext_media_key_values: ext_media_key_values ext_media_key_value
                      { (yylhs.value.ival) = (yystack_[1].value.ival); }
    break;

  case 30: // ext_media_key_values: %empty
                                           { }
    break;

  case 31: // $@9: %empty
                             {
                    iparser->setKey (*(yystack_[0].value.sval)); }
    break;

  case 32: // ext_media_key_value: "key" $@9 "value"
                                                     {
                    struct M3U_Media_Element& element_r = iparser->current_3 ();
                    element_r.keyValues.push_back (std::make_pair (iparser->lastKey (), *(yystack_[0].value.sval))); }
    break;

  case 33: // ext_media_rest_4: "element_end"
                                  {
  struct M3U_Playlist& playlist_r = iparser->current ();
  struct M3U_Media_Element& element_r = iparser->current_3 ();
  playlist_r.media_elements.push_back (element_r); }
    break;

  case 34: // ext_stream_inf_rest_1: ext_stream_inf_key_values ext_stream_inf_rest_3
                                                                       { }
    break;

  case 35: // ext_stream_inf_key_values: ext_stream_inf_key_values ext_stream_inf_key_value
                           { (yylhs.value.ival) = (yystack_[1].value.ival); }
    break;

  case 36: // ext_stream_inf_key_values: %empty
                                 { }
    break;

  case 37: // $@10: %empty
                                  {
  iparser->setKey (*(yystack_[0].value.sval)); }
    break;

  case 38: // ext_stream_inf_key_value: "key" $@10 "value"
                                   {
  struct M3U_StreamInf_Element& element_r = iparser->current_4 ();
  element_r.keyValues.push_back (std::make_pair (iparser->lastKey (), *(yystack_[0].value.sval))); }
    break;

  case 39: // $@11: %empty
                               {
  struct M3U_StreamInf_Element& element_r = iparser->current_4 ();
  element_r.URL = *(yystack_[0].value.sval); }
    break;

  case 40: // ext_stream_inf_rest_3: "URL" $@11 ext_stream_inf_rest_4
                                               { }
    break;

  case 41: // ext_stream_inf_rest_4: "element_end"
                                       {
  struct M3U_Playlist& playlist_r = iparser->current ();
  struct M3U_StreamInf_Element& element_r = iparser->current_4 ();
  playlist_r.stream_inf_elements.push_back (element_r); }
    break;



            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        context yyctx (*this, yyla);
        std::string msg = yysyntax_error_ (yyctx);
        error (yyla.location, YY_MOVE (msg));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yyerror_range[1].location = yystack_[0].location;
        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
  }

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  parser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr;
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              else
                goto append;

            append:
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }

  std::string
  parser::symbol_name (symbol_kind_type yysymbol)
  {
    return yytnamerr_ (yytname_[yysymbol]);
  }



  // parser::context.
  parser::context::context (const parser& yyparser, const symbol_type& yyla)
    : yyparser_ (yyparser)
    , yyla_ (yyla)
  {}

  int
  parser::context::expected_tokens (symbol_kind_type yyarg[], int yyargn) const
  {
    // Actual number of expected tokens
    int yycount = 0;

    const int yyn = yypact_[+yyparser_.yystack_[0].state];
    if (!yy_pact_value_is_default_ (yyn))
      {
        /* Start YYX at -YYN if negative to avoid negative indexes in
           YYCHECK.  In other words, skip the first -YYN actions for
           this state because they are default actions.  */
        const int yyxbegin = yyn < 0 ? -yyn : 0;
        // Stay within bounds of both yycheck and yytname.
        const int yychecklim = yylast_ - yyn + 1;
        const int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
        for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
          if (yycheck_[yyx + yyn] == yyx && yyx != symbol_kind::S_YYerror
              && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
            {
              if (!yyarg)
                ++yycount;
              else if (yycount == yyargn)
                return 0;
              else
                yyarg[yycount++] = YY_CAST (symbol_kind_type, yyx);
            }
      }

    if (yyarg && yycount == 0 && 0 < yyargn)
      yyarg[0] = symbol_kind::S_YYEMPTY;
    return yycount;
  }






  int
  parser::yy_syntax_error_arguments_ (const context& yyctx,
                                                 symbol_kind_type yyarg[], int yyargn) const
  {
    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state merging
         (from LALR or IELR) and default reductions corrupt the expected
         token list.  However, the list is correct for canonical LR with
         one exception: it will still contain any token that will not be
         accepted due to an error action in a later state.
    */

    if (!yyctx.lookahead ().empty ())
      {
        if (yyarg)
          yyarg[0] = yyctx.token ();
        int yyn = yyctx.expected_tokens (yyarg ? yyarg + 1 : yyarg, yyargn - 1);
        return yyn + 1;
      }
    return 0;
  }

  // Generate an error message.
  std::string
  parser::yysyntax_error_ (const context& yyctx) const
  {
    // Its maximum.
    enum { YYARGS_MAX = 5 };
    // Arguments of yyformat.
    symbol_kind_type yyarg[YYARGS_MAX];
    int yycount = yy_syntax_error_arguments_ (yyctx, yyarg, YYARGS_MAX);

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
      default: // Avoid compiler warnings.
        YYCASE_ (0, YY_("syntax error"));
        YYCASE_ (1, YY_("syntax error, unexpected %s"));
        YYCASE_ (2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_ (3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_ (4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_ (5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    std::ptrdiff_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += symbol_name (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const signed char parser::yypact_ninf_ = -27;

  const signed char parser::yytable_ninf_ = -1;

  const signed char
  parser::yypact_[] =
  {
      -6,   -27,    11,   -27,   -27,     6,   -27,   -27,    -3,     3,
     -27,   -27,   -27,   -27,   -27,     6,   -27,   -27,   -27,   -27,
       4,   -27,     1,   -27,    -5,   -27,   -27,   -27,   -27,   -27,
     -27,   -27,   -27,   -27,     7,     8,     9,    12,   -27,   -27,
     -27,   -27,   -27,   -27,    -2,   -27,    13,   -27,   -27,    15,
     -27,   -27,    17,   -27,   -27
  };

  const signed char
  parser::yydefact_[] =
  {
       0,     2,     0,     5,     1,    11,     6,     4,    19,     0,
       3,    18,    14,    16,    10,     9,     7,    30,    36,     8,
       0,    15,     0,    17,     0,    12,    33,    31,    29,    28,
      37,    39,    35,    34,     0,     0,     0,     0,    20,    13,
      32,    38,    41,    40,    19,    22,     0,    21,    24,    19,
      25,    23,     0,    27,    26
  };

  const signed char
  parser::yypgoto_[] =
  {
     -27,   -27,   -27,   -27,     2,   -27,   -27,   -27,   -27,   -27,
     -27,   -27,    16,   -27,   -27,   -27,   -27,   -26,   -27,   -27,
     -27,   -27,   -27,   -27,   -27,   -27,   -27,   -27,   -27,   -27,
     -27,   -27
  };

  const signed char
  parser::yydefgoto_[] =
  {
       0,     2,     3,     5,     7,     9,    20,     8,    14,    34,
      17,    18,    46,    39,    44,    47,    49,    48,    52,    54,
      21,    22,    28,    35,    29,    23,    24,    32,    36,    33,
      37,    43
  };

  const signed char
  parser::yytable_[] =
  {
      10,    30,    11,    11,    31,    26,    45,    27,     1,    12,
      13,     4,     6,    16,    38,    25,    42,    19,    40,    41,
      11,    53,    50,    51,    15
  };

  const signed char
  parser::yycheck_[] =
  {
       3,     6,     5,     5,     9,     4,     8,     6,    14,    12,
      13,     0,     6,    10,     7,    11,     4,    15,    10,    10,
       5,     4,     9,    49,     8
  };

  const signed char
  parser::yystos_[] =
  {
       0,    14,    16,    17,     0,    18,     6,    19,    22,    20,
       3,     5,    12,    13,    23,    27,    10,    25,    26,    19,
      21,    35,    36,    40,    41,    11,     4,     6,    37,    39,
       6,     9,    42,    44,    24,    38,    43,    45,     7,    28,
      10,    10,     4,    46,    29,     8,    27,    30,    32,    31,
       9,    32,    33,     4,    34
  };

  const signed char
  parser::yyr1_[] =
  {
       0,    15,    17,    16,    18,    18,    20,    19,    21,    21,
      22,    22,    24,    23,    25,    23,    26,    23,    27,    27,
      29,    28,    31,    30,    30,    33,    32,    34,    35,    36,
      36,    38,    37,    39,    40,    41,    41,    43,    42,    45,
      44,    46
  };

  const signed char
  parser::yyr2_[] =
  {
       0,     2,     0,     5,     2,     0,     0,     3,     1,     0,
       2,     0,     0,     5,     0,     3,     0,     3,     1,     0,
       0,     3,     0,     3,     1,     0,     4,     1,     2,     2,
       0,     0,     3,     1,     2,     2,     0,     0,     3,     0,
       3,     1
  };


#if YYDEBUG || 1
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const parser::yytname_[] =
  {
  "\"end\"", "error", "\"invalid token\"", "\"lf\"", "\"element_end\"",
  "\"date_time\"", "\"key\"", "\"length\"", "\"title\"", "\"URL\"",
  "\"value\"", "\"begin_ext_inf\"", "\"begin_media\"",
  "\"begin_ext_stream_inf\"", "\"extm3u\"", "$accept", "playlist", "$@1",
  "ext_x_key_values", "ext_x_key_value", "$@2", "ext_x_key_value_2",
  "elements", "element", "$@3", "$@4", "$@5", "program_date_time",
  "ext_inf_rest_1", "$@6", "ext_inf_rest_2", "$@7", "ext_inf_rest_3",
  "$@8", "ext_inf_rest_4", "ext_media_rest_1", "ext_media_key_values",
  "ext_media_key_value", "$@9", "ext_media_rest_4",
  "ext_stream_inf_rest_1", "ext_stream_inf_key_values",
  "ext_stream_inf_key_value", "$@10", "ext_stream_inf_rest_3", "$@11",
  "ext_stream_inf_rest_4", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const short
  parser::yyrline_[] =
  {
       0,   195,   195,   195,   208,   210,   211,   211,   216,   218,
     220,   222,   223,   223,   233,   233,   236,   236,   246,   249,
     251,   251,   254,   254,   257,   258,   258,   268,   273,   274,
     276,   277,   277,   281,   286,   287,   289,   290,   290,   294,
     294,   297
  };

  void
  parser::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
  parser::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG

  parser::symbol_kind_type
  parser::yytranslate_ (int t) YY_NOEXCEPT
  {
    // YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to
    // TOKEN-NUM as returned by yylex.
    static
    const signed char
    translate_table[] =
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
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14
    };
    // Last valid token kind.
    const int code_max = 269;

    if (t <= 0)
      return symbol_kind::S_YYEOF;
    else if (t <= code_max)
      return static_cast <symbol_kind_type> (translate_table[t]);
    else
      return symbol_kind::S_YYUNDEF;
  }

} // yy



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
