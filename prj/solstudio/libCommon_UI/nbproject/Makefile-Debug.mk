#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=cc
CCC=CC
CXX=CC
FC=f95
AS=as

# Macros
CND_PLATFORM=OracleSolarisStudio-Solaris-x86
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/1368229934/common_ui_glade_definition.o \
	${OBJECTDIR}/_ext/1368229934/common_ui_gtk_builder_definition.o \
	${OBJECTDIR}/_ext/1368229934/common_ui_gtk_manager.o \
	${OBJECTDIR}/_ext/1368229934/common_ui_tools.o \
	${OBJECTDIR}/_ext/1368229934/stdafx.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/liblibCommon_UI.${CND_DLIB_EXT}

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/liblibCommon_UI.${CND_DLIB_EXT}: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/liblibCommon_UI.${CND_DLIB_EXT} ${OBJECTFILES} ${LDLIBSOPTIONS} -G -KPIC -norunpath -h liblibCommon_UI.${CND_DLIB_EXT}

${OBJECTDIR}/_ext/1368229934/common_ui_glade_definition.o: ../../../src/ui/common_ui_glade_definition.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1368229934
	$(COMPILE.cc) -g -D_POSIX_PTHREAD_SEMANTICS -I../../../src -I/usr/include/libglade-2.0 -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/lib/gtk-2.0/include -I/usr/include/gtk-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/glib-2.0 -I../../../../ATCD/ACE -KPIC  -o ${OBJECTDIR}/_ext/1368229934/common_ui_glade_definition.o ../../../src/ui/common_ui_glade_definition.cpp

${OBJECTDIR}/_ext/1368229934/common_ui_gtk_builder_definition.o: ../../../src/ui/common_ui_gtk_builder_definition.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1368229934
	$(COMPILE.cc) -g -D_POSIX_PTHREAD_SEMANTICS -I../../../src -I/usr/include/libglade-2.0 -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/lib/gtk-2.0/include -I/usr/include/gtk-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/glib-2.0 -I../../../../ATCD/ACE -KPIC  -o ${OBJECTDIR}/_ext/1368229934/common_ui_gtk_builder_definition.o ../../../src/ui/common_ui_gtk_builder_definition.cpp

${OBJECTDIR}/_ext/1368229934/common_ui_gtk_manager.o: ../../../src/ui/common_ui_gtk_manager.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1368229934
	$(COMPILE.cc) -g -D_POSIX_PTHREAD_SEMANTICS -I../../../src -I/usr/include/libglade-2.0 -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/lib/gtk-2.0/include -I/usr/include/gtk-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/glib-2.0 -I../../../../ATCD/ACE -KPIC  -o ${OBJECTDIR}/_ext/1368229934/common_ui_gtk_manager.o ../../../src/ui/common_ui_gtk_manager.cpp

${OBJECTDIR}/_ext/1368229934/common_ui_tools.o: ../../../src/ui/common_ui_tools.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1368229934
	$(COMPILE.cc) -g -D_POSIX_PTHREAD_SEMANTICS -I../../../src -I/usr/include/libglade-2.0 -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/lib/gtk-2.0/include -I/usr/include/gtk-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/glib-2.0 -I../../../../ATCD/ACE -KPIC  -o ${OBJECTDIR}/_ext/1368229934/common_ui_tools.o ../../../src/ui/common_ui_tools.cpp

${OBJECTDIR}/_ext/1368229934/stdafx.o: ../../../src/ui/stdafx.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1368229934
	$(COMPILE.cc) -g -D_POSIX_PTHREAD_SEMANTICS -I../../../src -I/usr/include/libglade-2.0 -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/lib/gtk-2.0/include -I/usr/include/gtk-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/glib-2.0 -I../../../../ATCD/ACE -KPIC  -o ${OBJECTDIR}/_ext/1368229934/stdafx.o ../../../src/ui/stdafx.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/liblibCommon_UI.${CND_DLIB_EXT}
	${CCADMIN} -clean

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
