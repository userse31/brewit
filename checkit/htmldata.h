//Screen states
#define RESOLUTION_SCREEN 0
#define RESOLUTION_SCREEN_HTML "<h1>BrewIt!</h1>Screen Resolutions:<br/>Main Screen:%ix%i<br/>Alt Screen:%ix%i<br/>-Things to try-<br/><a href=\"-28\">Disable Control Register Memory Protection</a><br/><a href=\"1\">Next</a>",device_info.cxScreen,device_info.cyScreen,device_info.cxAltScreen,device_info.cxAltScreen
#define GUI_SCREEN 1
#define GUI_SCREEN_HTML "GUI Info:<br/>Scroll Bar Width:%i<br/>Character Set:$%04x<br/>Auto Scroll Speed:%i(ms)<br/>Color Depth:%i<br/>wMenuImageDelay:%i<br/><a href=\"0\">Back</a><br/><a href=\"2\">Next</a>",device_info.cxScrollBar,device_info.wEncoding,device_info.wMenuTextScroll,device_info.nColorDepth,device_info.wMenuImageDelay
#define MISC_SCREEN 2
#define MISC_SCREEN_HTML "Misculanious<br/>Ram:%i<br/>Default Prompt Properties:$%08x<br/>App Closing Keycode:$%04x<br/>Keycode to Close All Apps:$%04x<br/>Active non-sleep:%i(ms)<br/>Max File Path Length:%i<br/>Platform ID:$%08x<br/>Uptime:%i(ms)<br/><a href=\"1\">Back</a><br/><a href=\"3\">Next</a>",device_info.dwRAM,device_info.dwPromptProps,device_info.wKeyCloseApp,device_info.dwSleepDefer,device_info.wMaxPath,device_info.dwPlatformID
#define BENCH_SCREEN 3
//#define BENCH_SCREEN_HTML "Benchmarks:<br/>FSIN():%u/sec<br/>Unsigned Longs:%lu/sec<br/><a href=\"2\">Back</a>",bench_trig(),bench_longs()
#define BENCH_SCREEN_HTML "Benchmarks:<br/><a href=\"4\">FSIN</a><br/><a href=\"5\">Unsigned Longs</a><br/><a href=\"2\">Back</a><br/><a href=\"6\">Next</a>"

#define DO_FSIN_BENCH 4
#define DO_LONG_BENCH 5
#define BROWSE_SCREEN 6
#define BROWSE_SCREEN_HTML "ISHELL<br/><form action=\"ibx:f\">BrowseFile<br/><input name=\"in\"></input><input type=\"submit\" value=\"GO\"></input></form><form action=\"ibx:u\">BrowseURL<br/><input name=\"in\"></input><input type=\"submit\" value=\"GO\"></input></form><form action=\"ibx:p\">PostURL<br/><input name=\"in\"></input><input type=\"submit\" value=\"GO\"></input></form><form action=\"ibx:a\">StartApplet(hex)<br/><input name=\"in\"></input><input type=\"submit\" value=\"GO\"></input></form><a href=\"3\">Back</a><br/><a href=\"7\">Next</a>"
#define DEV_SCREEN 7
#define DEV_SCREEN_HTML "Development:<br/>Image Base:$%08x<br/>-Dump 64KiB of Memory-<br/><form action=\"e:e\">Path<input name=\"start_addr\"></input><input type=\"submit\" value=\"SET\"></input></form><br/><a href=\"-8\">Dump to fs:/~/</a><br/><a href=\"-11\">Dump to fs:/card0/</a><br/>Registers!<br/>R13(SP):$%08x<br/>CPSR:$%08x;%s<br/><a href=\"6\">Back</a><br/><a href=\"8\">Next</a>",image_base_address_ram,_r13,_cpsr,describe_cpsr()
//With compiler optimizations on, r14 seems to stay blank (atleast when we get a copy of it). Removed it as it is essentially an extra PC and we already sort of know that.
#define COPY_SCREEN 8
//#define COPY_SCREEN_HTML "File Copy<br/><form action=\"c:c\">src<input name=\"src\"></input><br/>dest<input name=\"dest\"></input><input type=\"submit\" value=\"copy\"></input></form><form action=\"e:e\">src<input name=\"src\"></input><br/>dest<input name=\"dest\"></input><input type=\"submit\" value=\"copy dir\"></input></form><a href=\"7\">Back</a><br/><a href=\"9\">Next</a>"
#define COPY_SCREEN_HTML "File Copy<br/><form action=\"c:c\">src<input name=\"src\"></input><br/>dest<input name=\"dest\"></input><input type=\"submit\" value=\"copy\"></input></form><a href=\"-1\">Dump MIFs to fs:/~/</a><br/><a href=\"-2\">Dump MIFs to fs:/card0/</a><br/><a href=\"7\">Back</a><br/><a href=\"9\">Next</a>"
#define LS_SCREEN 9
#define LS_SCREEN_HTML "Directory listing<br/><form action=\"d:d\">Path<input name=\"path\"></input><input type=\"submit\" value=\"list\"></input></form><a href=\"8\">Back</a><br/><a href=\"10\">Next</a>"
#define KEXEC_SCREEN 10
#define KEXEC_SCREEN_HTML "\"\"KEXEC\"\"<br/>WARNING: Here be undefined behavior! Make sure to save all work and exit all programs before continuing.<br/><a href=\"-12\">Load fs:/~/image.bin</a><br/><a href=\"-13\">Load fs:/card0/image.bin<br/><a href=\"9\">Back</a><br/><a href=\"11\">Next</a>"
#define MRC 11
#define MRC_HTML "System Control<br/><a href=\"-14\">Get ID Codes</a><br/>Main:$%08x<br/>Cache Type:$%08x<br/>TCM:$%08x<br/>TLB:$%08x<br/>MPU Type:$%08x<br/><a href=\"-15\">System Config Bits</a><br/>CTRL REG:$%08x<br/><a href=\"-16\">Page Table Control</a><br/>TTBR0:$%08x<br/>TTBR1:$%08x<br/>TTBC:$%08x<br/><a href=\"-17\">Domain Access Control</a><br/>DAC:$%08x<br/><a href=\"-23\">Cache Lockdown</a><br/>UNI/DATA:$%08x<br/>INS:$%08x<br/><a href=\"-24\">TLB Lockdown</a><br/>Data/Uni: $%08x<br/>Instruction: $%08x<br/><a href=\"-25\">DMA Control</a><br/>Present: $%08x<br/><a href=\"-27\">Process ID</a><br/>FCSE: $%08x<br/><a href=\"10\">Back</a><br/><a href=\"11\">Next</a>",CR0_MAIN_ID,CR0_CACHE_TYPE,CR0_TCM_TYPE,CR0_TLB_TYPE,CR0_MPU_TYPE,CR1_CTRL,CR2_TTBR0,CR2_TTBR1,CR2_TTBC,CR3_DAC,CR9_DATA_LOCK,CR9_INS_LOCK,CR10_C0_0,CR10_C0_1,CR11_C0_0,CR13_C0_0

//Specials
#define DO_MIFDUMP_HOME -1
#define DO_MIFDUMP_CARD0 -2
#define DO_BEEP_REMINDER -3
#define DO_BEEP_MSG -4
#define DO_BEEP_ERROR -5
#define DO_BEEP_VIBRATE_ALERT -6
#define DO_BEEP_VIBRATE_REMIND -7
#define DUMP_MEMORY -8
#define SUB_PTR -9
#define ADD_PTR -10
#define DUMP_MEMORY_SD -11
#define KEXEC_APP_HOME -12
#define KEXEC_APP_SDCARD -13
#define ATTEMPT_MRC0 -14
#define ATTEMPT_MRC1 -15
#define ATTEMPT_MRC2 -16
#define ATTEMPT_MRC3 -17
#define ATTEMPT_MRC9 -23
#define ATTEMPT_MRC10 -24
#define ATTEMPT_MRC11 -25
#define ATTEMPT_MRC13 -27
#define ATTEMPT_MRC_HTML "Main ID:$%08x<br/><a href=\"0\">Back</a>",CP15_MAIN_ID
#define ATTEMPT_MEMPROT_DISABLE -28