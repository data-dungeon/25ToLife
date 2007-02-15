####################################################
# PS2 Makefile generated for GNU Make by ProDG VSI #
####################################################

.SILENT:
.SUFFIXES:
.SUFFIXES: .c .cpp .cxx .cc .s .asm .dsm

OUTDIR = SNPS2Consumer
INTDIR = SNPS2Consumer

LCFILE = "d:/Ttl/Avalanche/GameLib/Voice/rel.cmd"
LIBS=\
	 "liblgaud.a"\
	 "SaseCodec3200_G0_v163b.a"\

AS=ps2cc
AS_FLAGS=\
	-DSN_TARGET_PS2_DLL\
	-DSN\
	-DPS2\
	-DJJS\
	-DNETZ\
	-DCDROM\
	-DUSE_STL\
	-DCONSUMER_BUILD\
	-I"d:/ttl/avalanche/Engine"\
	-I"d:/ttl/avalanche/Gamelib"\
	-I"d:/ttl/NetZ/PlayStation2/include"\
	-I"d:/ttl/PS2/Logitech"\
	-I"d:/ttl/NellyMoser/Lib"\
	-I. -c\
	-fdevstudio\
	-g\
	-Wa,-sn\

DVP=ps2cc
DVP_FLAGS=\
	-DSN_TARGET_PS2_DLL\
	-DSN\
	-DPS2\
	-DJJS\
	-DNETZ\
	-DCDROM\
	-DUSE_STL\
	-DCONSUMER_BUILD\
	-I"d:/ttl/avalanche/Engine"\
	-I"d:/ttl/avalanche/Gamelib"\
	-I"d:/ttl/NetZ/PlayStation2/include"\
	-I"d:/ttl/PS2/Logitech"\
	-I"d:/ttl/NellyMoser/Lib"\
	-I. -fdevstudio -c\
	-g\

CC=ps2cc
CC_FLAGS=\
	-DSN_TARGET_PS2_DLL\
	-DSN\
	-DPS2\
	-DJJS\
	-DNETZ\
	-DCDROM\
	-DUSE_STL\
	-DCONSUMER_BUILD\
	-I"d:/ttl/avalanche/Engine"\
	-I"d:/ttl/avalanche/Gamelib"\
	-I"d:/ttl/NetZ/PlayStation2/include"\
	-I"d:/ttl/PS2/Logitech"\
	-I"d:/ttl/NellyMoser/Lib"\
	-I. -c\
	-fdevstudio\
	-MMD\
	-g\
	-G0\
	-fno-exceptions\
	-O2\
	-Wno-inline-static-semantics\
	-Wa,-sn\
	-mno-check-zero-division\
	-ffast-math\
	-mvu0-use-vf0-vf15\

CXX=ps2cc
CXX_FLAGS=\
	-DSN_TARGET_PS2_DLL\
	-DSN\
	-DPS2\
	-DJJS\
	-DNETZ\
	-DCDROM\
	-DUSE_STL\
	-DCONSUMER_BUILD\
	-I"d:/ttl/avalanche/Engine"\
	-I"d:/ttl/avalanche/Gamelib"\
	-I"d:/ttl/NetZ/PlayStation2/include"\
	-I"d:/ttl/PS2/Logitech"\
	-I"d:/ttl/NellyMoser/Lib"\
	-I. -c\
	-fdevstudio\
	-MMD\
	-g\
	-G0\
	-fno-exceptions\
	-O2\
	-Wno-inline-static-semantics\
	-Wa,-sn\
	-fno-rtti\
	-mno-check-zero-division\
	-ffast-math\
	-mvu0-use-vf0-vf15\

LD=ps2dlllk
LD_FLAGS=\
	"d:\Ttl\Avalanche\GameLib\Voice\rel.lk" ps2cc.exe @"$(INTDIR)/_ps2cc.txt"


OBJS=\
	"SNPS2Consumer/gsm_decode.obj"\
	"SNPS2Consumer/gsm_encode.obj"\
	"SNPS2Consumer/gsm_state.obj"\
	"SNPS2Consumer/openlpc.obj"\
	"SNPS2Consumer/CTimer.obj"\
	"SNPS2Consumer/GSMCodec.obj"\
	"SNPS2Consumer/nellymoser_wrapper.obj"\
	"SNPS2Consumer/NellyMoserCodec.obj"\
	"SNPS2Consumer/OpenLPCCodec.obj"\
	"SNPS2Consumer/PS2LogitechDevices.obj"\
	"SNPS2Consumer/PS2LogitechSink.obj"\
	"SNPS2Consumer/PS2LogitechSource.obj"\
	"SNPS2Consumer/VoiceChat.obj"\
	"SNPS2Consumer/VoiceChatManager.obj"\

COMPILE: $(OBJS)

"SNPS2Consumer/gsm_decode.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/Voice/CodecLib/Gsm/gsm_decode.c"
	$(CC) $(CC_FLAGS) "d:/Ttl/Avalanche/GameLib/Voice/CodecLib/Gsm/gsm_decode.c" -o $@

"SNPS2Consumer/gsm_encode.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/Voice/CodecLib/Gsm/gsm_encode.c"
	$(CC) $(CC_FLAGS) "d:/Ttl/Avalanche/GameLib/Voice/CodecLib/Gsm/gsm_encode.c" -o $@

"SNPS2Consumer/gsm_state.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/Voice/CodecLib/Gsm/gsm_state.c"
	$(CC) $(CC_FLAGS) "d:/Ttl/Avalanche/GameLib/Voice/CodecLib/Gsm/gsm_state.c" -o $@

"SNPS2Consumer/openlpc.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/Voice/CodecLib/OpenLPC/openlpc.c"
	$(CC) $(CC_FLAGS) "d:/Ttl/Avalanche/GameLib/Voice/CodecLib/OpenLPC/openlpc.c" -o $@

"SNPS2Consumer/CTimer.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/CTimer.cpp"
	$(CXX) $(CXX_FLAGS) "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/CTimer.cpp" -o $@

"SNPS2Consumer/GSMCodec.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/GSMCodec.cpp"
	$(CXX) $(CXX_FLAGS) "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/GSMCodec.cpp" -o $@

"SNPS2Consumer/nellymoser_wrapper.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/nellymoser_wrapper.cpp"
	$(CXX) $(CXX_FLAGS) "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/nellymoser_wrapper.cpp" -o $@

"SNPS2Consumer/NellyMoserCodec.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/NellyMoserCodec.cpp"
	$(CXX) $(CXX_FLAGS) "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/NellyMoserCodec.cpp" -o $@

"SNPS2Consumer/OpenLPCCodec.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/OpenLPCCodec.cpp"
	$(CXX) $(CXX_FLAGS) "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/OpenLPCCodec.cpp" -o $@

"SNPS2Consumer/PS2LogitechDevices.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/PS2LogitechDevices.cpp"
	$(CXX) $(CXX_FLAGS) "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/PS2LogitechDevices.cpp" -o $@

"SNPS2Consumer/PS2LogitechSink.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/PS2LogitechSink.cpp"
	$(CXX) $(CXX_FLAGS) "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/PS2LogitechSink.cpp" -o $@

"SNPS2Consumer/PS2LogitechSource.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/PS2LogitechSource.cpp"
	$(CXX) $(CXX_FLAGS) "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/PS2LogitechSource.cpp" -o $@

"SNPS2Consumer/VoiceChat.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/VoiceChat.cpp"
	$(CXX) $(CXX_FLAGS) "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/VoiceChat.cpp" -o $@

"SNPS2Consumer/VoiceChatManager.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/VoiceChatManager.cpp"
	$(CXX) $(CXX_FLAGS) "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/VoiceChatManager.cpp" -o $@

LINK: "SNPS2Consumer/Voice.elf"

"SNPS2Consumer/Voice.elf" : $(OBJS) $(LCFILE)
	@echo ProDG Linking $@
	$(LD) $(LD_FLAGS) --start-group $(OBJS) $(LIBS) --end-group -o $@

