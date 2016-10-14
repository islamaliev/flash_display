#include <Foundation/Foundation.h>

@class NSAutoreleasePool;
@class NSApplication;
@class ApplicationDelegate;

@interface MacOSXAppData : NSObject {
    @public
    NSAutoreleasePool* pool;
    NSApplication* application;
    ApplicationDelegate* delegate;
};
@end

@implementation MacOSXAppData {

}
@end
