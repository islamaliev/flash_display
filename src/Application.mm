#include "Application.h"
#include <Cocoa/Cocoa.h>
#import <OpenGL/OpenGL.h>
#import <OpenGL/gl3.h>

using namespace flash;
using namespace ui;

@interface ApplicationDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate> {
}
@end

@interface OpenGLView : NSOpenGLView {
    CVDisplayLinkRef DisplayLink;
}
@end

@interface MacOSXAppData : NSObject {
    @public
    NSAutoreleasePool* pool;
    NSApplication* application;
    ApplicationDelegate* delegate;
    NSWindow* window;
    CGLContextObj* context;
    
    void(*initFunc)(void);
    void(*updateFunc)(void);
};
@end

@implementation ApplicationDelegate : NSObject

- (void) customClose: (id) sender {
    [NSApp stop: self];
}

- (void) applicationWillFinishLaunching: (NSNotification*) notification {
    NSLog(@"ApplicationDelegate::applicationWillFinishLaunching()");
    MacOSXAppData* appData = (MacOSXAppData*) (Application::instance().m_data);

#ifdef OFFSCREEN
    CGLPixelFormatAttribute attributes[4] = {
        kCGLPFAAccelerated,   // no software rendering
        kCGLPFAOpenGLProfile, // core profile with the version stated below
        (CGLPixelFormatAttribute) kCGLOGLPVersion_3_2_Core,
        (CGLPixelFormatAttribute) 0
    };
    
    appData->context = new CGLContextObj();
    
    CGLPixelFormatObj pix;
    CGLError errorCode;
    GLint num; // stores the number of possible pixel formats
    errorCode = CGLChoosePixelFormat(attributes, &pix, &num);
    assert(errorCode == kCGLNoError);
    errorCode = CGLCreateContext(pix, NULL, appData->context); // second parameter can be another context for object sharing
    assert(errorCode == kCGLNoError);
    CGLDestroyPixelFormat(pix);
    errorCode = CGLSetCurrentContext(*appData->context);
    assert(errorCode == kCGLNoError);
#else
    NSRect viewBounds = NSMakeRect(0, 0, 800, 600);
    NSRect screenBounds = [[NSScreen mainScreen] frame];
    NSRect centered = NSMakeRect(NSMidX(screenBounds) - NSMidX(viewBounds),
                             NSMidY(screenBounds) - NSMidY(viewBounds), viewBounds.size.width, viewBounds.size.height);
    appData->window = [[NSWindow alloc] initWithContentRect:centered styleMask:NSTitledWindowMask | NSClosableWindowMask
            | NSResizableWindowMask backing:NSBackingStoreBuffered defer:NO];
    appData->window.contentView = [[OpenGLView alloc] init];
    appData->window.delegate = self;

    [[appData->window standardWindowButton: NSWindowCloseButton] setAction: @selector(customClose:)];
    [[appData->window standardWindowButton: NSWindowCloseButton] setTarget: self];

    [appData->window setOneShot: NO];
    [appData->window setTitle: @"Flash Engine"];
    [appData->window makeKeyAndOrderFront: self];
#endif

    appData->initFunc();
}

- (void) applicationDidFinishLaunching: (NSNotification*) notification {
    NSLog(@"ApplicationDelegate::applicationDidFinishLaunching()");
    [NSApp stop: self];
}

- (void) applicationWillTerminateLaunching: (NSNotification*) notification {
    NSLog(@"ApplicationDelegate::applicationWillTerminateLaunching()");
    MacOSXAppData* appData = (MacOSXAppData*) (Application::instance().m_data);
    CGLSetCurrentContext(NULL);
    CGLDestroyContext(*appData->context);
    delete appData->context;
}

- (void) windowDidUpdate: (NSNotification *) notification {
    NSLog(@"ApplicationDelegate::windowDidUpdate()");
}

- (void) windowDidBecomeMain: (NSNotification *) notification {
    NSLog(@"ApplicationDelegate::windowDidBecomeMain()");
}

- (void) windowDidResignMain: (NSNotification *) notification {
    NSLog(@"ApplicationDelegate::windowDidResignMain()");
}

- (void) windowWillClose: (NSNotification *) notification {
    NSLog(@"ApplicationDelegate::windowWillClose()");
}

- (void) windowDidBecomeKey: (NSNotification *) notification {
    NSLog(@"ApplicationDelegate::windowDidBecomeKey()");
}

- (void) windowDidResignKey: (NSNotification *) notification {
    NSLog(@"ApplicationDelegate::windowDidResignKey()");
}

- (void) dealloc {
    [super dealloc];
}

@end

@implementation MacOSXAppData {
}
@end

Application::Application() {
    MacOSXAppData* data = [[MacOSXAppData alloc] init];
    data->pool = [[NSAutoreleasePool alloc] init];
    data->application = [NSApplication sharedApplication];

    data->delegate = [[[ApplicationDelegate alloc] init] autorelease];

    [data->application setDelegate: data->delegate];
    // make app icon appear in the Dock
    [data->application setActivationPolicy: NSApplicationActivationPolicyRegular];
    m_data = data;
}

int Application::run(void(*initF)(void), void(*updateF)(void)) {
    MacOSXAppData* data = static_cast<MacOSXAppData*>(m_data);
    data->initFunc = initF;
    data->updateFunc = updateF;

    [data->application run];

    return EXIT_SUCCESS;
}

void Application::swap() {
    MacOSXAppData* data = static_cast<MacOSXAppData*>(m_data);
    [[[data->window contentView] openGLContext] flushBuffer];
}

bool _running = true;

void Application::stop() {
    _running = false;
    [NSApp terminate: nil];
}

bool Application::isRunning() const {
    return _running;
}

static CVReturn OnVSync(CVDisplayLinkRef, const CVTimeStamp *, const CVTimeStamp *, CVOptionFlags, CVOptionFlags *, void *)
{
    MacOSXAppData* appData = static_cast<MacOSXAppData*>(Application::instance().m_data);
    appData->updateFunc();
    return kCVReturnSuccess;
}

@implementation OpenGLView

- (id) initWithFrame: (NSRect) frame {
    NSLog(@"OpenGLView::initWithFrame()");
    NSOpenGLPixelFormatAttribute pixelFormatAttributes[] = {
            NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
            NSOpenGLPFAColorSize    , 24                           ,
            NSOpenGLPFAAlphaSize    , 8                            ,
            NSOpenGLPFADoubleBuffer ,
            NSOpenGLPFAAccelerated  ,
            NSOpenGLPFANoRecovery   ,
            0
    };
    NSOpenGLPixelFormat* pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes: pixelFormatAttributes];
    self = [super initWithFrame: frame pixelFormat: pixelFormat];

    [[self openGLContext] makeCurrentContext];

    GLint swapInt = 1;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
    CVDisplayLinkCreateWithActiveCGDisplays(&DisplayLink);
    CGLContextObj cglContext = [[self openGLContext] CGLContextObj];
    CGLPixelFormatObj cglPixelFormat = [pixelFormat CGLPixelFormatObj];
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(DisplayLink, cglContext, cglPixelFormat);
    CVDisplayLinkSetOutputCallback(DisplayLink, &OnVSync, nil);
    CVDisplayLinkStart(DisplayLink);

    return self;
}
@end
