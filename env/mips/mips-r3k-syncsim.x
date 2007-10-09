/*GNU ld version 2.9-mipssde-031003 (with BFD 2.9-mipssde-031003)
  Supported emulations:
   elf32algmip
   elf32ebmip
   elf32elmip
   mipsbig
   mipslit
   elf32bsmip
   elf32lsmip
using internal linker script:
==================================================*/
OUTPUT_FORMAT("elf32-bigmips", "elf32-bigmips",
	      "elf32-littlemips")
OUTPUT_ARCH(mips)
ENTRY(__start)
SEARCH_DIR(/usr/local/sde5/sde/lib);
/* Do we need any of these for elf?
   __DYNAMIC = 0;    */

SECTIONS
{
  . = 0x00000000;
  .text : { *(.start); *(.text) }
  . = 0x00002000;
  .data : { *(.data); *(.sdata); *(.scommon); }
  .bss  : { *(.bss); *(.sbss); }
  . = 0x80000080;
  .ktext : { *(.ktext) }
  . = 0x80002000;
  .kdata : { *(.kdata) }

  IO_CTL  = 0xffff0000;
  IO_IN   = 0xffff0004;
  IO_OUT  = 0xffff0008;

  TMR_CTL = 0xffff0010;
  TMR_CNT = 0xffff0014;
  TMR_CMP = 0xffff0018;
}
