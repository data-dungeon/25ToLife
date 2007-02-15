#
# makefile for microcodes
# converted to opus make
#

.OPTIONS: MacroQuoted
.OPTIONS: TargetQuoted

GASP = "$(SN_PATH)\ee-gcc\bin\ee-gasp.exe"
#VCL  = "D:\dl\sony\vcl14_beta3\vcl14_beta3_win32_release.exe"
VCL  = "c:\tools\vcl.exe"

# -t10 -d
VCL_FLAGS = -n -m -t1
GASP_FLAGS = -apsc ';'
GASP_FLAGS = -ac ';'

MICROCODE_SOURCE = \
	VUStripBuffer1x2CD.vcl \
	VUStripBuffer2x2CD.vcl \
	VUStripBuffer3x2CD.vcl \
	VUStripBuffer4x2CD.vcl \
	VUStripBuffer5x2CD.vcl \
	VUStripBuffer6x2CD.vcl \
	VUStripBuffer7x2CD.vcl \
	VUStripBuffer8x2CD.vcl \
	newparticle.vcl \
	Particulate.vcl

DEPS = \
	Assert.i \
	High.i \
	Low.i \
	MacroMagic.i \
	PsuedoInstr.i \
	Quat.i \
	vcl_sml.i \
	VUStripBufferKernel.i \
	$(GASP) \
	$(VCL)

TARGETS = $(MICROCODE_SOURCE,B,>.vsm)

all:	$(TARGETS,B,>.vsm)

clean:
	-cmd /c del $(TARGETS)
	-cmd /c del $(TARGETS,b,>.log)
	-cmd /c del $(TARGETS,b,>.gasp)

%.vsm : %.vcl
	--$(GASP) $(GASP_FLAGS) -o $(.SOURCE,B,>.gasp) $(.SOURCE) > $(.TARGET,B,>.log) 2>&1
	--$(VCL) $(VCL_FLAGS) -o$(.TARGET) $(.SOURCE,B,>.gasp) >> $(.TARGET,B,>.log) 2>&1

%foreach S $(MICROCODE_SOURCE)
$(S,B,>.vsm) : $(S,B,>.vcl) $(DEPS)
%end
