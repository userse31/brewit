#include "AEEModGen.h"          // Module interface definitions.
#include "AEEAppGen.h"          // Applet interface definitions.
#include "AEEShell.h"           // Shell interface definitions.  
#include "AEEHtmlViewer.h"
#include "AEEFile.h"
#include "AEEStdLib.h"

#include "CCheckit.h"
#include "Checkit_res.h"
#include "htmldata.h"

//Damn it! Should of thought up "BrewIt!" earlier! Now all the project files have "Checkit" on them...

//Constants for finding base of image (arm).
const unsigned char image_search[]={0x10,0x00,0x00,0xEA,0x11,0x00,0x00,0xEA,0x42,0x52,0x45,0x57,0x02,0x00,0x00,0x00};

typedef struct _Checkit {
    AEEApplet  applet;
    IDisplay * piDisplay;
    IShell   * piShell;
    AEEDeviceInfo  deviceInfo;
} Checkit;

static  boolean Checkit_HandleEvent(Checkit* pMe,AEEEvent eCode,uint16 wParam, uint32 dwParam);
boolean Checkit_InitAppData(Checkit* pMe);
void    Checkit_FreeAppData(Checkit* pMe);
static void Checkit_init_stuff(Checkit * pMe);
static void viewer_callback(void* pvUser, HViewNotify* pNotify);
static unsigned int find_base_addr();
static void dump_memory(int dest);
static int arraycmp_16(unsigned char *x, unsigned char *y);
static void submit_manager(const char *url);
static int chartohex(char x);
static void copy_file(const char *_src,const char *_dest);
static void copy_file_recursive(const char *_src,const char *_dest);
static void dump_mifs(const char *dest_path);
static void kexec_home();
static void kexec_sdcard();
static void attempt_MRC0();
static void attempt_MRC1();
static void attempt_MRC2();
static void attempt_MRC3();
static void attempt_MRC9();
static void attempt_MRC10();
static void attempt_MRC11();
static void attempt_MRC13();

static void memprot_disable();

//Globals
AEEDeviceInfo device_info;
IHtmlViewer *phtmlviewer;
Checkit * pYes;
unsigned int image_base_address_ram=0;
unsigned int dump_memory_ptr=0;
unsigned int _r13=0;//Stack pointer
unsigned int _r14=0;//Link register
unsigned int _cpsr=0;//Current Program Status Register. "SPSR" is "Saved Program Status Register"
//which is only for interrupt modes and stuff for keeping the states of threads constant. We don't
//need to read that.
//R15 is the PC. We don't need that either as we know where the CPU is executing(our program).

//System Control "Coprocessor". There are 16 _primary_ registers (but more permutations, but dear lord ARM worded this part really poorly in the manual!). 32 bits
//ID Codes
unsigned int CR0_MAIN_ID=0;
unsigned int CR0_CACHE_TYPE=0;
unsigned int CR0_TCM_TYPE=0;
unsigned int CR0_TLB_TYPE=0;
unsigned int CR0_MPU_TYPE=0;
//Control Bits
unsigned int CR1_CTRL=0;
unsigned int CR1_AUX_CTRL=0;
unsigned int CR1_COPROC_CTRL=0;
//Page Table Control
unsigned int CR2_TTBR0=0;
unsigned int CR2_TTBR1=0;
unsigned int CR2_TTBC=0;
//Domain Access Control
unsigned int CR3_DAC=0;
//Cache Lockdown
unsigned int CR9_DATA_LOCK=0;
unsigned int CR9_INS_LOCK=0;
//TLB Lockdown
unsigned int CR10_C0_0=0;
unsigned int CR10_C0_1=0;
//DMA
unsigned int CR11_C0_0=0;
//PID
unsigned int CR13_C0_0=0;

//Vectors! (Low, High)
//Reset: $00000000, $FFFF0000
//Undefined: $00000004, $FFFF0004
//Software Interrupt Exception: $00000008, $FFFF0008
//Prefetch Abort: $0000000C, $FFFF000C
//Data Abort: $00000010, $FFFF0010

int AEEClsCreateInstance(AEECLSID ClsId, IShell * piShell, IModule * piModule, void ** ppObj){
    *ppObj = NULL;
    if( AEECLSID_CCHECKIT == ClsId ) {
	    if( TRUE == AEEApplet_New(sizeof(Checkit),ClsId,piShell,piModule,(IApplet**)ppObj,(AEEHANDLER)Checkit_HandleEvent,(PFNFREEAPPDATA)Checkit_FreeAppData)){
		    if(TRUE == Checkit_InitAppData((Checkit*)*ppObj)) {
			    return AEE_SUCCESS;
		    }else{
                IApplet_Release((IApplet*)*ppObj);
                return AEE_EFAILED;
            }
        }
    }
    return AEE_EFAILED;
}

boolean Checkit_InitAppData(Checkit * pMe){
    pMe->piDisplay = pMe->applet.m_pIDisplay;
    pMe->piShell   = pMe->applet.m_pIShell;
    pMe->deviceInfo.wStructSize = sizeof(pMe->deviceInfo);
    ISHELL_GetDeviceInfo(pMe->applet.m_pIShell,&pMe->deviceInfo);
    return TRUE;
}

void Checkit_FreeAppData(Checkit * pMe){
    IHTMLVIEWER_Release(phtmlviewer);
}

static boolean Checkit_HandleEvent(Checkit* pMe,AEEEvent eCode, uint16 wParam, uint32 dwParam){  
	if(phtmlviewer!=NULL){
		if(IHTMLVIEWER_HandleEvent(phtmlviewer,eCode,wParam,dwParam)){
			return TRUE;
		}
	}
    switch (eCode) {
        case EVT_APP_START:
            Checkit_init_stuff(pMe);
            return TRUE;
        case EVT_APP_STOP:
      	    return TRUE;
        case EVT_APP_SUSPEND:
      	    return TRUE;
        case EVT_APP_RESUME:
            Checkit_init_stuff(pMe); 
      	    return TRUE;
        case EVT_APP_MESSAGE:
      	    return TRUE;
        case EVT_KEY:
      	    return TRUE;
        case EVT_FLIP:
            return TRUE;
        case EVT_KEYGUARD:
            return TRUE;
        default:
            break;
    }
    return FALSE;
}

static const char* describe_cpsr(){
	unsigned int tmp=_cpsr&31;
	switch(tmp){//Cpu mode settings
		case 0x10://User
			return "User Mode";
		case 0x11://FIQ Mode
			return "FIQ Mode";
		case 0x12://IRQ
			return "IRQ Mode";
		case 0x13://Supervisor
			return "Supervisor Mode";
		case 0x17://Abort
			return "Abort Mode";
		case 0x1b://Undefined
			return "Undefined Mode";
		case 0x1f://System
			return "System Mode";
		default:
			return "OH NO BRO";
	}
}

static unsigned int bench_trig(){
	double accum=0;
	unsigned int rounds=0;
	unsigned int start_time=ISHELL_GetTimeMS(pYes->piShell);
	for(double x=0;(ISHELL_GetTimeMS(pYes->piShell)-start_time)<1000;x++){
		accum+=FSIN(x);
		rounds++;
	}
	return rounds;
}

static unsigned long bench_longs(){
	unsigned long accum=0;
	unsigned int start_time=ISHELL_GetTimeMS(pYes->piShell);
	while((ISHELL_GetTimeMS(pYes->piShell)-start_time)<1000){
		accum++;
	}
	return accum;
}

static void dump_mifs(const char *dest_path){
	IFileMgr *_ifmp;
	FileInfo tmp;
	char *new_name=(char*)MALLOC(sizeof(char)*32);
	char *new_path=(char*)MALLOC(sizeof(char)*128);
	ISHELL_CreateInstance(pYes->piShell,AEECLSID_FILEMGR,(void **)&_ifmp);
	IFILEMGR_EnumInit(_ifmp,"fs:/mif/",FALSE);
	while(IFILEMGR_EnumNext(_ifmp,&tmp)){
		DBGPRINTF("Dumping %s...",tmp.szName);
		//Get name of the file itself, we need to change the ".mif" extension to prevent reencryption.
		for(int i=8;i<64;i++){
			if(tmp.szName[i]==0||tmp.szName[i]=='.'){
				new_name[i-8]=0;
				break;
			}
			new_name[i-8]=tmp.szName[i];
		}
		DBGPRINTF("%s",new_name);
		SNPRINTF(new_path,128,"%s%s.mie",dest_path,new_name);
		DBGPRINTF("To %s",new_path);
		copy_file(tmp.szName,new_path);
	}
	FREE(new_name);
	FREE(new_path);
	IFILEMGR_Release(_ifmp);
}

static int arraycmp_16(unsigned char *x, unsigned char *y){
	for(int i=0;i<16;i++){
		if(x[i]!=y[i]){
			return FALSE;
		}
	}
	return TRUE;
}

static unsigned int find_base_addr(){
//Microsoft Windows utilizes virtual memory and DLL files are not copied
//from disk to memory in a 1-to-1 manner. Just return zero for x86!
#ifdef AEE_SIMULATOR
return 0;
#endif
	//Qualcomm BREW 3.1.x does not have virtual memory, nor does it do dynamic
	//binary image loading. It just copies the MOD executable into ram!
	unsigned char *tmp=(unsigned char *)AEEClsCreateInstance;
	DBGPRINTF("AEEClsCreateInstance: %p",AEEClsCreateInstance);
	DBGPRINTF("Starting search at %p",tmp);
	for(int i=0;i<0xffff;i++){
		if(arraycmp_16(tmp,(unsigned char*)image_search)){
			DBGPRINTF("Found it! %p",tmp);
			return (unsigned int)tmp;
		}
		tmp--;
	}
	return 0;
}

static void dump_memory(int dest){
	DBGPRINTF("Dumping $%08x",dump_memory_ptr);
	IFileMgr *pfm=0;
	IFile *fp=0;
	ISHELL_CreateInstance(pYes->piShell,AEECLSID_FILEMGR,(void **)&pfm);
	if(pfm==NULL){
		ISHELL_Beep(pYes->piShell,BEEP_ERROR,false);
		return;
	}
	if(dest==0){
	IFILEMGR_Remove(pfm,"fs:/~/snippit.bin");
	fp=IFILEMGR_OpenFile(pfm,"fs:/~/snippit.bin",_OFM_CREATE);
	}
	if(dest==1){
		IFILEMGR_Remove(pfm,"fs:/card0/snippit.bin");
		fp=IFILEMGR_OpenFile(pfm,"fs:/card0/snippit.bin",_OFM_CREATE);
	}
	if(fp==NULL){
		ISHELL_Beep(pYes->piShell,BEEP_ERROR,false);
		return;
	}
	IFILE_Seek(fp,_SEEK_START,0);
	unsigned char *tmp_ptr=(unsigned char*)dump_memory_ptr;
	DBGPRINTF("tmp_ptr: %p",tmp_ptr);
	unsigned int bytes_written=0;
	if(dump_memory_ptr==0){//Special case as IFILE_Write() bails if memory address is 0.
		unsigned char tmp[]={0};
		tmp[0]=tmp_ptr[0];
		bytes_written=IFILE_Write(fp,&tmp,1);
		bytes_written+=IFILE_Write(fp,(const void*)1,0xfffe);
	}else{
		bytes_written=IFILE_Write(fp,tmp_ptr,0xffff);
	}
	//Make a sound to confirm the dump was successful.
	if(bytes_written!=0){
		ISHELL_Beep(pYes->piShell,BEEP_MSG,false);
	}else{
		ISHELL_Beep(pYes->piShell,BEEP_ERROR,false);
	}
	IFILE_Release(fp);
	IFILEMGR_Release(pfm);
}

static void screen_finite_state_machine(int screen_id){
	char *html_buffer=(char*)MALLOC(sizeof(char)*2048);
	switch(screen_id){
		case RESOLUTION_SCREEN:
			SNPRINTF(html_buffer,2048,RESOLUTION_SCREEN_HTML);
			break;
		case GUI_SCREEN:
			SNPRINTF(html_buffer,2048,GUI_SCREEN_HTML);
			break;
		case MISC_SCREEN:
			SNPRINTF(html_buffer,2048,MISC_SCREEN_HTML,ISHELL_GetUpTimeMS(pYes->piShell));
			break;
		case BENCH_SCREEN:
			SNPRINTF(html_buffer,2048,BENCH_SCREEN_HTML);
			break;
		case DO_FSIN_BENCH:
			SNPRINTF(html_buffer,2048,"FSIN():%u/sec<br/><a href=\"3\">Back</a>",bench_trig());
			break;
		case DO_LONG_BENCH:
			SNPRINTF(html_buffer,2048,"Unsigned Longs:%lu/sec<br/><a href=\"3\">Back</a>",bench_longs());
			break;
		case DEV_SCREEN:
			SNPRINTF(html_buffer,2048,DEV_SCREEN_HTML);
			break;
		case BROWSE_SCREEN:
			SNPRINTF(html_buffer,2048,BROWSE_SCREEN_HTML);
			break;
		case COPY_SCREEN:
			SNPRINTF(html_buffer,2048,COPY_SCREEN_HTML);
			break;
		case LS_SCREEN:
			SNPRINTF(html_buffer,2048,LS_SCREEN_HTML);
			break;
		case KEXEC_SCREEN:
			SNPRINTF(html_buffer,2048,KEXEC_SCREEN_HTML);
			break;
		case MRC:
			SNPRINTF(html_buffer,2048,MRC_HTML);
			break;
		//Specials
		case DO_MIFDUMP_HOME:
			dump_mifs("fs:/~/");
			break;
		case DO_MIFDUMP_CARD0:
			dump_mifs("fs:/card0/");
			break;
		case DO_BEEP_REMINDER:
			ISHELL_Beep(pYes->piShell,BEEP_REMINDER,false);
			break;
		case DO_BEEP_MSG:
			ISHELL_Beep(pYes->piShell,BEEP_MSG,false);
			break;
		case DO_BEEP_ERROR:
			ISHELL_Beep(pYes->piShell,BEEP_ERROR,false);
			break;
		case DO_BEEP_VIBRATE_ALERT:
			ISHELL_Beep(pYes->piShell,BEEP_VIBRATE_ALERT,false);
			break;
		case DO_BEEP_VIBRATE_REMIND:
			ISHELL_Beep(pYes->piShell,BEEP_VIBRATE_REMIND,false);
			break;
		case DUMP_MEMORY:
			dump_memory(0);
			break;
		case DUMP_MEMORY_SD:
			dump_memory(1);
			break;
		case SUB_PTR:
			dump_memory_ptr-=0x100000;
			SNPRINTF(html_buffer,2048,DEV_SCREEN_HTML);
			break;
		case ADD_PTR:
			dump_memory_ptr+=0x100000;
			SNPRINTF(html_buffer,2048,DEV_SCREEN_HTML);
			break;
		case KEXEC_APP_HOME:
			kexec_home();
			break;
		case KEXEC_APP_SDCARD:
			kexec_sdcard();
			break;
		case ATTEMPT_MRC0:
			attempt_MRC0();
			SNPRINTF(html_buffer,2048,MRC_HTML);
			break;
		case ATTEMPT_MRC1:
			attempt_MRC1();
			SNPRINTF(html_buffer,2048,MRC_HTML);
			break;
		case ATTEMPT_MRC2:
			attempt_MRC2();
			SNPRINTF(html_buffer,2048,MRC_HTML);
			break;
		case ATTEMPT_MRC3:
			attempt_MRC3();
			SNPRINTF(html_buffer,2048,MRC_HTML);
			break;
		case ATTEMPT_MRC9:
			attempt_MRC9();
			SNPRINTF(html_buffer,2048,MRC_HTML);
			break;
		case ATTEMPT_MRC10:
			attempt_MRC10();
			SNPRINTF(html_buffer,2048,MRC_HTML);
			break;
		case ATTEMPT_MRC11:
			attempt_MRC11();
			SNPRINTF(html_buffer,2048,MRC_HTML);
			break;
		case ATTEMPT_MRC13:
			attempt_MRC13();
			SNPRINTF(html_buffer,2048,MRC_HTML);
			break;
		case ATTEMPT_MEMPROT_DISABLE:
			memprot_disable();
			break;
		default:
			break;
	}
	if(screen_id>=0 || screen_id==SUB_PTR || screen_id==ADD_PTR || screen_id==ATTEMPT_MRC0 || screen_id==ATTEMPT_MRC1 || screen_id==ATTEMPT_MRC2 || screen_id==ATTEMPT_MRC3 || screen_id==ATTEMPT_MRC9 || screen_id==ATTEMPT_MRC10 || screen_id==ATTEMPT_MRC11 || screen_id==ATTEMPT_MRC13){
		IHTMLVIEWER_SetData(phtmlviewer,html_buffer,-1);
	}
	FREE(html_buffer);
}

static int chartohex(char x){
	switch(x){
		case '0':
			return 0;
		case '1':
			return 1;
		case '2':
			return 2;
		case '3':
			return 3;
		case '4':
			return 4;
		case '5':
			return 5;
		case '6':
			return 6;
		case '7':
			return 7;
		case '8':
			return 8;
		case '9':
			return 9;
		case 'a':
		case 'A':
			return 10;
		case 'b':
		case 'B':
			return 11;
		case 'c':
		case 'C':
			return 12;
		case 'd':
		case 'D':
			return 13;
		case 'e':
		case 'E':
			return 14;
		case 'f':
		case 'F':
			return 15;
		default:
			return 0;
	}
}

static void ls_files(const char *_path){
	DBGPRINTF("%s",_path);
	IFileMgr *ifmp;
	FileInfo tmp;
	char *html_buffer=(char*)MALLOC(sizeof(char)*2048);
	SNPRINTF(html_buffer,2048,"<a href=\"9\">Back</a><br/>Folders:");
	ISHELL_CreateInstance(pYes->piShell,AEECLSID_FILEMGR,(void **)&ifmp);
	IFILEMGR_EnumInit(ifmp,_path,TRUE);//Folders
	while(IFILEMGR_EnumNext(ifmp,&tmp)){
		DBGPRINTF("%s",tmp.szName);
		SNPRINTF(html_buffer,2048,"%s<br/>%s",html_buffer,tmp.szName);
	}
	IFILEMGR_EnumInit(ifmp,_path,FALSE);//Files
	SNPRINTF(html_buffer,2048,"%s<br/>Files:",html_buffer);
	while(IFILEMGR_EnumNext(ifmp,&tmp)){
		DBGPRINTF("%s",tmp.szName);
		SNPRINTF(html_buffer,2048,"%s<br/>%s",html_buffer,tmp.szName);
	}
	IHTMLVIEWER_SetData(phtmlviewer,html_buffer,2048);
	FREE(html_buffer);
	IFILEMGR_Release(ifmp);
}

static void copy_file(const char *_src,const char *_dest){
	DBGPRINTF("%s,%s",_src,_dest);
	IFileMgr *ifmp;
	IFile *src;
	IFile *dest;
	ISHELL_CreateInstance(pYes->piShell,AEECLSID_FILEMGR,(void **)&ifmp);
	if(ifmp==NULL){
		ISHELL_Beep(pYes->piShell,BEEP_ERROR,false);
		return;
	}
	src=IFILEMGR_OpenFile(ifmp,_src,_OFM_READ);
	/*if(src==NULL){
		ISHELL_Beep(pYes->piShell,BEEP_ERROR,false);
		return;
	}*/
	dest=IFILEMGR_OpenFile(ifmp,_dest,_OFM_CREATE);
	if(src==NULL || dest==NULL){
		ISHELL_Beep(pYes->piShell,BEEP_ERROR,false);
		return;
	}
	unsigned char tmp[]={0};
	while(IFILE_Read(src,&tmp,sizeof(char))){
		IFILE_Write(dest,&tmp,sizeof(char));
	}
	IFILE_Release(src);
	IFILE_Release(dest);
	IFILEMGR_Release(ifmp);
}

static void copy_file_recursive(const char *_src,const char *_dest){
	return;
}

static void submit_manager(const char *url){
	DBGPRINTF("submit: %s",url);
	if(url[0]=='e'){
		char _start_addr[9];
		_start_addr[8]=0;
		int j=15;
		for(int i=0;i<8;i++){
			if(url[j+i]==0){
				_start_addr[i]=0;
			}
			_start_addr[i]=url[j+i];
		}
		//DBGPRINTF("Thing: %s",_start_addr);
		unsigned int tmp=0;
		tmp=chartohex(_start_addr[0]);
		tmp=(tmp<<4)|chartohex(_start_addr[1]);
		tmp=(tmp<<4)|chartohex(_start_addr[2]);
		tmp=(tmp<<4)|chartohex(_start_addr[3]);
		tmp=(tmp<<4)|chartohex(_start_addr[4]);
		tmp=(tmp<<4)|chartohex(_start_addr[5]);
		tmp=(tmp<<4)|chartohex(_start_addr[6]);
		tmp=(tmp<<4)|chartohex(_start_addr[7]);
		dump_memory_ptr=tmp;
		//DBGPRINTF("dump_memory_ptr:%p",dump_memory_ptr);
	}
	if(url[0]=='d'){
		char _path[128];
		int j=9;
		for(int i=0;i<128;i++){
		switch(url[j]){
			case '+':
				_path[i]=' ';
				j++;
				break;
			case '%'://Why the HELL do... OH! It masks '%' with 'X', doesn't it? Printf stuff eh?
				if(chartohex(url[j+1])=='%'){
					_path[i]='%';
					j+=2;
					break;
				}
				_path[i]=(chartohex(url[j+1])<<4)+chartohex(url[j+2]);
				j+=3;
				break;
			case 0:
			case '&':
				_path[i]=0;
				i=128;
				break;
			default:
				_path[i]=url[j];
				j++;
				break;
			}
		}
		ls_files(_path);
		return;
	}
	if(url[0]=='c'){
		DBGPRINTF("File copy!");
		char _src[128];
		char _dest[128];
		int j=8;//start of "src" portion.
		int k=0;//start of "dest" portion.
		for(int i=8;i<128;i++){
			if(url[i]=='='){
				k=i+1;
				break;
			}
		}
		DBGPRINTF("%i,%i",j,k);
		for(int i=0;i<128;i++){
		switch(url[j]){
			case '+':
				_src[i]=' ';
				j++;
				break;
			case '%'://Why the HELL do... OH! It masks '%' with 'X', doesn't it? Printf stuff eh?
				if(chartohex(url[j+1])=='%'){
					_src[i]='%';
					j+=2;
					break;
				}
				_src[i]=(chartohex(url[j+1])<<4)+chartohex(url[j+2]);
				j+=3;
				break;
			case 0:
			case '&':
				_src[i]=0;
				i=128;
				break;
			default:
				_src[i]=url[j];
				j++;
				break;
			}
		}
		for(int i=0;i<128;i++){
		switch(url[k]){
			case '+':
				_dest[i]=' ';
				k++;
				break;
			case '%'://Why the HELL do... OH! It masks '%' with 'X', doesn't it? Printf stuff eh?
				if(chartohex(url[k+1])=='%'){
					_dest[i]='%';
					k+=2;
					break;
				}
				_dest[i]=(chartohex(url[k+1])<<4)+chartohex(url[k+2]);
				k+=3;
				break;
			case 0:
			case '&':
				_dest[i]=0;
				i=128;
				break;
			default:
				_dest[i]=url[k];
				k++;
				break;
			}
		}
		copy_file(_src,_dest);
		return;
	}
	if(url[0]=='e'){
		DBGPRINTF("File copy!");
		char _src[128];
		char _dest[128];
		int j=8;//start of "src" portion.
		int k=0;//start of "dest" portion.
		for(int i=8;i<128;i++){
			if(url[i]=='='){
				k=i+1;
				break;
			}
		}
		DBGPRINTF("%i,%i",j,k);
		for(int i=0;i<128;i++){
		switch(url[j]){
			case '+':
				_src[i]=' ';
				j++;
				break;
			case '%'://Why the HELL do... OH! It masks '%' with 'X', doesn't it? Printf stuff eh?
				if(chartohex(url[j+1])=='%'){
					_src[i]='%';
					j+=2;
					break;
				}
				_src[i]=(chartohex(url[j+1])<<4)+chartohex(url[j+2]);
				j+=3;
				break;
			case 0:
			case '&':
				_src[i]=0;
				i=128;
				break;
			default:
				_src[i]=url[j];
				j++;
				break;
			}
		}
		for(int i=0;i<128;i++){
		switch(url[k]){
			case '+':
				_dest[i]=' ';
				k++;
				break;
			case '%'://Why the HELL do... OH! It masks '%' with 'X', doesn't it? Printf stuff eh?
				if(chartohex(url[k+1])=='%'){
					_dest[i]='%';
					k+=2;
					break;
				}
				_dest[i]=(chartohex(url[k+1])<<4)+chartohex(url[k+2]);
				k+=3;
				break;
			case 0:
			case '&':
				_dest[i]=0;
				i=128;
				break;
			default:
				_dest[i]=url[k];
				k++;
				break;
			}
		}
		copy_file_recursive(_src,_dest);
		return;
	}
	char _str[128];
	AEECLSID target_classid=0;
	int j=9;
	//Unescape the string.
	if(url[4]!='a'){
	for(int i=0;i<128;i++){
		switch(url[j]){
			case '+':
				_str[i]=' ';
				j++;
				break;
			case '%'://Why the HELL do... OH! It masks '%' with 'X', doesn't it? Printf stuff eh?
				if(chartohex(url[j+1])=='%'){
					_str[i]='%';
					j+=2;
					break;
				}
				_str[i]=(chartohex(url[j+1])<<4)+chartohex(url[j+2]);
				j+=3;
				break;
			case 0:
			case '&':
				_str[i]=0;
				i=128;
				break;
			default:
				_str[i]=url[j];
				j++;
				break;
		}
	}
	DBGPRINTF("%s",_str);
	}else{
		for(int i=0;i<8;i++){
			target_classid=target_classid<<4;
			target_classid+=chartohex(url[i+j]);
		}
		DBGPRINTF("%08x",target_classid);
	}
	switch(url[4]){
		case 'f'://file
			if(!ISHELL_BrowseFile(pYes->piShell,_str)){
				ISHELL_Beep(pYes->piShell,BEEP_ERROR,false);
			}
			break;
		case 'u'://url
			if(!ISHELL_BrowseURL(pYes->piShell,_str)){
				ISHELL_Beep(pYes->piShell,BEEP_ERROR,false);
			}
			break;
		case 'p'://post url
			if(!ISHELL_PostURL(pYes->piShell,_str)){
				ISHELL_Beep(pYes->piShell,BEEP_ERROR,false);
			}
			break;
		case 'a'://Start Applet
			ISHELL_StartApplet(pYes->piShell,target_classid);
			break;
		default:
			DBGPRINTF("Oh no bro. %c",url[4]);
	}
}

static void viewer_callback(void* pvUser, HViewNotify* pNotify){
	switch(pNotify->code){
		case HVN_NONE:
			DBGPRINTF("No event!");
			break;
		case HVN_DONE:
			DBGPRINTF("Done event!");
			IHTMLVIEWER_Redraw(phtmlviewer);
			break;
		case HVN_JUMP:
			DBGPRINTF("Jump event!");
			DBGPRINTF("pszURL: %s",pNotify->u.jump.pszURL);
			screen_finite_state_machine(ATOI(pNotify->u.jump.pszURL));
			IHTMLVIEWER_Redraw(phtmlviewer);
			break;
		case HVN_SUBMIT:
			DBGPRINTF("Submit event!");
			submit_manager(pNotify->u.jump.pszURL);
			break;
		case HVN_FOCUS:
			DBGPRINTF("Focus Event!");
			break;
		case HVN_REDRAW_SCREEN:
			DBGPRINTF("Redraw event!");
			break;
		case HVN_FULLSCREEN_EDIT:
			DBGPRINTF("Fullscreen edit event!");
			break;
		case HVN_INVALIDATE:
			DBGPRINTF("Invalid date event!");
			break;
		case HVN_PAGEDONE:
			DBGPRINTF("Page done event!");
			break;
		case HVN_CONTENTDONE:
			DBGPRINTF("Content done event!");
			break;
		default:
			DBGPRINTF("Not documented!");
	}
}

static void Checkit_init_stuff(Checkit * pMe)
{
	//Copy pMe to pYes so we don't have to pass it as a damn argument in every pissing function call.
	pYes=pMe;
	//Find the base address of the binary image in memory.
	image_base_address_ram=find_base_addr();
	//Arm stuff for registers. An i686 cpu probably won't execute these very well.
#ifndef AEE_SIMULATOR
	asm("mov %0,r13":"=r"(_r13));
	asm("mov %0,r14":"=r"(_r14));
	asm("mrs r0,CPSR");//Get the CPSR, it contains stuff like what mode the CPU is in!
	asm("mov %0,r0":"=r"(_cpsr));
#endif
	DBGPRINTF("CPSR=%08x",_cpsr);
	//Query AMSS for device stats.
	device_info.wStructSize=sizeof(AEEDeviceInfo);
	ISHELL_GetDeviceInfo(pMe->piShell,&device_info);
	IDISPLAY_ClearScreen(pMe->piDisplay);
	AEERect viewer_size;
	viewer_size.x=0;
	viewer_size.y=0;
	viewer_size.dx=device_info.cxScreen;
	viewer_size.dy=device_info.cyScreen;
	ISHELL_CreateInstance(pMe->piShell,AEECLSID_HTML,(void**)&phtmlviewer);
	IHTMLVIEWER_SetRect(phtmlviewer,&viewer_size);
	IHTMLVIEWER_SetActive(phtmlviewer,TRUE);
	screen_finite_state_machine(RESOLUTION_SCREEN);
	IHTMLVIEWER_SetNotifyFn(phtmlviewer,viewer_callback,pMe);
	IHTMLVIEWER_SetProperties(phtmlviewer,HVP_SCROLLBAR);
}

static void kexec_home(){
	IFileMgr *ifmp;
	IFile *image;
	FileInfo pInfo;
	ISHELL_CreateInstance(pYes->piShell,AEECLSID_FILEMGR,(void **)&ifmp);
	if(ifmp==NULL){
		ISHELL_Beep(pYes->piShell,BEEP_ERROR,false);
		return;
	}
	image=IFILEMGR_OpenFile(ifmp,"fs:/~/image.bin",_OFM_READ);
	if(image==NULL){
		ISHELL_Beep(pYes->piShell,BEEP_ERROR,false);
		return;
	}
	IFILE_GetInfo(image,&pInfo);
	unsigned char *tmp=(unsigned char *)MALLOC(sizeof(char)*pInfo.dwSize);
	if(tmp==NULL){
		ISHELL_Beep(pYes->piShell,BEEP_ERROR,false);
		return;
	}

	IFILE_Read(image,tmp,sizeof(char)*pInfo.dwSize);
#ifndef AEE_SIMULATOR
		asm("mov r0,%0"::"r"(tmp));
		asm("push {r0}");
		asm("pop {r15}");
#endif
	IFILE_Release(image);
	IFILEMGR_Release(ifmp);
}

static void kexec_sdcard(){
	IFileMgr *ifmp;
	IFile *image;
	FileInfo pInfo;
	ISHELL_CreateInstance(pYes->piShell,AEECLSID_FILEMGR,(void **)&ifmp);
	if(ifmp==NULL){
		ISHELL_Beep(pYes->piShell,BEEP_ERROR,false);
		return;
	}
	image=IFILEMGR_OpenFile(ifmp,"fs:/card0/image.bin",_OFM_READ);
	if(image==NULL){
		ISHELL_Beep(pYes->piShell,BEEP_ERROR,false);
		return;
	}
	IFILE_GetInfo(image,&pInfo);
	unsigned char *tmp=(unsigned char *)MALLOC(sizeof(char)*pInfo.dwSize);
	if(tmp==NULL){
		ISHELL_Beep(pYes->piShell,BEEP_ERROR,false);
		return;
	}

	IFILE_Read(image,tmp,sizeof(char)*pInfo.dwSize);
#ifndef AEE_SIMULATOR
	asm("mov r0,%0"::"r"(tmp));
	asm("push {r0}");
	asm("pop {r15}");
#endif
	IFILE_Release(image);
	IFILEMGR_Release(ifmp);
}

static void attempt_MRC0(){
	//Read the CP15 registers
#ifndef AEE_SIMULATOR
	//mrc p15, 0 (Arm, why is there just a random zero here?), (destination), CRn, CRm, opcode2
	//ID Codes
	asm("mrc p15, 0, %0, c0, c0, 0":"=r"(CR0_MAIN_ID));
	asm("mrc p15, 0, %0, c0, c0, 1":"=r"(CR0_CACHE_TYPE));
	asm("mrc p15, 0, %0, c0, c0, 2":"=r"(CR0_TCM_TYPE));
	asm("mrc p15, 0, %0, c0, c0, 3":"=r"(CR0_TLB_TYPE));
	asm("mrc p15, 0, %0, c0, c0, 4":"=r"(CR0_MPU_TYPE));
#endif
}
static void attempt_MRC1(){
	//Control Bits
#ifndef AEE_SIMULATOR
	asm("mrc p15, 0, %0, c1, c0, 0":"=r"(CR1_CTRL));
	//The Auxiliary and coprocessor access registers are either nonexistant, or privileged where SUPERVISOR isn't allowed to touch.
	//asm("mrc p15, 0, %0, c1, c0, 1":"=r"(CR1_AUX_CTRL));
	//asm("mrc p15, 0, %0, c1, c0, 2":"=r"(CR1_COPROC_CTRL));
#endif
}
static void attempt_MRC2(){
#ifndef AEE_SIMULATOR
	//Memory protection and control
	asm("mrc p15, 0, %0, c2, c0, 0":"=r"(CR2_TTBR0));
	asm("mrc p15, 0, %0, c2, c0, 1":"=r"(CR2_TTBR1));
	asm("mrc p15, 0, %0, c2, c0, 2":"=r"(CR2_TTBC));
#endif
}
static void attempt_MRC3(){
#ifndef AEE_SIMULATOR
	//Domain Access Control
	asm("mrc p15, 0, %0, c2, c0, 0":"=r"(CR3_DAC));
#endif
}
static void attempt_MRC9(){
#ifndef AEE_SIMULATOR
	asm("mrc p15, 0, %0, c9, c0, 0":"=r"(CR9_DATA_LOCK));
	asm("mrc p15, 0, %0, c9, c0, 1":"=r"(CR9_INS_LOCK));
#endif
}
static void attempt_MRC10(){
#ifndef AEE_SIMULATOR
	asm("mrc p15, 0, %0, c10, c0, 0":"=r"(CR10_C0_0));
	asm("mrc p15, 0, %0, c10, c0, 1":"=r"(CR10_C0_1));
#endif
}
static void attempt_MRC11(){
#ifndef AEE_SIMULATOR
	asm("mrc p15, 0, %0, c11, c0, 0":"=r"(CR11_C0_0));
#endif
}
static void attempt_MRC13(){
#ifndef AEE_SIMULATOR
	asm("mrc p15, 0, %0, c13, c0, 0":"=r"(CR13_C0_0));
#endif
}

static void memprot_disable(){
	unsigned int tmp=0;
#ifndef AEE_SIMULATOR
	asm("mrc p15, 0, %0, c1, c0, 0":"=r"(tmp));
	tmp=tmp&0b11111111111111111111111111111110;
	asm("mcr p15, 0, %0, c1, c0, 0"::"r"(tmp));
	asm("mrc p15, 0, %0, c1, c0, 0":"=r"(CR1_CTRL));
#endif
}