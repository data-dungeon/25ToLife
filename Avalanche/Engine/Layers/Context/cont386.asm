; CONT386.ASM Context Switch for X86 Processors.
; Need ML assemble ptions /c /Cx /coff
; Custom build info
; ml.exe -Zi -c -coff -Cx -Fo$(IntDir)\$(InputName).obj $(InputDir)\$(InputName).asm
; $(IntDir)\$(InputName).obj

.386
.MODEL flat, C
.CODE

PSTACKBASEOFFSET	EQU		0
PSTACKOFFSET			EQU		4

ctxSwitchToTask PROC pProc:PTR, pSaveProc:PTR



	pushfd
	push	esi
	push	edi
	push	eax
	push	ebx
	push	ecx
	push	edx

	mov		eax, pSaveProc
	mov		[eax + PSTACKOFFSET], esp
	mov		[eax + PSTACKBASEOFFSET], ebp

	mov		eax, pProc
	mov		esp, [eax + PSTACKOFFSET]
	mov		ebp, [eax + PSTACKBASEOFFSET]

	pop		edx
	pop		ecx
	pop		ebx
	pop		eax
	pop		edi
	pop		esi
	popfd


	ret

ctxSwitchToTask ENDP



ctxKillSwitch PROC pProc:PTR



	pushfd
	push	esi
	push	edi
	push	eax
	push	ebx
	push	ecx
	push	edx

	mov		eax, pProc
	mov		esp, [eax + PSTACKOFFSET]
	mov		ebp, [eax + PSTACKBASEOFFSET]

	pop		edx
	pop		ecx
	pop		ebx
	pop		eax
	pop		edi
	pop		esi
	popfd


	ret

ctxKillSwitch ENDP


END