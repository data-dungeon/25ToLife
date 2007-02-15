
MICROCODE_SOURCE =	TrigMicro.vcl QuaternMicro.vcl CollideMicro.vcl FrustumMicro.vcl occluder.vcl

INCLUDEPATH=-I../../Display/PS2/VUCode
#VCL_FLAGS = -n -t1
VCL_FLAGS = -n -d
#GASP_FLAGS = -apsc ';' $(INCLUDEPATH)
GASP_FLAGS = -ac ';' $(INCLUDEPATH)

all: $(patsubst %.vcl, %.vsm, $(MICROCODE_SOURCE))

clean:
	del $(patsubst %.vcl, %.vsm, $(MICROCODE_SOURCE)) $(patsubst %.vcl, %.log, $(MICROCODE_SOURCE)) $(patsubst %.vcl, %.gasp, $(MICROCODE_SOURCE))

# .PRECIOUS : %.gasp


%.gasp : %.vcl
	ee-gasp $(GASP_FLAGS) -o $@ $< > $(patsubst %.gasp, %.log, $@) 2>&1

%.vsm : %.gasp
	-del $@
	vcl $(VCL_FLAGS) -o$@ $< >> $(patsubst %.vsm, %.log, $@) 2>&1
