//Screen states
#define RESOLUTION_SCREEN 0
#define RESOLUTION_SCREEN_HTML "<h1>BrewIt!</h1>Screen Resolutions:<br/>Main Screen:%ix%i<br/>Alt Screen:%ix%i<br/><a href=\"1\">Next</a>",device_info.cxScreen,device_info.cyScreen,device_info.cxAltScreen,device_info.cxAltScreen
#define GUI_SCREEN 1
#define GUI_SCREEN_HTML "GUI Info:<br/>Scroll Bar Width:%i<br/>Character Set:$%04x<br/>Auto Scroll Speed:%i(ms)<br/>Color Depth:%i<br/>wMenuImageDelay:%i<br/><a href=\"0\">Back</a><br/><a href=\"2\">Next</a>",device_info.cxScrollBar,device_info.wEncoding,device_info.wMenuTextScroll,device_info.nColorDepth,device_info.wMenuImageDelay
#define MISC_SCREEN 2
#define MISC_SCREEN_HTML "Misculanious<br/>Ram:%i<br/>Default Prompt Properties:$%08x<br/>App Closing Keycode:$%04x<br/>Keycode to Close All Apps:$%04x<br/>Active non-sleep:%i(ms)<br/>Max File Path Length:%i<br/>Platform ID:$%08x<br/>Uptime:%i(ms)<br/><a href=\"1\">Back</a><br/><a href=\"3\">Next</a>",device_info.dwRAM,device_info.dwPromptProps,device_info.wKeyCloseApp,device_info.dwSleepDefer,device_info.wMaxPath,device_info.dwPlatformID
#define BENCH_SCREEN 3
#define BENCH_SCREEN_HTML "Benchmarks:<br/>FSIN():%u/sec<br/>Unsigned Longs:%lu/sec<br/><a href=\"2\">Back</a>",bench_trig(),bench_longs