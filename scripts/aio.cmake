set (AIO_SUPPORT_DEFAULT ON)
if (UNIX)
 include (CheckLibraryExists)
 check_library_exists (rt aio_init "" HAVE_POSIX_AIO)
 if (HAVE_POSIX_AIO)
  message (STATUS "found POSIX aio")
  set (AIO_FOUND TRUE)
 endif (HAVE_POSIX_AIO)

# set (AIO_LIBRARY /usr/lib64/libaio.so)
 set (AIO_LIB_FILE aio)
 find_library (AIO_LIBRARY
               NAMES ${AIO_LIB_FILE} libaio.so
               PATHS /usr
               PATH_SUFFIXES lib64 lib x86_64-linux-gnu
               DOC "searching for ${AIO_LIB_FILE}")
 if (AIO_LIBRARY)
  message (STATUS "found libaio")
  set (AIO_FOUND TRUE)
 endif (AIO_LIBRARY)
endif (UNIX)
if (AIO_FOUND)
 option (AIO_SUPPORT "enable aio support" ${AIO_SUPPORT_DEFAULT})
 if (AIO_SUPPORT)
  add_definitions (-DAIO_SUPPORT)
 endif (AIO_SUPPORT)
endif (AIO_FOUND)
