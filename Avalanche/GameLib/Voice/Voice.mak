########################################################
# PS2 Makefile generated for GNU Make by ProDG VSI.NET #
########################################################

.SILENT:
.SUFFIXES:
.SUFFIXES: .c .cpp .cxx .cc .s .asm .dsm

OUTDIR = SNPS2Debug
INTDIR = SNPS2Debug

LCFILE = "c:/dev/Avalanche/GameLib/Voice/rel.cmd"
LIBS=\
	 "liblgaud.a"\
	 "SaseCodec3200_G0.a"\

AS=ps2cc
AS_FLAGS=\
	-Wa,-g\
	-DSN_TARGET_PS2_DLL\
	-D_DEBUG\
	-DSN\
	-DPS2\
	-DDSKINNER\
	-DNETZ\
	-DUSE_STL\
	-I"C:/Dev/Avalanche/Engine"\
	-I"C:/Dev/Avalanche/Gamelib"\
	-I"C:/Dev/NetZ/PlayStation2/include"\
	-I"C:/Dev/PS2/Logitech"\
	-I"C:/Dev/NellyMoser/lib"\
	-I. -c\
	-fdevstudio\
	-Wa,-sn\
	-g\

DVP=ps2cc
DVP_FLAGS=\
	-Wd,-g\
	-DSN_TARGET_PS2_DLL\
	-D_DEBUG\
	-DSN\
	-DPS2\
	-DDSKINNER\
	-DNETZ\
	-DUSE_STL\
	-I"C:/Dev/Avalanche/Engine"\
	-I"C:/Dev/Avalanche/Gamelib"\
	-I"C:/Dev/NetZ/PlayStation2/include"\
	-I"C:/Dev/PS2/Logitech"\
	-I"C:/Dev/NellyMoser/lib"\
	-I. -fdevstudio -c\
	-g\

CC=ps2cc
CC_FLAGS=\
	-g\
	-DSN_TARGET_PS2_DLL\
	-D_DEBUG\
	-DSN\
	-DPS2\
	-DDSKINNER\
	-DNETZ\
	-DUSE_STL\
	-I"C:/Dev/Avalanche/Engine"\
	-I"C:/Dev/Avalanche/Gamelib"\
	-I"C:/Dev/NetZ/PlayStation2/include"\
	-I"C:/Dev/PS2/Logitech"\
	-I"C:/Dev/NellyMoser/lib"\
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

CXX_FLAGS=$(CC_FLAGS)

LD=ps2dlllk
LD_FLAGS=\
	"c:\dev\Avalanche\GameLib\Voice\rel.lk" ps2cc.exe @_ps2cc.txt

LD_OBJS=\
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

COMPILE: $(LD_OBJS)

"SNPS2Debug/gsm_decode.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Voice/CodecLib/Gsm/gsm_decode.c"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Voice/CodecLib/Gsm/gsm_decode.c" -o $@

"SNPS2Debug/gsm_encode.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Voice/CodecLib/Gsm/gsm_encode.c"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Voice/CodecLib/Gsm/gsm_encode.c" -o $@

"SNPS2Debug/gsm_state.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Voice/CodecLib/Gsm/gsm_state.c"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Voice/CodecLib/Gsm/gsm_state.c" -o $@

"SNPS2Debug/openlpc.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Voice/CodecLib/OpenLPC/openlpc.c"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Voice/CodecLib/OpenLPC/openlpc.c" -o $@

"SNPS2Debug/CTimer.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Voice/NetZ/Ps2/CTimer.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Voice/NetZ/Ps2/CTimer.cpp" -o $@

"SNPS2Debug/GSMCodec.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Voice/NetZ/Ps2/GSMCodec.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Voice/NetZ/Ps2/GSMCodec.cpp" -o $@

"SNPS2Debug/nellymoser_wrapper.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Voice/NetZ/Ps2/nellymoser_wrapper.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Voice/NetZ/Ps2/nellymoser_wrapper.cpp" -o $@

"SNPS2Debug/NellyMoserCodec.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Voice/NetZ/Ps2/NellyMoserCodec.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Voice/NetZ/Ps2/NellyMoserCodec.cpp" -o $@

"SNPS2Debug/OpenLPCCodec.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Voice/NetZ/Ps2/OpenLPCCodec.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Voice/NetZ/Ps2/OpenLPCCodec.cpp" -o $@

"SNPS2Debug/PS2LogitechDevices.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Voice/NetZ/Ps2/PS2LogitechDevices.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Voice/NetZ/Ps2/PS2LogitechDevices.cpp" -o $@

"SNPS2Debug/PS2LogitechSink.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Voice/NetZ/Ps2/PS2LogitechSink.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Voice/NetZ/Ps2/PS2LogitechSink.cpp" -o $@

"SNPS2Debug/PS2LogitechSource.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Voice/NetZ/Ps2/PS2LogitechSource.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Voice/NetZ/Ps2/PS2LogitechSource.cpp" -o $@

"SNPS2Debug/VoiceChat.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Voice/NetZ/Ps2/VoiceChat.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Voice/NetZ/Ps2/VoiceChat.cpp" -o $@

"SNPS2Debug/VoiceChatManager.obj":
	@echo ProDG Compiling "c:/dev/Avalanche/GameLib/Voice/NetZ/Ps2/VoiceChatManager.cpp"
	$(CC) $(CC_FLAGS) "c:/dev/Avalanche/GameLib/Voice/NetZ/Ps2/VoiceChatManager.cpp" -o $@

LINK: "SNPS2Debug/Voice.elf"

"SNPS2Debug/Voice.elf" : $(LD_OBJS)
	@echo ProDG Linking $@
	$(LD) $(LD_FLAGS) $(LD_OBJS) -o $@

include "SNPS2Debug/gsm_decode.d"
include "SNPS2Debug/gsm_encode.d"
include "SNPS2Debug/gsm_state.d"
include "SNPS2Debug/openlpc.d"
include "SNPS2Debug/CTimer.d"
include "SNPS2Debug/GSMCodec.d"
include "SNPS2Debug/NellyMoserCodec.d"
include "SNPS2Debug/OpenLPCCodec.d"
include "SNPS2Debug/PS2LogitechDevices.d"
include "SNPS2Debug/PS2LogitechSink.d"
include "SNPS2Debug/PS2LogitechSource.d"
include "SNPS2Debug/VoiceChat.d"
include "SNPS2Debug/VoiceChatManager.d"

