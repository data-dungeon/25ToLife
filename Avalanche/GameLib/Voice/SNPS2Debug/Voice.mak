####################################################
# PS2 Makefile generated for GNU Make by ProDG VSI #
####################################################

.SILENT:
.SUFFIXES:
.SUFFIXES: .c .cpp .cxx .cc .s .asm .dsm

OUTDIR = SNPS2Debug
INTDIR = SNPS2Debug

LCFILE = "d:/Ttl/Avalanche/GameLib/Voice/rel.cmd"
LIBS=\
	 "liblgaud.a"\
	 "SaseCodec3200_G0_v163b.a"\

AS=ps2cc
AS_FLAGS=\
	-DSN_TARGET_PS2_DLL\
	-D_DEBUG\
	-DSN\
	-DPS2\
	-DJJS\
	-DUSE_STL\
	-I"d:/ttl/avalanche/Engine"\
	-I"d:/ttl/avalanche/Gamelib"\
	-I"d:/ttl/NetZ/PlayStation2/include"\
	-I"d:/ttl/PS2/Logitech"\
	-I"d:/ttl/NellyMoser/lib"\
	-I. -c\
	-fdevstudio\
	-Wa,-sn\
	-g\

DVP=ps2cc
DVP_FLAGS=\
	-DSN_TARGET_PS2_DLL\
	-D_DEBUG\
	-DSN\
	-DPS2\
	-DJJS\
	-DUSE_STL\
	-I"d:/ttl/avalanche/Engine"\
	-I"d:/ttl/avalanche/Gamelib"\
	-I"d:/ttl/NetZ/PlayStation2/include"\
	-I"d:/ttl/PS2/Logitech"\
	-I"d:/ttl/NellyMoser/lib"\
	-I. -fdevstudio -c\
	-g\

CC=ps2cc
CC_FLAGS=\
	-DSN_TARGET_PS2_DLL\
	-D_DEBUG\
	-DSN\
	-DPS2\
	-DJJS\
	-DUSE_STL\
	-I"d:/ttl/avalanche/Engine"\
	-I"d:/ttl/avalanche/Gamelib"\
	-I"d:/ttl/NetZ/PlayStation2/include"\
	-I"d:/ttl/PS2/Logitech"\
	-I"d:/ttl/NellyMoser/lib"\
	-I. -c\
	-fdevstudio\
	-MMD\
	-Wno-inline-static-semantics\
	-Wa,-sn\
	-fno-exceptions\
	-mno-check-zero-division\
	-ffast-math\
	-mvu0-use-vf0-vf15\
	-g\
	-G0\

CXX=ps2cc
CXX_FLAGS=\
	-DSN_TARGET_PS2_DLL\
	-D_DEBUG\
	-DSN\
	-DPS2\
	-DJJS\
	-DUSE_STL\
	-I"d:/ttl/avalanche/Engine"\
	-I"d:/ttl/avalanche/Gamelib"\
	-I"d:/ttl/NetZ/PlayStation2/include"\
	-I"d:/ttl/PS2/Logitech"\
	-I"d:/ttl/NellyMoser/lib"\
	-I. -c\
	-fdevstudio\
	-MMD\
	-Wno-inline-static-semantics\
	-Wa,-sn\
	-fno-rtti\
	-fno-exceptions\
	-mno-check-zero-division\
	-ffast-math\
	-mvu0-use-vf0-vf15\
	-g\
	-G0\

LD=ps2dlllk
LD_FLAGS=\
	"d:\Ttl\Avalanche\GameLib\Voice\rel.lk" ps2cc.exe @"$(INTDIR)/_ps2cc.txt"


OBJS=\
	"SNPS2Debug/gsm_decode.obj"\
	"SNPS2Debug/gsm_encode.obj"\
	"SNPS2Debug/gsm_state.obj"\
	"SNPS2Debug/openlpc.obj"\
	"SNPS2Debug/CTimer.obj"\
	"SNPS2Debug/GSMCodec.obj"\
	"SNPS2Debug/nellymoser_wrapper.obj"\
	"SNPS2Debug/NellyMoserCodec.obj"\
	"SNPS2Debug/OpenLPCCodec.obj"\
	"SNPS2Debug/PS2LogitechDevices.obj"\
	"SNPS2Debug/PS2LogitechSink.obj"\
	"SNPS2Debug/PS2LogitechSource.obj"\
	"SNPS2Debug/VoiceChat.obj"\
	"SNPS2Debug/VoiceChatManager.obj"\

COMPILE: $(OBJS)

"SNPS2Debug/gsm_decode.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/Voice/CodecLib/Gsm/gsm_decode.c"
	$(CC) $(CC_FLAGS) "d:/Ttl/Avalanche/GameLib/Voice/CodecLib/Gsm/gsm_decode.c" -o $@

"SNPS2Debug/gsm_encode.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/Voice/CodecLib/Gsm/gsm_encode.c"
	$(CC) $(CC_FLAGS) "d:/Ttl/Avalanche/GameLib/Voice/CodecLib/Gsm/gsm_encode.c" -o $@

"SNPS2Debug/gsm_state.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/Voice/CodecLib/Gsm/gsm_state.c"
	$(CC) $(CC_FLAGS) "d:/Ttl/Avalanche/GameLib/Voice/CodecLib/Gsm/gsm_state.c" -o $@

"SNPS2Debug/openlpc.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/Voice/CodecLib/OpenLPC/openlpc.c"
	$(CC) $(CC_FLAGS) "d:/Ttl/Avalanche/GameLib/Voice/CodecLib/OpenLPC/openlpc.c" -o $@

"SNPS2Debug/CTimer.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/CTimer.cpp"
	$(CXX) $(CXX_FLAGS) "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/CTimer.cpp" -o $@

"SNPS2Debug/GSMCodec.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/GSMCodec.cpp"
	$(CXX) $(CXX_FLAGS) "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/GSMCodec.cpp" -o $@

"SNPS2Debug/nellymoser_wrapper.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/nellymoser_wrapper.cpp"
	$(CXX) $(CXX_FLAGS) "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/nellymoser_wrapper.cpp" -o $@

"SNPS2Debug/NellyMoserCodec.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/NellyMoserCodec.cpp"
	$(CXX) $(CXX_FLAGS) "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/NellyMoserCodec.cpp" -o $@

"SNPS2Debug/OpenLPCCodec.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/OpenLPCCodec.cpp"
	$(CXX) $(CXX_FLAGS) "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/OpenLPCCodec.cpp" -o $@

"SNPS2Debug/PS2LogitechDevices.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/PS2LogitechDevices.cpp"
	$(CXX) $(CXX_FLAGS) "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/PS2LogitechDevices.cpp" -o $@

"SNPS2Debug/PS2LogitechSink.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/PS2LogitechSink.cpp"
	$(CXX) $(CXX_FLAGS) "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/PS2LogitechSink.cpp" -o $@

"SNPS2Debug/PS2LogitechSource.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/PS2LogitechSource.cpp"
	$(CXX) $(CXX_FLAGS) "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/PS2LogitechSource.cpp" -o $@

"SNPS2Debug/VoiceChat.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/VoiceChat.cpp"
	$(CXX) $(CXX_FLAGS) "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/VoiceChat.cpp" -o $@

"SNPS2Debug/VoiceChatManager.obj":
	@echo ProDG Compiling "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/VoiceChatManager.cpp"
	$(CXX) $(CXX_FLAGS) "d:/Ttl/Avalanche/GameLib/Voice/NetZ/Ps2/VoiceChatManager.cpp" -o $@

LINK: "SNPS2Debug/Voice.elf"

"SNPS2Debug/Voice.elf" : $(OBJS) $(LCFILE)
	@echo ProDG Linking $@
	$(LD) $(LD_FLAGS) --start-group $(OBJS) $(LIBS) --end-group -o $@

