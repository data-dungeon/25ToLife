
MICROCODE_SOURCE =	WaterPS2.vcl WaterPS2-2.vcl
IPATH = ../../../EngineNew/Display/PS2Renderer/VUCode
INCLUDEPATH=-I${IPATH}
VCL_FLAGS = -n -t10 -m -d
OVCL_FLAGS = -n -t10 -m
#GASP_FLAGS = -apsc ';' $(INCLUDEPATH)
GASP_FLAGS = -apc ';' $(INCLUDEPATH)
#GASP_FLAGS = -ac ';'

ENGINE=-Id:/projects/avalanche/engine -Id:/projects/avalanche/gamelib

# if you dont have cygwin installed, may God have pitty on your soul,
# and you should comment out these lines
TIME=c:/cygwin/bin/time
NICE=nice

all: $(patsubst %.vcl, %.dsm, $(MICROCODE_SOURCE))

opt: $(patsubst %.vcl, %.odsm, $(MICROCODE_SOURCE))

clean:
	rm $(patsubst %.vcl, %.dsm, $(MICROCODE_SOURCE)) $(patsubst %.vcl, %.log, $(MICROCODE_SOURCE)) $(patsubst %.vcl, %.gasp, $(MICROCODE_SOURCE))

# .PRECIOUS : %.gasp

%.gasp : %.vcl ${IPATH}/Low.i ${IPATH}/MacroMagic.i ${IPATH}/High.i ${IPATH}/Low.i ${IPATH}/Assert.i ${IPATH}/PsuedoInstr.i
	ee-gasp $(GASP_FLAGS) -o $@ $< > $(patsubst %.gasp, %.log, $@) 2>&1

%.ogasp : %.vcl ${IPATH}/Low.i ${IPATH}/MacroMagic.i ${IPATH}/High.i ${IPATH}/Low.i ${IPATH}/Assert.i ${IPATH}/PsuedoInstr.i
	ee-gasp $(GASP_FLAGS) -o $@ $< > $(patsubst %.ogasp, %.olog, $@) 2>&1

%.odsm : %.ogasp
	-rm $@
	$(NICE) $(TIME) vcl $(OVCL_FLAGS) -o$@ $< >> $(patsubst %.odsm, %.olog, $@) 2>&1

%.dsm : %.gasp
	-rm $@
	$(TIME) vcl $(VCL_FLAGS) -o$@ $< >> $(patsubst %.dsm, %.log, $@) 2>&1


#%.dsm : %.vucpp
#	vectorcvu.exe /outfile $@ /createheader /singledma /noprogress /asm /autoinline 0 /nodefaultinclude /include "C:\Program Files\Codeplay\includeVU" /include "C:\Program Files\Codeplay\includePS2" /include "C:\usr\local\sce\ee\gcc\lib\gcc-lib\ee\3.0.3\include" /include "C:\usr\local\sce\ee\gcc\ee\include" /include "C:\usr\local\sce\ee\gcc\include\g++-2" /include "c:\usr\local\sce\common\include" /include "d:\projects\avalanche\engine" /include "d:\projects\avalanche\gamelib" /include "c:\usr\local\sce\ee\include" /target 1 /cpp /gnu /ansi /statistics /advice $<
#	perl -pibak fixdma.pl $@

