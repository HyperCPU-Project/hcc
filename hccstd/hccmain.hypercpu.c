asm(".attr(entry) hcc_start_main:");
asm("mov xsp, 0xfff;");
asm("mov xbp, 0xfff;");
asm("call main;");
asm("halt;");
