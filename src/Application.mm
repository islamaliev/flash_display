#include "Application.h"
#include <Cocoa/Cocoa.h>
#include "MacOSXAppData.m"

using namespace flash;
using namespace ui;

@interface ApplicationDelegate : NSObject <NSApplicationDelegate> {
}
@end

@implementation ApplicationDelegate : NSObject

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

int Application::run() {
    MacOSXAppData* data = static_cast<MacOSXAppData*>(m_data);
    [data->application run];

    [data->pool drain];

    return EXIT_SUCCESS;
}
