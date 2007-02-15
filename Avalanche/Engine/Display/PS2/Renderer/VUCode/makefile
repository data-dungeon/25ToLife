
SHELL       = /bin/sh

.SUFFIXES: .vm4 .vsm .cpp
.SYMBOLIC: VUCodeDefDel

PERL=perl
ATTRIB=cmd /c attrib
DEL=cmd /c del
M4_OPTS=-I source
M4=m4
CAT=cat

MICRO_TABLES = AnimBasicMT.cpp \
        AnimLitMT.cpp \
        AnimPLitMT.cpp \
        AnimLitFogCullMT.cpp \
        AnimLitCullMT.cpp \
        AnimLitFogMT.cpp \
        AnimShadowerMT.cpp \
        AnimPLitMPMT.cpp \
        AnimLitMPMT.cpp \
        AnimLitCullMPMT.cpp \
        AnimLitFogMPMT.cpp \
        AnimLitFogCullMPMT.cpp \
		AnimLitCullPLitMT.cpp \
		AnimLitCullPLitMPMT.cpp \
		AnimLitFogCullPLitMT.cpp \
		AnimLitFogCullPLitMPMT.cpp \
        AnimEVMapMT.cpp

ANIM_VSM_FILES = AnimBasic.vsm \
	AnimLit.vsm \
	AnimLitFogCull.vsm \
	AnimLitCull.vsm \
	AnimLitFog.vsm \
	AnimShadower.vsm \
	AnimPLit.vsm \
	AnimEVMap.vsm \
	AnimLitMP.vsm \
	AnimPLitMP.vsm \
	AnimLitCullMP.vsm \
	AnimLitFogMP.vsm \
	AnimLitFogCullMP.vsm \
	AnimLitFogCullPLit.vsm \
	AnimLitFogCullPLitMP.vsm \
	AnimLitCullPLit.vsm \
	AnimLitCullPLitMP.vsm

STATIC_VSM_FILES = DBStaticCompress.vsm \
	DBStaticCompressCull.vsm \
	DBStaticCompressClip.vsm \
	DBStaticCompress2.vsm \
	DBStaticCompress2Cull.vsm \
	DBStaticCompress2Clip.vsm \
	DBStaticCompress2All.vsm \
	DBStaticCompress2AllCull.vsm \
	DBStaticCompress2AllClip.vsm \
	DBStaticCompress2AllClipPlight.vsm \
	DBStaticCompress2VTP.vsm \
	DBStaticCompress2VTPCull.vsm \
	DBStaticCompress2VTPClip.vsm \
	DBStaticCompress2SMC.vsm \
	DBStaticCompress2SMCCull.vsm \
	DBStaticCompress2SMCClip.vsm \
	DBStaticCompress2CullMP.vsm \
	DBStaticCompress2AllCullMP.vsm \
	DBStaticCompress2AllEngCull.vsm \
	DBStaticCompress2AllEngCullMP.vsm \
	DBStaticCompress2AllEngClip.vsm \
	DBStaticCompress2AllCore2Clip.vsm \
	StaticEVMap.vsm

MISC_VSM_FILES = DBDynamic.vsm \
	GenericBufferClip.vsm 

VCS_VSM_FILES = DBVertexColorSetCompress2.vsm \
	DBVertexColorSetCompress2Cull.vsm \
	DBVertexColorSetCompress2Clip.vsm \
	DBVertexColorSetCompress2All.vsm  \
	DBVertexColorSetCompress2AllPLight.vsm  \
	DBVertexColorSetCompress2AllCull.vsm \
	DBVertexColorSetCompress2AllClip.vsm \
	DBVertexColorSetCompress2AllClipPLight.vsm \
	DBVertexColorSetCompress2VTP.vsm \
	DBVertexColorSetCompress2VTPCull.vsm \
	DBVertexColorSetCompress2VTPClip.vsm \
	DBVertexColorSetCompress2SMC.vsm \
	DBVertexColorSetCompress2SMCCull.vsm \
	DBVertexColorSetCompress2SMCClip.vsm \
	DBVertexColorSetCompress2CullMP.vsm \
	DBVertexColorSetCompress2AllCullMP.vsm \
	DBVertexColorSetCompress2AllEngCull.vsm \
	DBVertexColorSetCompress2AllEngCullMP.vsm \
	DBVertexColorSetCompress2AllEngClip.vsm \
	DBVertexColorSetCompress2AllCore2Clip.vsm 

VSM_FILES = $(STATIC_VSM_FILES) $(VCS_VSM_FILES) $(MISC_VSM_FILES) $(ANIM_VSM_FILES)

VSM_SOURCE = $(patsubst %.vsm,source/%.vm4,$(VSM_FILES))

all: $(VSM_FILES) $(MICRO_TABLES) VUCode.h

VUCode.h: source/vuparse.pl $(VSM_FILES) makefile
	-$(ATTRIB) -r $@
	-$(DEL) $@
	$(PERL) source/vuparse.pl process=start_hdr defname=VUCodeDef.h > $@
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ AnimBasic.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ AnimLit.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ AnimLitFogCull.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ AnimLitCull.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ AnimLitFog.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ AnimShadower.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ AnimPLit.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ AnimEVMap.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ AnimLitMP.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ AnimPLitMP.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ AnimLitCullMP.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ AnimLitFogMP.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ AnimLitFogCullMP.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ AnimLitFogCullPLit.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ AnimLitFogCullPLitMP.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ AnimLitCullPLit.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ AnimLitCullPLitMP.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBStaticCompressCull.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBStaticCompressClip.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBStaticCompress2.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBStaticCompress2Cull.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBStaticCompress2Clip.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBStaticCompress2All.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBStaticCompress2AllCull.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBStaticCompress2AllClip.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBStaticCompress2AllClipPlight.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBStaticCompress2VTP.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBStaticCompress2VTPCull.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBStaticCompress2VTPClip.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBStaticCompress2SMC.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBStaticCompress2SMCCull.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBStaticCompress2SMCClip.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBStaticCompress2CullMP.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBStaticCompress2AllCullMP.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBStaticCompress2AllEngCull.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBStaticCompress2AllEngCullMP.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBStaticCompress2AllEngClip.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBStaticCompress2AllCore2Clip.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ StaticEVMap.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ GenericBufferClip.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBVertexColorSetCompress2.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBVertexColorSetCompress2Cull.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBVertexColorSetCompress2Clip.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBVertexColorSetCompress2All.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBVertexColorSetCompress2AllPLight.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBVertexColorSetCompress2AllCull.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBVertexColorSetCompress2AllPLight.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBVertexColorSetCompress2AllClip.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBVertexColorSetCompress2AllClipPLight.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBVertexColorSetCompress2VTP.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBVertexColorSetCompress2VTPCull.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBVertexColorSetCompress2VTPClip.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBVertexColorSetCompress2SMC.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBVertexColorSetCompress2SMCCull.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBVertexColorSetCompress2SMCClip.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBVertexColorSetCompress2CullMP.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBVertexColorSetCompress2AllCullMP.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBVertexColorSetCompress2AllEngCull.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBVertexColorSetCompress2AllEngCullMP.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBVertexColorSetCompress2AllEngClip.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBVertexColorSetCompress2AllCore2Clip.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBDynamic.vsm
	$(PERL) source/vuparse.pl process=add_hdr  >>$@ DBStaticCompress.vsm
	$(PERL) source/vuparse.pl process=finish_hdr >> $@

%.vsm : source/%.vm4 source/VUDefines.vm4
	-$(ATTRIB) -r $@
	$(M4) $(M4_OPTS) < $< > $@

%MT.cpp : source/%.vm4 source/vuparse.pl
	-$(ATTRIB) -r $@
	$(PERL) source/vuparse.pl process=write_tbl tname=$@ nspace=VUMicro < $< > $@

clean:
	$(DEL) $(VSM_FILES) $(MICRO_TABLES) VUCode.h
