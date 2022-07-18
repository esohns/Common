// *NOTE*: compile with large(r) file support
#define _FILE_OFFSET_BITS 64

#include "ace/config-win32.h"

// *NOTE*: this settings corresponds with the UNICODE preprocessor symbol (MSVC
//         builds):
// *TODO*: check how this correlates with the standard multi-byte compiler
//         setting; in places, this seems to be inconsistent throughout both
//         frameworks
//#define ACE_USES_WCHAR 1

// *NOTE*: needed for ACE_IOStream
//#define ACE_HAS_STANDARD_CPP_LIBRARY 1

// *NOTE*: don't use the regular pipe-based mechanism,
// it has several drawbacks (see relevant documentation)
#define ACE_HAS_REACTOR_NOTIFICATION_QUEUE 1

// *NOTE*: don't use the WFMO-reactor on Microsoft Windows (TM) platforms,
//         it only supports ~64 concurrent handles...
#define ACE_USE_SELECT_REACTOR_FOR_REACTOR_IMPL 1

// *NOTE*: (proactor) ACE contains SEH-enabled code; however, the default MPC
//         (auto-)generated project files do not apply the correct compilation
//         flags (sets /EHsc instead of /EHa), leading to memory leaks
//         --> as a workaround, disable SEH
#undef ACE_HAS_WIN32_STRUCTURAL_EXCEPTIONS

// *NOTE*: ACE_IOStream support requires these definitions
//#define ACE_USES_OLD_IOSTREAMS
//#undef ACE_LACKS_ACE_IOSTREAM
// *NOTE*: <iostream> from compat-gcc-34-c++ (3.4.6) has no support for
//         iostream::ipfx/ipsx, and MSVC complains about the declarations
//#define ACE_LACKS_IOSTREAM_FX

#define ACE_LACKS_LINEBUFFERED_STREAMBUF
#define ACE_LACKS_UNBUFFERED_STREAMBUF
// *NOTE*: recent gcc iostream::operator>> implementations do not support
//         pointer-type arguments any more; this disables the corresponding
//         (macro) code in IOStream.h
// *NOTE*: MSVC complains about the generated code as well...
// *TODO*: remove this define and add platform/compiler-specific #ifdefs to
//         IOStream.h
//#define ACE_LACKS_SIGNED_CHAR

// *NOTE*: make sure the ACE project files are generated with ipv6=1 (see local.features),
//         otherwise there will be stack corruption around ACE_INET_Addr
//         see also: https://stevehuston.wordpress.com/2010/07/02/trouble-with-ace-and-ipv6-make-sure-your-config-is-consistent/
#define ACE_HAS_IPV6
