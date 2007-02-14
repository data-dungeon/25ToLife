/*
** relapp.cmd -- linker script for TTL and other avalanche projects
**
**   Important differences from the stock linker script :
**
**	   - .sndata section allows for linking with a DLL
**
**	   - .bss, .sbss, and .spad marked as (NOLOAD).  This prevents
**        the DLL version of the linker from making a 2GB ELF file.
**
**     - .assert section unique to our project, all assert strings
**       don't take space in the executable, and are loaded from the ELF
**       directly when an assert happens.
**
** 9/16/04 Rob Nelson
*/

_stack_size = 0x40000; /* 256k */
_stack = -1;
_heap_size = -1;

GROUP(-lc -lkernl -lgcc)

ENTRY(ENTRYPOINT)

SECTIONS
{
	.text				0x00100000: { crt0.o(.text) *(.text) *(.gnu.linkonce.t*) }
	.reginfo				      : { KEEP(*(.reginfo)) }
	.data				ALIGN(128): { *(.data) *(.vutext) *(.vudata) *(.gnu.linkonce.d*) }
	.sndata				ALIGN(128): { sn_dll_header_root = .; . += 28000; }
	.rodata				ALIGN(128): { *(.rodata) *(.gnu.linkonce.r*) }
	.rdata				ALIGN(128): { *(.rdata) }
	.gcc_except_table	ALIGN(128): { *(.gcc_except_table) }
	
	_gp = ALIGN(128) + 0x7ff0;
	
	.lit8				ALIGN(128): { *(.lit8) }
	.lit4				ALIGN(128): { *(.lit4) }
	.sdata				ALIGN(128): { *(.sdata) *(.gnu.linkonce.s*) }
	
	_fbss = .;
	
	.sbss				ALIGN(128) (NOLOAD): { *(.sbss) *(.scommon) }
	.bss				ALIGN(128) (NOLOAD): { *(.bss) *(.vubss) }
	
	end  = .;
	_end = .;
	
	.assert				ALIGN(128) (NOLOAD): { *(.assert) }

	.spad				0x70000000 (NOLOAD): { *(.spad) }
}
