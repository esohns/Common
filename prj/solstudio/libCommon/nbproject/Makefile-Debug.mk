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
	${OBJECTDIR}/_ext/1386528437/common_file_tools.o \
	${OBJECTDIR}/_ext/1386528437/common_logger.o \
	${OBJECTDIR}/_ext/1386528437/common_referencecounter_base.o \
	${OBJECTDIR}/_ext/1386528437/common_signalhandler.o \
	${OBJECTDIR}/_ext/1386528437/common_statemachine_base.o \
	${OBJECTDIR}/_ext/1386528437/common_task_base.o \
	${OBJECTDIR}/_ext/1386528437/common_timer_manager.o \
	${OBJECTDIR}/_ext/1386528437/common_timerhandler.o \
	${OBJECTDIR}/_ext/1386528437/common_tools.o \
	${OBJECTDIR}/_ext/1386528437/stdafx.o


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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/liblibCommon.${CND_DLIB_EXT}

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/liblibCommon.${CND_DLIB_EXT}: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/liblibCommon.${CND_DLIB_EXT} ${OBJECTFILES} ${LDLIBSOPTIONS} -G -KPIC -norunpath -h liblibCommon.${CND_DLIB_EXT}

${OBJECTDIR}/_ext/1386528437/common_file_tools.o: ../../../src/common_file_tools.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	$(COMPILE.cc) -g -DHAVE_CONFIG_H -D_POSIX_PTHREAD_SEMANTICS -I../../../include -I../../../../ATCD/ACE -KPIC  -o ${OBJECTDIR}/_ext/1386528437/common_file_tools.o ../../../src/common_file_tools.cpp

${OBJECTDIR}/_ext/1386528437/common_logger.o: ../../../src/common_logger.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	$(COMPILE.cc) -g -DHAVE_CONFIG_H -D_POSIX_PTHREAD_SEMANTICS -I../../../include -I../../../../ATCD/ACE -KPIC  -o ${OBJECTDIR}/_ext/1386528437/common_logger.o ../../../src/common_logger.cpp

${OBJECTDIR}/_ext/1386528437/common_referencecounter_base.o: ../../../src/common_referencecounter_base.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	$(COMPILE.cc) -g -DHAVE_CONFIG_H -D_POSIX_PTHREAD_SEMANTICS -I../../../include -I../../../../ATCD/ACE -KPIC  -o ${OBJECTDIR}/_ext/1386528437/common_referencecounter_base.o ../../../src/common_referencecounter_base.cpp

${OBJECTDIR}/_ext/1386528437/common_signalhandler.o: ../../../src/common_signalhandler.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	$(COMPILE.cc) -g -DHAVE_CONFIG_H -D_POSIX_PTHREAD_SEMANTICS -I../../../include -I../../../../ATCD/ACE -KPIC  -o ${OBJECTDIR}/_ext/1386528437/common_signalhandler.o ../../../src/common_signalhandler.cpp

${OBJECTDIR}/_ext/1386528437/common_statemachine_base.o: ../../../src/common_statemachine_base.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	$(COMPILE.cc) -g -DHAVE_CONFIG_H -D_POSIX_PTHREAD_SEMANTICS -I../../../include -I../../../../ATCD/ACE -KPIC  -o ${OBJECTDIR}/_ext/1386528437/common_statemachine_base.o ../../../src/common_statemachine_base.cpp

${OBJECTDIR}/_ext/1386528437/common_task_base.o: ../../../src/common_task_base.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	$(COMPILE.cc) -g -DHAVE_CONFIG_H -D_POSIX_PTHREAD_SEMANTICS -I../../../include -I../../../../ATCD/ACE -KPIC  -o ${OBJECTDIR}/_ext/1386528437/common_task_base.o ../../../src/common_task_base.cpp

${OBJECTDIR}/_ext/1386528437/common_timer_manager.o: ../../../src/common_timer_manager.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	$(COMPILE.cc) -g -DHAVE_CONFIG_H -D_POSIX_PTHREAD_SEMANTICS -I../../../include -I../../../../ATCD/ACE -KPIC  -o ${OBJECTDIR}/_ext/1386528437/common_timer_manager.o ../../../src/common_timer_manager.cpp

${OBJECTDIR}/_ext/1386528437/common_timerhandler.o: ../../../src/common_timerhandler.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	$(COMPILE.cc) -g -DHAVE_CONFIG_H -D_POSIX_PTHREAD_SEMANTICS -I../../../include -I../../../../ATCD/ACE -KPIC  -o ${OBJECTDIR}/_ext/1386528437/common_timerhandler.o ../../../src/common_timerhandler.cpp

${OBJECTDIR}/_ext/1386528437/common_tools.o: ../../../src/common_tools.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	$(COMPILE.cc) -g -DHAVE_CONFIG_H -D_POSIX_PTHREAD_SEMANTICS -I../../../include -I../../../../ATCD/ACE -KPIC  -o ${OBJECTDIR}/_ext/1386528437/common_tools.o ../../../src/common_tools.cpp

${OBJECTDIR}/_ext/1386528437/stdafx.o: ../../../src/stdafx.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	$(COMPILE.cc) -g -DHAVE_CONFIG_H -D_POSIX_PTHREAD_SEMANTICS -I../../../include -I../../../../ATCD/ACE -KPIC  -o ${OBJECTDIR}/_ext/1386528437/stdafx.o ../../../src/stdafx.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/liblibCommon.${CND_DLIB_EXT}
	${CCADMIN} -clean

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
