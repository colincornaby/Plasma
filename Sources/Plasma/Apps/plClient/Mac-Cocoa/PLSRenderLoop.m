//
//  PLSRenderLoop.m
//  plClient
//
//  Created by Colin Cornaby on 11/6/25.
//

#import "PLSRenderLoop.h"
#include <QuartzCore/QuartzCore.h>

NS_ASSUME_NONNULL_BEGIN

@interface PLSRenderLoop ()

@property (nonnull) NSThread* renderThread;
@property CVDisplayLinkRef displayLink;
@property NSView* currentView;
@property bool continueRunLoop;

@end

@implementation PLSRenderLoop

- (id)init {
    self = [super init];
    self.renderThread = [[NSThread alloc] initWithTarget:self selector:@selector(runLoop) object:nil];
    self.renderThread.name = @"Render Thread";
    self.continueRunLoop = true;
    [self.renderThread start];
    return self;
}

- (void)startLoop:(NSView *)view {
    self.currentView = view;
    self.continueRunLoop = true;
    [self setupLegacyDisplayLink];
    
    // The legacy display link requires us to track display changes
    // ourselves and re-link if the display changes.
    // Sign up for the callback to do that ourselves.
    [[NSNotificationCenter defaultCenter]
        addObserverForName:NSWindowDidChangeScreenNotification
                    object:view.window
                     queue:[NSOperationQueue mainQueue]
                usingBlock:^(NSNotification* _Nonnull note) {
                    // if we change displays, setup a new draw loop. The new display might
                    // have a different or variable refresh rate.
                    [self setupLegacyDisplayLink];
                }];
}

- (void)setupLegacyDisplayLink
{
    
    if (self.displayLink) {
        CVDisplayLinkStop(self.displayLink);
        CVDisplayLinkRelease(self.displayLink);
    }

    CVDisplayLinkCreateWithCGDisplay(
        [self.currentView.window.screen.deviceDescription[@"NSScreenNumber"] intValue], &_displayLink);
    CVDisplayLinkSetOutputHandler(
        self.displayLink,
        ^CVReturn(CVDisplayLinkRef _Nonnull displayLink, const CVTimeStamp* _Nonnull inNow,
                  const CVTimeStamp* _Nonnull inOutputTime, CVOptionFlags flagsIn,
                  CVOptionFlags* _Nonnull flagsOut) {
                      [self.delegate performSelector:@selector(renderInRenderLoop:) onThread:self.renderThread withObject:self waitUntilDone:YES];
            return kCVReturnSuccess;
        });
    CVDisplayLinkStart(self.displayLink);
}

- (void)setupLoop
{
    // Future implementations will need this function
    // but not the legacy version
}

- (void)runLoop
{
    NSRunLoop *runLoop = [NSRunLoop currentRunLoop];
    [self setupLoop];
    
    // The system sets the '_continueRunLoop' ivar outside this thread, so it needs to synchronize. Create a
    // 'continueRunLoop' local var that the system can set from the _continueRunLoop ivar in a @synchronized block.
    BOOL continueRunLoop = YES;
    
    // Begin the run loop.
    while (continueRunLoop)
    {
        // Create the autorelease pool for the current iteration of the loop.
        @autoreleasepool
        {
            // Run the loop once accepting input only from the display link.
            [runLoop runMode:NSDefaultRunLoopMode beforeDate:[NSDate distantFuture]];
        }
        
        // Synchronize this with the _continueRunLoop ivar, which is set on another thread.
        @synchronized(self)
        {
            // When accessing anything outside the thread, such as the '_continueRunLoop' ivar,
            // the system reads it inside the synchronized block to ensure it writes fully/atomically.
            continueRunLoop = _continueRunLoop;
        }
    }
    NSLog(@"Thread over");
}

- (void)dealloc
{
    
    self.continueRunLoop = false;
}

@end

NS_ASSUME_NONNULL_END
