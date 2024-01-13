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

//Globals
AEEDeviceInfo device_info;
IHtmlViewer *phtmlviewer;
Checkit * pYes;

/*=============================================================================
FUNCTION: AEEClsCreateInstance

DESCRIPTION:
    This function is invoked while the app is being loaded. All modules must 
    provide this function. Ensure to retain the same name and parameters for 
    this function. In here, the module must verify the ClassID and then invoke 
    the AEEApplet_New() function that has been provided in AEEAppGen.c. 

    After invoking AEEApplet_New(), this function can do app-specific 
    initialization. In this example, a generic structure is provided so that 
    app developers need not change the app-specific initialization section
    every time, except for a call to IDisplay_InitAppData().
    
    This is done as follows:
    InitAppData() is called to initialize the AppletData instance. It is the
    app developer's responsibility to fill in the app data initialization code
    of InitAppData(). The app developer is also responsible for releasing
    memory allocated for data contained in AppletData. This is done in 
    IDisplay_FreeAppData().

PROTOTYPE:
    int AEEClsCreateInstance(AEECLSID ClsId,
                             IShell * piShell, 
                             IModule * piModule,
                             void ** ppObj)

PARAMETERS:
    ClsId: [in]:
      Specifies the ClassID of the applet which is being loaded.

    piShell [in]:
      Contains pointer to the IShell object. 

    piModule [in]:
      Contains pointer to the IModule object of the current module to which this
      app belongs.

    ppObj [out]: 
      On return, *ppObj must point to a valid IApplet structure. Allocation of
      memory for this structure and initializing the base data members is done
      by AEEApplet_New().

DEPENDENCIES:
    None

RETURN VALUE:
    AEE_SUCCESS:
      If this app needs to be loaded and if AEEApplet_New()invocation was
      successful.
   
   AEE_EFAILED:
     If this app does not need to be loaded or if errors occurred in
     AEEApplet_New(). If this function returns FALSE, this app will not load.

SIDE EFFECTS:
    None
=============================================================================*/
int AEEClsCreateInstance(AEECLSID ClsId, IShell * piShell, IModule * piModule, 
						 void ** ppObj)
{
    *ppObj = NULL;

    // Confirm this applet is the one intended to be created (classID matches):
    if( AEECLSID_CCHECKIT == ClsId ) {
        // Create the applet and make room for the applet structure.
        // NOTE: FreeAppData is called after EVT_APP_STOP is sent to
        //       HandleEvent.
	    if( TRUE == AEEApplet_New(sizeof(Checkit),
                        ClsId,
                        piShell,
                        piModule,
                        (IApplet**)ppObj,
                        (AEEHANDLER)Checkit_HandleEvent,
                        (PFNFREEAPPDATA)Checkit_FreeAppData) ) {
                     		
            // Initialize applet data. This is called before EVT_APP_START is
            // sent to the HandleEvent function.
		    if(TRUE == Checkit_InitAppData((Checkit*)*ppObj)) {
			    return AEE_SUCCESS; // Data initialized successfully.
		    }
		    else {
                // Release the applet. This will free the memory allocated for
                // the applet when AEEApplet_New was called.
                IApplet_Release((IApplet*)*ppObj);
                return AEE_EFAILED;
            }
        } // End AEEApplet_New
    }
    return AEE_EFAILED;
}


/*=============================================================================
FUNCTION: Checkit_InitAppData

DESCRIPTION:
    This function is called when the application is starting up, so the 
	initialization and resource allocation code is executed here.

PROTOTYPE:
    boolean Checkit_InitAppData(Checkit * pMe)

PARAMETERS:
    pMe [in]:
      Pointer to the AEEApplet structure. This structure contains information
      specific to this applet. It was initialized in AEEClsCreateInstance().
  
DEPENDENCIES:
    None

RETURN VALUE:
    TRUE:
      If there were no failures.

SIDE EFFECTS:
    None
=============================================================================*/
boolean Checkit_InitAppData(Checkit * pMe)
{
    // Save local copy for easy access:
    pMe->piDisplay = pMe->applet.m_pIDisplay;
    pMe->piShell   = pMe->applet.m_pIShell;

    // Get the device information for this handset.
    // Reference all the data by looking at the pMe->deviceInfo structure.
    // Check the API reference guide for all the handy device info you can get.
    pMe->deviceInfo.wStructSize = sizeof(pMe->deviceInfo);
    ISHELL_GetDeviceInfo(pMe->applet.m_pIShell,&pMe->deviceInfo);
    
    // Insert your code here for initializing or allocating resources...

    return TRUE;// No failures up to this point, so return success.
}


void Checkit_FreeAppData(Checkit * pMe)
{
    IHTMLVIEWER_Release(phtmlviewer);
}

static boolean Checkit_HandleEvent(Checkit* pMe,AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
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
      	    return FALSE;
        case EVT_FLIP:
            return TRUE;
        case EVT_KEYGUARD:
            return TRUE;
        default:
            break;
    }
    return FALSE; // Event wasn't handled.
}

unsigned int bench_trig(){
	double accum=0;
	unsigned int rounds=0;
	unsigned int start_time=ISHELL_GetTimeMS(pYes->piShell);
	for(double x=0;(ISHELL_GetTimeMS(pYes->piShell)-start_time)<1000;x++){
		accum+=FSIN(x);
		rounds++;
	}
	return rounds;
}

unsigned long bench_longs(){
	unsigned long accum=0;
	unsigned int start_time=ISHELL_GetTimeMS(pYes->piShell);
	while((ISHELL_GetTimeMS(pYes->piShell)-start_time)<1000){
		accum++;
	}
	return accum;
}

static void screen_finite_state_machine(int screen_id){
	char *html_buffer=(char*)MALLOC(sizeof(char)*512);
	switch(screen_id){
		case RESOLUTION_SCREEN:
			SNPRINTF(html_buffer,512,RESOLUTION_SCREEN_HTML);
			break;
		case GUI_SCREEN:
			SNPRINTF(html_buffer,512,GUI_SCREEN_HTML);
			break;
		case MISC_SCREEN:
			SNPRINTF(html_buffer,512,MISC_SCREEN_HTML,ISHELL_GetUpTimeMS(pYes->piShell));
			break;
		case BENCH_SCREEN:
			SNPRINTF(html_buffer,512,BENCH_SCREEN_HTML);
		default:
			break;
	}
	IHTMLVIEWER_SetData(phtmlviewer,html_buffer,-1);
	FREE(html_buffer);
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
