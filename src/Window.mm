#include "Window.h"
#include <Cocoa/Cocoa.h>
#include "MacOSXAppData.m"
#include "Application.h"

using namespace flash;
using namespace ui;

struct MacOSXWindowData {
    NSWindow* window;
};

@interface OpenGLView : NSOpenGLView {
}
@end

@implementation OpenGLView

- (id) initWithFrame: (NSRect) frame {
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

    return self;
}
@end

@interface WindowDelegate : NSObject <NSWindowDelegate> {
}
@end

@implementation WindowDelegate : NSObject

- (void) customClose: (id) sender {
    [NSApp stop: self];
}

- (void) applicationWillFinishLaunching: (NSNotification*) notification {

}

- (void) applicationDidFinishLaunching: (NSNotification*) notification {
}

- (void) dealloc {
    [super dealloc];
}

@end

Window::~Window() {
    delete static_cast<MacOSXWindowData*>(m_data);
}

Window* Window::createWindow() {
    Window* window = new Window();
    MacOSXWindowData* data = new MacOSXWindowData();
    window->m_data = data;

    MacOSXAppData* appData = static_cast<MacOSXAppData*>(Application::instance().m_data);

    NSRect viewBounds = NSMakeRect(0, 0, 800, 600);
    NSRect screenBounds = [[NSScreen mainScreen] frame];
    NSRect centered = NSMakeRect(NSMidX(screenBounds) - NSMidX(viewBounds),
                                 NSMidY(screenBounds) - NSMidY(viewBounds), viewBounds.size.width, viewBounds.size.height);
    NSWindow* nsWindow = [[NSWindow alloc] initWithContentRect: centered
            styleMask: NSTitledWindowMask | NSClosableWindowMask | NSResizableWindowMask backing: NSBackingStoreBuffered defer: NO];
    nsWindow.contentView = [[OpenGLView alloc] init];
    nsWindow.delegate = [[WindowDelegate alloc] init];

    [[nsWindow standardWindowButton: NSWindowCloseButton] setAction: @selector(customClose:)];
    [[nsWindow standardWindowButton: NSWindowCloseButton] setTarget: appData->delegate];

    [nsWindow setOneShot: NO];
    [nsWindow setTitle: @"Flash Engine"];
    [nsWindow makeKeyAndOrderFront: appData->delegate];
    data->window = nsWindow;
    return window;
}