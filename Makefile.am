#ACLOCAL_AMFLAGS = -I m4

SUBDIRS = \
  src
if BUILD_MODULES
 SUBDIRS += modules
endif

DIST_SUBDIRS = \
  3rd_party \
  m4 \
  scripts \
  src
if BUILD_MODULES
  DIST_SUBDIRS += modules
endif

EXTRA_DIST = \
  autogen.sh \
  LICENSE \
  README.md

