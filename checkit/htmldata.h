//Footer
//<a href=\"0\">Resolutions</a><br/><a href=\"1\">GUI</a><br/><a href=\"2\">Misc</a><br/><a href=\"3\">Benchmark</a><br/><a href=\"6\">ISHELL</a><br/><a href=\"7\">Development</a><br/><a href=\"8\">Copy</a><br/><a href=\"9\">DIR</a><br/><a href=\"10\">KEXEC</a><br/><a href=\"11\">System Control</a>

//Screen states
#define HTML_FOOTER "<br/><a href=\"0\">Resolutions</a><br/><a href=\"1\">GUI</a><br/><a href=\"2\">Misc</a><br/><a href=\"3\">Benchmark</a><br/><a href=\"6\">ISHELL</a><br/><a href=\"7\">Development</a><br/><a href=\"8\">Copy</a><br/><a href=\"9\">DIR</a><br/><a href=\"10\">KEXEC</a><br/><a href=\"11\">System Control</a><br/><a href=\"12\">Function Pointers 0</a><br/><a href=\"13\">Function Pointers 1</a>"

#define RESOLUTION_SCREEN 0
#define RESOLUTION_SCREEN_HTML "<h1>BrewIt!</h1>Screen Resolutions:<br/>Main Screen:%ix%i<br/>Alt Screen:%ix%i<br/>-Things to try-<br/><a href=\"-28\">Disable Control Register Memory Protection</a><br/>" HTML_FOOTER ,device_info.cxScreen,device_info.cyScreen,device_info.cxAltScreen,device_info.cxAltScreen
#define GUI_SCREEN 1
#define GUI_SCREEN_HTML "GUI Info:<br/>Scroll Bar Width:%i<br/>Character Set:$%04x<br/>Auto Scroll Speed:%i(ms)<br/>Color Depth:%i<br/>wMenuImageDelay:%i<br/>" HTML_FOOTER ,device_info.cxScrollBar,device_info.wEncoding,device_info.wMenuTextScroll,device_info.nColorDepth,device_info.wMenuImageDelay
#define MISC_SCREEN 2
//Misspelled "miscellaneous" and it got used in the GUI. Absolutely embarrassing...
#define MISC_SCREEN_HTML "Miscellaneous<br/>Ram:%i<br/>Default Prompt Properties:$%08x<br/>App Closing Keycode:$%04x<br/>Keycode to Close All Apps:$%04x<br/>Active non-sleep:%i(ms)<br/>Max File Path Length:%i<br/>Platform ID:$%08x<br/>" HTML_FOOTER ,device_info.dwRAM,device_info.dwPromptProps,device_info.wKeyCloseApp,device_info.dwSleepDefer,device_info.wMaxPath,device_info.dwPlatformID
#define BENCH_SCREEN 3
//#define BENCH_SCREEN_HTML "Benchmarks:<br/>FSIN():%u/sec<br/>Unsigned Longs:%lu/sec<br/><a href=\"2\">Back</a>",bench_trig(),bench_longs()
#define BENCH_SCREEN_HTML "Benchmarks:<br/><a href=\"4\">FSIN</a><br/><a href=\"5\">Unsigned Longs</a><br/>" HTML_FOOTER

#define DO_FSIN_BENCH 4
#define DO_LONG_BENCH 5
#define BROWSE_SCREEN 6
#define BROWSE_SCREEN_HTML "ISHELL<br/><form action=\"ibx:f\">BrowseFile<br/><input name=\"in\"></input><input type=\"submit\" value=\"GO\"></input></form><form action=\"ibx:u\">BrowseURL<br/><input name=\"in\"></input><input type=\"submit\" value=\"GO\"></input></form><form action=\"ibx:p\">PostURL<br/><input name=\"in\"></input><input type=\"submit\" value=\"GO\"></input></form><form action=\"ibx:a\">StartApplet(hex)<br/><input name=\"in\"></input><input type=\"submit\" value=\"GO\"></input></form><br/>" HTML_FOOTER
#define DEV_SCREEN 7
#define DEV_SCREEN_HTML "Development:<br/>Image Base:$%08x<br/>-Dump Memory-<br/><form action=\"e:e\">Start Address (Hex):<input name=\"start_addr\"></input><br/>Length (Hex):<input name=\"length\"></input><input type=\"submit\" value=\"SET\"></input></form><br/><a href=\"-8\">Dump to fs:/shared/</a><br/><a href=\"-11\">Dump to fs:/card0/</a><br/>Registers!<br/>R13(SP):$%08x<br/>CPSR:$%08x;%s<br/>" HTML_FOOTER ,image_base_address_ram,_r13,_cpsr,describe_cpsr()
//With compiler optimizations on, r14 seems to stay blank (atleast when we get a copy of it). Removed it as it is essentially an extra PC and we already sort of know that.
#define COPY_SCREEN 8
#define COPY_SCREEN_HTML "File Copy<br/><form action=\"c:c\">src<input name=\"src\"></input><br/>dest<input name=\"dest\"></input><input type=\"submit\" value=\"copy\"></input></form><a href=\"-1\">Dump MIFs to fs:/~/</a><br/><a href=\"-2\">Dump MIFs to fs:/card0/</a><br/>" HTML_FOOTER
#define LS_SCREEN 9
#define LS_SCREEN_HTML "Directory listing<br/><form action=\"d:d\">Path<input name=\"path\"></input><input type=\"submit\" value=\"list\"></input></form><br/>" HTML_FOOTER
#define KEXEC_SCREEN 10
#define KEXEC_SCREEN_HTML "\"\"KEXEC\"\"<br/>WARNING: Here be undefined behavior! Make sure to save all work and exit all programs before continuing.<br/><a href=\"-12\">Load fs:/~/image.bin</a><br/><a href=\"-13\">Load fs:/card0/image.bin<br/>" HTML_FOOTER
#define MRC 11
#define MRC_HTML "System Control<br/><a href=\"-14\">Get ID Codes</a><br/>Main:$%08x<br/>Cache Type:$%08x<br/>TCM:$%08x<br/>TLB:$%08x<br/>MPU Type:$%08x<br/><a href=\"-15\">System Config Bits</a><br/>CTRL REG:$%08x<br/><a href=\"-16\">Page Table Control</a><br/>TTBR0:$%08x<br/>TTBR1:$%08x<br/>TTBC:$%08x<br/><a href=\"-17\">Domain Access Control</a><br/>DAC:$%08x<br/><a href=\"-23\">Cache Lockdown</a><br/>UNI/DATA:$%08x<br/>INS:$%08x<br/><a href=\"-24\">TLB Lockdown</a><br/>Data/Uni: $%08x<br/>Instruction: $%08x<br/><a href=\"-25\">DMA Control</a><br/>Present: $%08x<br/><a href=\"-27\">Process ID</a><br/>FCSE: $%08x<br/>" HTML_FOOTER ,CR0_MAIN_ID,CR0_CACHE_TYPE,CR0_TCM_TYPE,CR0_TLB_TYPE,CR0_MPU_TYPE,CR1_CTRL,CR2_TTBR0,CR2_TTBR1,CR2_TTBC,CR3_DAC,CR9_DATA_LOCK,CR9_INS_LOCK,CR10_C0_0,CR10_C0_1,CR11_C0_0,CR13_C0_0
#define FUNCTION_POINTERS_SCREEN_0 12
//():$%p<br/>
#define FUNCTION_POINTERS_SCREEN_0_HTML "Function Pointers 0<br/>memmove():$%p<br/>memset():$%p<br/>memcmp():$%p<br/>memchr():$%p<br/>memstr():$%p<br/>memrchr():$%p<br/>memchrend():$%p<br/>memrchrbegin():$%p<br/>sleep():$%p<br/>strcpy():$%p<br/>strncpy():$%p<br/>strncmp():$%p<br/>stricmp():$%p<br/>strnicmp():$%p<br/>strcat():$%p<br/>strcmp():$%p<br/>strlen():$%p<br/>strchr():$%p<br/>strchrend():$%p<br/>strchrsend():$%p<br/>strrchr():$%p<br/>strstr():$%p<br/>stristr():$%p<br/>strbegins():$%p<br/>aee_stribegins():$%p<br/>strends():$%p<br/>strlower():$%p<br/>strupper():$%p<br/>sprintf():$%p<br/>snprintf():$%p<br/>strtoul():$%p<br/>strtod():$%p<br/>strlcpy():$%p<br/>strlcat():$%p<br/>wstrcpy():$%p<br/>wstrcat():$%p<br/>wstrcmp():$%p<br/>wstrncmp():$%p<br/>wstricmp():$%p<br/>wstrnicmp():$%p<br/>wstrlen():$%p<br/>wstrchr():$%p<br/>wstrrchr():$%p<br/>wsprintf():$%p<br/>strtowstr():$%p<br/>wstrtostr():$%p<br/>wstrtofloat():$%p<br/>floattowstr():$%p<br/>utf8towstr():$%p<br/>wstrtoutf8():$%p<br/>wstrlower():$%p<br/>wstrupper():$%p<br/>wstrlcpy():$%p<br/>wstrlcat():$%p<br/>chartype():$%p<br/>atoi():$%p<br/>wstrcompress():$%p<br/>strexpand():$%p<br/>LocalTimeOffset():$%p<br/>GetAppInstance():$%p<br/>GetALSContext():$%p<br/>qsort():$%p<br/>" HTML_FOOTER ,\
	GET_HELPER()->memmove,GET_HELPER()->memset,GET_HELPER()->memcmp,GET_HELPER()->memchr,GET_HELPER()->memstr,GET_HELPER()->memrchr,GET_HELPER()->memchrend,GET_HELPER()->memrchrbegin,GET_HELPER()->sleep,GET_HELPER()->strcpy,GET_HELPER()->strncpy,GET_HELPER()->strncmp,GET_HELPER()->stricmp,GET_HELPER()->strnicmp,GET_HELPER()->strcat,GET_HELPER()->strcmp,GET_HELPER()->strlen,GET_HELPER()->strchr,GET_HELPER()->strchrend,GET_HELPER()->strchrsend,GET_HELPER()->strrchr,GET_HELPER()->strstr,GET_HELPER()->stristr,GET_HELPER()->strbegins,GET_HELPER()->aee_stribegins,GET_HELPER()->strends,GET_HELPER()->strlower,GET_HELPER()->strupper,GET_HELPER()->sprintf,GET_HELPER()->snprintf,GET_HELPER()->strtoul,GET_HELPER()->strtod,GET_HELPER()->strlcpy,GET_HELPER()->strlcat,GET_HELPER()->wstrcpy,GET_HELPER()->wstrcat,GET_HELPER()->wstrcmp,GET_HELPER()->wstrncmp,GET_HELPER()->wstricmp,GET_HELPER()->wstrnicmp,GET_HELPER()->wstrlen,GET_HELPER()->wstrchr,GET_HELPER()->wstrrchr,GET_HELPER()->wsprintf,GET_HELPER()->strtowstr,GET_HELPER()->wstrtostr,GET_HELPER()->wstrtofloat,GET_HELPER()->floattowstr,GET_HELPER()->utf8towstr,GET_HELPER()->wstrtoutf8,GET_HELPER()->wstrlower,GET_HELPER()->wstrupper,GET_HELPER()->wstrlcpy,GET_HELPER()->wstrlcat,GET_HELPER()->chartype,GET_HELPER()->atoi,GET_HELPER()->wstrcompress,GET_HELPER()->strexpand,GET_HELPER()->LocalTimeOffset,GET_HELPER()->GetAppInstance,GET_HELPER()->GetALSContext,GET_HELPER()->qsort

#define FUNCTION_POINTERS_SCREEN_1 13
#define FUNCTION_POINTERS_SCREEN_1_HTML "Function Pointers 1<br/>vsprintf():$%p<br/>vsnprintf():$%p<br/>SetupNativeImage():$%p<br/>free():$%p<br/>sysfree():$%p<br/>dumpheap():$%p<br/>malloc():$%p<br/>realloc():$%p<br/>wstrdup():$%p<br/>strdup():$%p<br/>dbgheapmark():$%p<br/>lockmem():$%p<br/>unlockmem():$%p<br/>GetFSFree():$%p<br/>GetRAMFree():$%p<br/>wwritelong():$%p<br/>wwritelongex():$%p<br/>wstrsize():$%p<br/>wstrcopyn():$%p<br/>OEMStrSize():$%p<br/>OEMStrLen():$%p<br/>GetAEEVersion():$%p<br/>f_op():$%p<br/>f_cmp():$%p<br/>f_assignstr():$%p<br/>f_assignint():$%p<br/>f_calc():$%p<br/>f_get():$%p<br/>trunc():$%p<br/>utrunc():$%p<br/>err_realloc():$%p<br/>err_strdup():$%p<br/>getlasterror():$%p<br/>dbgprintf():$%p<br/>dbgevent():$%p<br/>GetRand():$%p<br/>GetTimeMS():$%p<br/>GetSeconds():$%p<br/>GetUTCSeconds():$%p<br/>GetUpTimeMS():$%p<br/>GetJulianDate():$%p<br/>JulianToSeconds():$%p<br/>inet_aton():$%p<br/>inet_ntoa():$%p<br/>swaps():$%p<br/>swapl():$%p<br/>setstaticptr():$%p<br/>wgs84_to_degrees():$%p<br/>AEEOS_IsBadPtr():$%p<br/>aee_basename():$%p<br/>aee_makepath():$%p<br/>aee_splitpath():$%p<br/>inet_pton():$%p<br/>inet_ntop():$%p<br/>" HTML_FOOTER ,\
	VSPRINTF,VSNPRINTF,GET_HELPER()->SetupNativeImage,GET_HELPER()->free,GET_HELPER()->sysfree,GET_HELPER()->dumpheap,GET_HELPER()->malloc,GET_HELPER()->realloc,GET_HELPER()->wstrdup,GET_HELPER()->strdup,GET_HELPER()->dbgheapmark,GET_HELPER()->lockmem,GET_HELPER()->unlockmem,GET_HELPER()->GetFSFree,GET_HELPER()->GetRAMFree,GET_HELPER()->wwritelong,GET_HELPER()->wwritelongex,GET_HELPER()->wstrsize,GET_HELPER()->wstrncopyn,GET_HELPER()->OEMStrSize,GET_HELPER()->OEMStrLen,GET_HELPER()->GetAEEVersion,GET_HELPER()->f_op,GET_HELPER()->f_cmp,GET_HELPER()->f_assignstr,GET_HELPER()->f_assignint,GET_HELPER()->f_calc,GET_HELPER()->f_get,GET_HELPER()->trunc,GET_HELPER()->utrunc,GET_HELPER()->err_realloc,GET_HELPER()->err_strdup,GET_HELPER()->getlasterror,GET_HELPER()->dbgprintf,GET_HELPER()->dbgevent,GET_HELPER()->GetRand,GET_HELPER()->GetTimeMS,GET_HELPER()->GetSeconds,GET_HELPER()->GetUTCSeconds,GET_HELPER()->GetUpTimeMS,GET_HELPER()->GetJulianDate,GET_HELPER()->JulianToSeconds,GET_HELPER()->inet_aton,GET_HELPER()->inet_ntoa,GET_HELPER()->swaps,GET_HELPER()->swapl,GET_HELPER()->setstaticptr,GET_HELPER()->wgs84_to_degrees,GET_HELPER()->AEEOS_IsBadPtr,GET_HELPER()->aee_basename,GET_HELPER()->aee_makepath,GET_HELPER()->aee_splitpath,GET_HELPER()->inet_pton,GET_HELPER()->inet_ntop


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