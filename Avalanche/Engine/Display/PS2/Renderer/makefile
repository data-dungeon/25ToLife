############################################################
#
# PS2Renderer make file.
#
# Environment variables / command line (if defined, they are true):
#
# COMPILER
#		SN
#			use the SN compiler
# 		CW
#			use the CW compiler (default ifndef either of the others)
#
# DIRECT_TO_ENGINE
#			put .a file directly into engine location
#
# VERSION
#		DEBUG
#			build the debug version (default ifndef VERSION)
#		RELEASE
#			build the release version
#		PROFILE
#			build the profiler version
#     CONSUMER_BUILD
#        build the consumer version
#
############################################################
############################################################

############################################################
# source files
############################################################

HANDLER_SOURCE_FILES = VUCodeHandlers.cpp \
	VUAnimData.cpp \
	VUColorSetData.cpp \
	VUStaticData31.cpp

RENDERER_SOURCE_FILES = AnimDDMAObject.cpp \
	PatchDDMAObject.cpp \
	DDMAObject.cpp \
	MasterCodeFactory.cpp \
	MasterList.cpp \
	PS2Renderer.cpp \
	VUCodeFactory.cpp \
	VUCodeManager.cpp \
	VUCodeMemManager.cpp \
	VUCodeSetupHelper.cpp \
	VUCodeSlot.cpp \
	VUDataClass.cpp \
	VUManagedSlot.cpp \
	VUMicroTables.cpp \
	VUCodeDefs.cpp \
	VUDynamicMPG.cpp 

VU_MICRO_TABLE_FILES = AnimBasicMT.cpp \
	AnimLitMT.cpp \
	AnimLitFogCullMT.cpp \
	AnimLitFogCullPLitMT.cpp \
	AnimLitFogCullPLitMPMT.cpp \
	AnimLitCullMT.cpp \
	AnimLitCullPLitMT.cpp \
	AnimLitCullPLitMPMT.cpp \
	AnimLitFogMT.cpp \
	AnimLitMPMT.cpp \
	AnimPLitMPMT.cpp \
	AnimPLitMT.cpp \
	AnimLitFogCullMPMT.cpp \
	AnimLitCullMPMT.cpp \
	AnimLitFogMPMT.cpp \
	AnimEVMapMT.cpp \
	AnimShadowerMT.cpp

ANIM_VM4_FILES = AnimBasic.vm4 \
	AnimLit.vm4 \
	AnimPLit.vm4 \
	AnimLitFogCull.vm4 \
	AnimLitFogCullPLit.vm4 \
	AnimLitCull.vm4 \
	AnimLitCullPLit.vm4 \
	AnimLitFog.vm4 \
	AnimLitMP.vm4 \
	AnimPLitMP.vm4 \
	AnimLitFogCullMP.vm4 \
	AnimLitFogCullPLitMP.vm4 \
	AnimLitCullMP.vm4 \
	AnimLitCullPLitMP.vm4 \
	AnimLitFogMP.vm4 \
	AnimEVMap.vm4 \
	AnimShadower.vm4

STATIC_VM4_FILES = DBStaticCompress.vm4 \
	DBEVMap.vm4 \
	StaticEVMap.vm4 \
	StaticSphereMap.vm4 \
	StaticRefMap.vm4 \
	StaticSphereRefMap.vm4 \
	DBStaticCompress2AllCore2Clip.vm4 \
	DBStaticCompress2CullMP.vm4 \
	DBStaticCompress2AllCullMP.vm4 \
	DBStaticCompress2AllEngCull.vm4 \
	DBStaticCompress2AllEngCullMP.vm4 \
	DBStaticCompress2AllEngClip.vm4 \
	DBStaticCompressCull.vm4 \
	DBStaticCompressClip.vm4 \
	DBStaticCompress2.vm4 \
	DBStaticCompress2SMC.vm4 \
	DBStaticCompress2SMCCull.vm4 \
	DBStaticCompress2SMCClip.vm4 \
	DBStaticCompress2Cull.vm4 \
	DBStaticCompress2Clip.vm4 \
	DBStaticCompress2All.vm4 \
	DBStaticCompress2AllCull.vm4 \
	DBStaticCompress2AllClip.vm4 \
	DBStaticCompress2VTP.vm4 \
	DBStaticCompress2VTPCull.vm4 \
	DBStaticCompress2VTPClip.vm4 \
	DBStaticCompress2AllClipPlight.vm4 \
	DBStaticCompress2Shadower.vm4

MISC_VM4_FILES = 	GenericBufferClip.vm4

VCS_VM4_FILES = DBVertexColorSetCompress2.vm4 \
	DBVertexColorSetCompress2CullMP.vm4 \
	DBVertexColorSetCompress2AllCore2Clip.vm4 \
	DBVertexColorSetCompress2AllCullMP.vm4 \
	DBVertexColorSetCompress2AllEngCullMP.vm4 \
	DBVertexColorSetCompress2AllEngCull.vm4 \
	DBVertexColorSetCompress2AllEngClip.vm4 \
	DBVertexColorSetCompress2SMCClip.vm4 \
	DBVertexColorSetCompress2SMCCull.vm4 \
	DBVertexColorSetCompress2SMC.vm4 \
	DBVertexColorSetCompress2SMClip.vm4 \
	DBVertexColorSetCompress2SMCull.vm4 \
	DBVertexColorSetCompress2SM.vm4 \
	DBVertexColorSetCompress2Cull.vm4 \
	DBVertexColorSetCompress2Clip.vm4 \
	DBVertexColorSetCompress2All.vm4  \
	DBVertexColorSetCompress2AllCull.vm4 \
	DBVertexColorSetCompress2AllClip.vm4 \
	DBVertexColorSetCompress2AllClipPLight.vm4 \
	DBVertexColorSetCompress2VTP.vm4 \
	DBVertexColorSetCompress2VTPClip.vm4 \
	DBVertexColorSetCompress2VTPCull.vm4 \
	DBVertexColorSetCompress2AllPLight.vm4 \
	DBVertexColorSetCompress2Shadower.vm4

COMMON_SOURCE_FILES = ColorB.cpp \
	ColorF.cpp \
	DMABlock.cpp \
	DMATag.cpp \
	GIFTag.cpp \
	PRIMReg.cpp

############################################################
# select which kind of compiler we'll use
############################################################

%if $(COMPILER) == "SN"
	SN_TOOLS=$(SN_PATH)/ee-gcc/bin/
	CC="$(SN_PATH)/ps2cc"
	AR="$(SN_PATH)/snarl"
	DVPAS="ps2dvpas"
	POSTFIX=sn
	CCOPTS = -Wa,-sn -DCOMPILING_RENDERER -DPS2 -DSN -DA_COMPILER_SN -ffast-math -fno-rtti -fno-exceptions -Wall -Wsoftware-math-library -fdefault-single-float -Wpromote-doubles -Winline -mno-check-zero-division -mvu0-use-vf0-vf15 $(COMMON_CCOPTS)
	CCDEBUG = -g -D_DEBUG
	CCRELEASE = -O3 -g
    CCCONSUMER = -O3 -g -DSHIP_IT
    CCRELEASEEENET = -O3 -g -DEENET
	AROUT = q
	FIX_D_FILE = $(PERL) -pi.bak -e "s/\\r//g; s(([\\.\\:A-Za-z])\\\\([\\.\\:A-Za-z]))(\\1/\\2)g;s((.*)/(.*)\\.o)($(OBJ_DIR)/\\2.o)g;" 
%elseif $(COMPILER) == "CW"
	CC=$(CW_TOOL_FOLDER)mwccps2
	AR=$(CW_TOOL_FOLDER)mwldps2
	DVPAS=$(CW_TOOL_FOLDER)ee-dvp-as
	POSTFIX=cw
	CCOPTS = -DPS2 -DCOMPILING_RENDERER -gccinc -I- -search $(OUTPUTFORMAT) -include PREFIX_PS2.h -DA_COMPILER_CW -w all,nonotused,noextracomma -RTTI off -str readonly,reuse $(COMMON_CCOPTS) 
	CCDEBUG = -w nonotinlined  -sym on,elf -D_DEBUG
	CCRELEASE = -opt level=2,intrinsics -inline all,deferred,level=3
	CCCONSUMER = -opt level=2,intrinsics -inline all,deferred,level=3 -DSHIP_IT
	AROPTS = -library
	AROUT = -o
	FIX_D_FILE = $(PERL) -pi.bak -e "s/\\r//g; s(([\\.\\:A-Za-z])\\\\([\\.\\:A-Za-z]))(\\1/\\2)g;" 
%if $(GCCOUTPUT) == 1
	OUTPUTFORMAT=-msgstyle gcc
%else
	OUTPUTFORMAT=-msgstyle mpw
%endif

%else
error:
	@echo ARGH! invalid COMPILER value passed to make... try SN or CW
%endif

COMMON_CCOPTS = -MD -c $(INCLUDES) $(VERSION_CCOPTS) -DA_PLATFORM_PS2 -DA_RENDERER_PS2 -DPS2
INCLUDES = -I$(VSM_SOURCE_DIR) -I$(INTERMEDIATE_SRC_DIR) -I$(CXX_SOURCE_DIR) -I$(COMMON_SOURCE_DIR) -I$(VUCODE_DIR) -Ic:/usr/local/sce/ee/include 

############################################################
# misc executables
############################################################
MKDIR = cmd /c mkdir
DEL = cmd /c del /f
RMDIR = cmd /c rmdir /q /s
ATTRIB = attrib
M4 = "m4"
PERL = "$(TTLPATH)\bin\perl"
MOVE = move
COPY = copy
ECHO = echo

############################################################
# Folders
############################################################

# build obj folders
DEBUG_PREFIX = debug
RELEASE_PREFIX = release
CONSUMER_PREFIX = consumer
PROFILE_PREFIX = profile
INTERMEDIATE_SRC_DIR = $(OBJ_DIR)

# source folders
VUCODE_DIR = VUCode
VSM_SOURCE_DIR = $(VUCODE_DIR)/Source
CXX_SOURCE_DIR = Code
COMMON_SOURCE_DIR = Common

# setup search path for sources...
VPATH = $(CXX_SOURCE_DIR):$(VSM_SOURCE_DIR):$(COMMON_SOURCE_DIR):$(INTERMEDIATE_SRC_DIR)

############################################################
# final targets
############################################################

%if $(VERSION) == "RELEASE"
	OBJ_DIR = $(COMPILER)PS2Release
	VERSION_CCOPTS = $(CCRELEASE)
%elseif $(VERSION) == "RELEASEEENET"
	OBJ_DIR = $(COMPILER)PS2ReleaseEENET
	VERSION_CCOPTS = $(CCRELEASEEENET)
%elseif $(VERSION) == "PROFILE"
	OBJ_DIR = $(COMPILER)PS2Profile
	VERSION_CCOPTS = $(CCPROFILE)
%elseif $(VERSION) == "CONSUMER_BUILD"
	OBJ_DIR = $(COMPILER)PS2Consumer
	VERSION_CCOPTS = $(CCCONSUMER)
%else
	OBJ_DIR = $(COMPILER)PS2Debug
	VERSION_CCOPTS = $(CCDEBUG)
%endif

TARGET = $(OBJ_DIR)/Renderer.lib


############################################################
# everyone doesn't have to build the renderer.. check for
# quick exit
############################################################

%if !%dir( $(VUCODE_DIR) ) || !%dir( $(CXX_SOURCE_DIR) ) || !%dir( $(COMMON_SOURCE_DIR) )
%if !%file( $(TARGET) )
%echo ERROR: Renderer build failed: No source and can't find $(TARGET)
%exit 1
%endif
%echo Using prebuilt $(TARGET)
%exit 0
%endif


############################################################
# some generated files
############################################################

VUPARSE = $(VSM_SOURCE_DIR)/vuparse.pl
VUCODE_H = $(OBJ_DIR)/VUCode.h

############################################################
# generate pattern substitiutions & conglomerate defines 
# and stuff for files here...
############################################################

VM4_FILES = $(STATIC_VM4_FILES) $(VCS_VM4_FILES) $(MISC_VM4_FILES) $(ANIM_VM4_FILES)
VSM_SOURCE = $(VM4_FILES, <$(VSM_SOURCE_DIR)/)
VSM_TEMP = $(VM4_FILES, <$(INTERMEDIATE_SRC_DIR)/, S/\.vm4/.vsm/)
MICRO_TABLES = $(VU_MICRO_TABLE_FILES,<$(INTERMEDIATE_SRC_DIR)/)

VSM_OBJS = $(VM4_FILES,<$(OBJ_DIR)/,S/\.vm4/.obj/)
CXX_SOURCE_FILES = $(RENDERER_SOURCE_FILES) $(HANDLER_SOURCE_FILES)

CXX_SOURCE = $(CXX_SOURCE_FILES,<$(CXX_SOURCE_DIR)/)
CXX_OBJS = $(CXX_SOURCE_FILES,<$(OBJ_DIR)/,S/\.cpp/.o/)
#CXX_DEPS = $(CXX_OBJS,S/\.o/.d/)

COMMON_SOURCE = $(COMMON_SOURCE_FILES,<$(COMMON_SOURCE_DIR)/)
COMMON_OBJS = $(COMMON_SOURCE_FILES,<$(OBJ_DIR)/,S/\.cpp/.o/)
#COMMON_DEPS = $(COMMON_OBJS,S/\.o/.d/)

#DEPENDANCIES = $(CXX_DEPS) $(COMMON_DEPS)

OBJS = $(VSM_OBJS) $(CXX_OBJS) $(COMMON_OBJS)

############################################################
# deal with compilation/linking flags here
############################################################
M4_OPTS = -I $(VSM_SOURCE_DIR)

############################################################
# main rules
############################################################

.PHONY: all
all: $(TARGET)

.PHONY: update-all
update-all:
	$(MAKE) COMPILER=CW VERSION=CONSUMER_BUILD
	$(MAKE) COMPILER=CW VERSION=RELEASE
	$(MAKE) COMPILER=CW VERSION=DEBUG
	$(MAKE) COMPILER=SN VERSION=CONSUMER_BUILD
	$(MAKE) COMPILER=SN VERSION=RELEASEEENET
	$(MAKE) COMPILER=SN VERSION=RELEASE
	$(MAKE) COMPILER=SN VERSION=DEBUG

.PHONY: clean
clean:
    %if %dir( $(OBJ_DIR) )
	@-$(RMDIR) $(OBJ_DIR,A\)
	%endif
	%if %file( $(VUCODE_H) )
	@-$(DEL) "$(VUCODE_H,A\)"
	%endif

#	@-$(DEL) /f /q *.a *.d *.d.bak
#	@-$(RMDIR) /s $(INTERMEDIATE_SRC_DIR)

############################################################
# sub rules to get the work done...
############################################################

$(TARGET):  $(OBJ_DIR) $(VUCODE_H) $(MICRO_TABLES) $(OBJS)
   @$(ECHO) Linking $@...
	@$(DEL) "$(TARGET,A\)"
	@$(AR) $(AROUT) $(TARGET) $(AROPTS) $(OBJS)

#$(INTERMEDIATE_SRC_DIR):
#	@$(MKDIR) $(INTERMEDIATE_SRC_DIR)

$(OBJ_DIR):
	@$(MKDIR) $(OBJ_DIR)

$(VUCODE_H) : $(VUPARSE) $(VSM_TEMP)
	@$(ECHO) Making $@
#	@-$(DEL) "$@"
	@$(PERL) $(VUPARSE) process=start_hdr defname=VUCodeDef.h > $@
	@$(PERL) $(VUPARSE) process=add_hdr  >>$@ $(VSM_TEMP)
	@$(PERL) $(VUPARSE) process=finish_hdr >> $@

############################################################
# dependancies
############################################################
DEPPATH=$(OBJ_DIR)/*.d
-include GARBAGE_FILE_THAT_DOESNT_REALLY_EXIST_TO_AVOID_MAKE_ERROR $(DEPPATH,F)

############################################################
# pattern rules
############################################################

$(OBJ_DIR)/%.o : $(INTERMEDIATE_SRC_DIR)/%.cpp
	@$(ECHO) Compiling $<
	@$(CC) $(CCOPTS) -o $@ $<

$(OBJ_DIR)/%.o : $(COMMON_SOURCE_DIR)/%.cpp
	@$(ECHO) Compiling $<
	@$(CC) $(CCOPTS) -o $@ $<
#	@-$(MOVE) $(subst .cpp,.d,$(notdir $<)) $(OBJ_DIR)

$(OBJ_DIR)/%.o : $(CXX_SOURCE_DIR)/%.cpp $(VUCODE_H)
	@$(ECHO) Compiling $<
	@$(CC) $(CCOPTS) -o $@ $<
#	@-$(MOVE) $(subst .cpp,.d,$(notdir $<)) $(OBJ_DIR)

.PRECIOUS: $(INTERMEDIATE_SRC_DIR)/%.vsm
$(OBJ_DIR)/%.obj : $(INTERMEDIATE_SRC_DIR)/%.vsm
	@$(ECHO) Assembling $<
	@$(DVPAS) $(DVP_OPTS) $< -o $@

$(INTERMEDIATE_SRC_DIR)/%MT.cpp : $(VSM_SOURCE_DIR)/%.vm4 $(VUPARSE)
	@$(ECHO) perl Preprocessing  $<
	@$(PERL) $(VUPARSE) process=write_tbl tname=$@ nspace=VUMicro < $< > $@

$(INTERMEDIATE_SRC_DIR)/%.vsm : $(VSM_SOURCE_DIR)/%.vm4 VUDefines.vm4
	@$(ECHO) m4 Preprocessing $<
	@$(M4) $(M4_OPTS) < $< > $@

############################################################
# special dependancies to make everything work
############################################################

