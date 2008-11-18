#include "nui.h"

using namespace std;

#include "nglApplication.h"
#include "nglUIApplication.h"

#include "../../Window/UIKit/nglUIWindow.h"
#include "nglWindow.h"

/*
** nglUIApplication
*/

void objCCallOnWillExit();

@implementation nglUIApplication

- (void) dealloc
{
//  NGL_OUT(_T("[nglUIApplication dealloc]\n"));
  [super dealloc];
}

- (BOOL) openURL: (NSURL*) pUrl
{
  //NGL_OUT(_T("[nglUIApplication openURL]\n"));
  return [super openURL: pUrl];
}

- (void) sendEvent: (UIEvent*) pEvent
{
//NGL_DEBUG( NGL_OUT(_T("[nglUIApplication sendEvent]\n")) );
  [super sendEvent: pEvent];
}

@end///< nglUIApplication


/*
** nglUIApplicationDelegate
*/
@implementation nglUIApplicationDelegate

- (void) dealloc
{
  //NGL_OUT(_T("[nglUIApplicationDelegate dealloc]\n"));
  [super dealloc];
}

- (void) applicationDidFinishLaunching:       (UIApplication*) pUIApplication
{
  //NGL_OUT(_T("[nglUIApplicationDelegate applicationDidFinishLaunching]\n"));
  NGL_ASSERT(App);

  objCCallOnInit(pUIApplication);
}

- (void) applicationDidBecomeActive:          (UIApplication*) pUIApplication
{
  NGL_DEBUG( NGL_OUT(_T("[nglUIApplicationDelegate applicationDidBecomeActive]\n")); )
  NGL_ASSERT(App);

  id win = [pUIApplication keyWindow];
  NGL_ASSERT(win);
  nglWindow* pWindow = [win getNGLWindow];
  NGL_ASSERT(pWindow);
  pWindow->CallOnActivation();
}

- (void) applicationWillResignActive:         (UIApplication*) pUIApplication
{
  NGL_DEBUG( NGL_OUT(_T("[nglUIApplicationDelegate applicationWillResignActive]\n")); )
  NGL_ASSERT(App);
  
  nglUIWindow* win = (nglUIWindow*)[pUIApplication keyWindow];
  NGL_ASSERT(win);
  nglWindow* pWindow = [win getNGLWindow];
  NGL_ASSERT(pWindow);
  pWindow->CallOnDesactivation();
}

- (void) applicationDidReceiveMemoryWarning:  (UIApplication*) pUIApplication
{
  NGL_DEBUG( NGL_OUT(_T("[nglUIApplicationDelegate applicationDidReceiveMemoryWarning]\n")); )
}
- (void) applicationSignificantTimeChange:    (UIApplication*) pUIApplication
{
//NGL_OUT(_T("[nglUIApplicationDelegate applicationSignificantTimeChange]\n"));
}
- (void) applicationWillTerminate:            (UIApplication*) pUIApplication
{
//	NGL_DEBUG( NGL_OUT(_T("[nglUIApplicationDelegate applicationWillTerminate]\n")) );

	objCCallOnWillExit();

  UIWindow* pWindow = [pUIApplication keyWindow];
  NGL_ASSERT(pWindow);
  [pWindow release];

///< advise the kernel we're quiting
  objCCallOnExit(0);
}

@end///< nglUIApplicationDelegate

/*
** nglApplication
*/

// #define NGL_APP_ENONE  0 (in nglApplicationBase.h)

const nglChar* gpApplicationErrorTable[] =
{
/*  0 */ _T("No error"),
/*  1 */ _T("Couldn't open the default X display. Is the DISPLAY environment variable set ?"),
  NULL
};

nglApplication::nglApplication()
{
//  mExitPosted = false;
  mUseIdle = false;
//  mIdleTimer = NULL;

// nglApplication is a kernel's client, just as plugin instances
  IncRef();
}

nglApplication::~nglApplication()
{
  SetIdle(false);
}


/*
 * Public methods
 */

void nglApplication::Quit (int Code)
{
//  mExitPosted = true;
//  mExitCode = Code;
  
//  QuitApplicationEventLoop();
}


/*
 * Internals
 */

/* Startup
 */

int nglApplication::Main(int argc, char** argv)
{
  NSAutoreleasePool *pPool = [NSAutoreleasePool new];

  Init(argc, argv);

  UIApplicationMain(argc, argv, @"nglUIApplication", @"nglUIApplicationDelegate");

  [pPool release];

  return 0;
}


bool nglApplication::Init(int ArgCnt, char** pArg)
{
  int i;

//Fetch application's name (App->mName) from argv[0]
  nglString arg0(pArg[0]);
  nglString Name = arg0;

//Only keep file name if it's a full path
  i = Name.Find ('/');
  if (i != -1)
    Name = Name.Extract (i + 1, Name.GetLength());
  SetName(Name);

  // Store user args in mArgs
  for (i = 1; i < ArgCnt; i++)
    AddArg( nglString(pArg[i]) );

  return true;
}

static nglString* gBundlePath = NULL;
nglString nglApplication::GetBundlePath()
{
	if (!gBundlePath)
	{
		const char* p = [[[NSBundle mainBundle] resourcePath] UTF8String];
		gBundlePath = new nglString(p);
	}

  return *gBundlePath;
}

/* Event management / main loop
 */

///////////////////////////////////////////////
/*
nglApplication::nglApplication(nglApplication* pApp);
nglApplication::~nglApplication();
void nglApplication::Quit (int Code);
void nglApplication::MakeMenu();
void nglApplication::DoMenuCommand(long menuResult);
int nglApplication::Run();
void nglApplication::OnEvent(int Flags);
void nglApplication::AddEvent(nglEvent* pEvent);
void nglApplication::DelEvent(nglEvent* pEvent);
bool nglApplication::AddTimer(nglTimer* pTimer);
bool nglApplication::DelTimer(nglTimer* pTimer);
bool nglApplication::AddWindow (nglWindow* pWin);
bool nglApplication::DelWindow (nglWindow* pWin);
void nglApplication::DoEvent(EventRecord *event);
OSErr nglApplication::QuitAppleEventHandler( const AppleEvent *appleEvt, AppleEvent* reply, UInt32 refcon );
pascal void TimerAction (EventLoopTimerRef  theTimer, void* userData);
nglString nglApplication::GetClipboard();
bool nglApplication::SetClipboard(const nglString& rString);
*/
