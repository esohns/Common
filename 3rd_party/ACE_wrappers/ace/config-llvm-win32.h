#define ACE_CONFIG_WIN32_H
#include "ace/config-win32-common.h"

// *TODO*: these seem to be handled inconsistently and should probably be moved
//         to config-win32-common.h
#define ACE_LACKS_DIRENT_H
#define ACE_LACKS_DLFCN_H
#define ACE_LACKS_NET_IF_H
#define ACE_LACKS_NETDB_H
#define ACE_LACKS_NETINET_IN_H
#define ACE_LACKS_POLL_H
#define ACE_LACKS_SEMAPHORE_H
#define ACE_LACKS_STRINGS_H
#define ACE_LACKS_STROPTS_H
#define ACE_LACKS_SYS_IOCTL_H
#define ACE_LACKS_SYS_IPC_H
#define ACE_LACKS_SYS_MMAN_H
#define ACE_LACKS_SYS_RESOURCE_H
#define ACE_LACKS_SYS_SELECT_H
#define ACE_LACKS_SYS_SEM_H
#define ACE_LACKS_SYS_SOCKET_H
#define ACE_LACKS_SYS_TIME_H
#define ACE_LACKS_SYS_UIO_H
#define ACE_LACKS_SYS_WAIT_H
#define ACE_LACKS_UCONTEXT_H

/////////////////////////////////////////

// *NOTE*: llvm does not support exceptions on win32 platforms yet
//         (see also: http://stackoverflow.com/questions/24197773/c-program-not-compiling-with-clang-and-visual-studio-2010-express)
#define _HAS_EXCEPTIONS 0

//#define ACE_throw_bad_alloc

/////////////////////////////////////////

// *NOTE*: this settings corresponds with the UNICODE preprocessor symbol (MSVC
//         builds):
// *TODO*: check how this correlates with the standard multi-byte compiler
//         setting; in places, this seems to be inconsistent throughout both
//         frameworks
//#define ACE_USES_WCHAR 1

// *NOTE*: needed for ACE_IOStream
#define ACE_HAS_STANDARD_CPP_LIBRARY 1
#define ACE_USES_STD_NAMESPACE_FOR_STDCPP_LIB 1

// *NOTE*: don't use the regular pipe-based mechanism,
// it has several drawbacks (see relevant documentation)
#define ACE_HAS_REACTOR_NOTIFICATION_QUEUE

// *NOTE*: don't use the WFMO-reactor on Microsoft Windows (TM) platforms,
//         it only supports ~64 concurrent handles...
#define ACE_USE_SELECT_REACTOR_FOR_REACTOR_IMPL

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
#define ACE_LACKS_SIGNED_CHAR
