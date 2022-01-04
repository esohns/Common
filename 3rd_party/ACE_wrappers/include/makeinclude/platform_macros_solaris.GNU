# -*- Makefile -*-

INSTALL_PREFIX=/usr/local

#ace_for_tao=0

buildbits=32
debug=1
dmalloc=0
exceptions=0
fast=0
include_env=0
inline=1
insure=0
link_groups=1
minimum_corba=0
mtrace=0
optimize=0
pace=0
probe=0
profile=0
purify=0
quantify=0
repo=0
rtti=1
shared_libs=1
static_libs=0
shared_libs_only=1
static_libs_only=0
split=0
static_link=0
#symlinks=absolute
templates=automatic
threads=1
trio=0
versioned_so=1

# features
pipes=1
qos=1
rapi=1
ssl=1

# (ui) reactors
fl=1
fox=1
tk=1
qt=1
wfmo=0
xt=1

# toolkits
stlport=0
winnt=0
winregistry=0

# apps
rwho=1
#sctp=openss7

include $(ACE_ROOT)/include/makeinclude/platform_sunos5_sunc++.GNU
