//Screen states
#define RESOLUTION_SCREEN 0
#define RESOLUTION_SCREEN_HTML "<h1>BrewIt!</h1>Screen Resolutions:<br/>Main Screen:%ix%i<br/>Alt Screen:%ix%i<br/><a href=\"1\">Next</a>",device_info.cxScreen,device_info.cyScreen,device_info.cxAltScreen,device_info.cxAltScreen
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
#define DEV_SCREEN_HTML "Development:<br/>Image Base:$%08x<br/>Dump Range: $%p-$%p<br/><a href=\"-9\">Sub $100000</a><br/><a href=\"-10\">Add $100000</a><br/><a href=\"-8\">Dump to fs:/~/</a><br/><a href=\"-11\">Dump to fs:/card0/</a><br/>Registers!<br/>R13(SP):$%08x<br/>CPSR:$%08x;%s<br/><a href=\"6\">Back</a><br/><a href=\"8\">Next</a>",image_base_address_ram,dump_memory_ptr,dump_memory_ptr+0xfffff,_r13,_cpsr,describe_cpsr()
//With compiler optimizations on, r14 seems to stay blank (atleast when we get a copy of it). Removed it as it is essentially an extra PC and we already sort of know that.
#define COPY_SCREEN 8
//#define COPY_SCREEN_HTML "File Copy<br/><form action=\"c:c\">src<input name=\"src\"></input><br/>dest<input name=\"dest\"></input><input type=\"submit\" value=\"copy\"></input></form><form action=\"e:e\">src<input name=\"src\"></input><br/>dest<input name=\"dest\"></input><input type=\"submit\" value=\"copy dir\"></input></form><a href=\"7\">Back</a><br/><a href=\"9\">Next</a>"
#define COPY_SCREEN_HTML "File Copy<br/><form action=\"c:c\">src<input name=\"src\"></input><br/>dest<input name=\"dest\"></input><input type=\"submit\" value=\"copy\"></input></form><a href=\"-1\">Dump MIFs to fs:/~/</a><br/><a href=\"-2\">Dump MIFs to fs:/card0/</a><br/><a href=\"7\">Back</a><br/><a href=\"9\">Next</a>"
#define LS_SCREEN 9
#define LS_SCREEN_HTML "Directory listing<br/><form action=\"d:d\">Path<input name=\"path\"></input><input type=\"submit\" value=\"list\"></input></form><a href=\"8\">Back</a><br/><a href=\"9\">Next</a>"


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