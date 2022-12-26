/*==LICENSE==*

CyanWorlds.com Engine - MMOG client, server and tools
Copyright (C) 2011  Cyan Worlds, Inc.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Additional permissions under GNU GPL version 3 section 7

If you modify this Program, or any covered work, by linking or
combining it with any of RAD Game Tools Bink SDK, Autodesk 3ds Max SDK,
NVIDIA PhysX SDK, Microsoft DirectX SDK, OpenSSL library, Independent
JPEG Group JPEG library, Microsoft Windows Media SDK, or Apple QuickTime SDK
(or a modified version of those libraries),
containing parts covered by the terms of the Bink SDK EULA, 3ds Max EULA,
PhysX SDK EULA, DirectX SDK EULA, OpenSSL and SSLeay licenses, IJG
JPEG Library README, Windows Media SDK EULA, or QuickTime SDK EULA, the
licensors of this Program grant you additional
permission to convey the resulting work. Corresponding Source for a
non-source form of such a combination shall include the source code for
the parts of OpenSSL and IJG JPEG Library used as well as that of the covered
work.

You can contact Cyan Worlds, Inc. by email legal@cyan.com
 or by snail mail at:
      Cyan Worlds, Inc.
      14617 N Newport Hwy
      Mead, WA   99021

*==LICENSE==*/

#include "plProduct.h"
#include "plClient/plClient.h"
#include "plClient/plClientLoader.h"
#import "PLSKeyboardEventMonitor.h"
#if PLASMA_PIPELINE_GL
#include "plPipeline/GL/plGLPipeline.h"
#endif
#if PLASMA_PIPELINE_METAL
#include "pfMetalPipeline/plMetalPipeline.h"
#endif

#include "pfConsoleCore/pfConsoleEngine.h"
#include "pfGameGUIMgr/pfGameGUIMgr.h"
#include "plInputCore/plInputDevice.h"

#import "Cocoa/Cocoa.h"
#if PLASMA_PIPELINE_GL
#import <OpenGL/gl.h>
#endif
#if PLASMA_PIPELINE_METAL
#import <Metal/Metal.h>
#endif
#import <QuartzCore/QuartzCore.h>
#import "PLSKeyboardEventMonitor.h"
#import "PLSView.h"
#import <QuartzCore/QuartzCore.hpp>

void PumpMessageQueueProc();

void plClient::IResizeNativeDisplayDevice(int width, int height, bool windowed) {}
void plClient::IChangeResolution(int width, int height) {}
void plClient::IUpdateProgressIndicator(plOperationProgress* progress) {}
void plClient::InitDLLs() {}
void plClient::ShutdownDLLs() {}
void plClient::ShowClientWindow() {}
void plClient::FlashWindow() {}

@interface AppDelegate: NSWindowController <NSApplicationDelegate, NSWindowDelegate, PLSViewDelegate> {
    @public plClientLoader gClient;
    
}

@property (retain) NSTimer *drawTimer;
@property (retain) PLSKeyboardEventMonitor *eventMonitor;
@property CVDisplayLinkRef displayLink;
@property CALayer* renderLayer;
@property (weak) PLSView* plsView;

@end

@implementation AppDelegate
PF_CONSOLE_LINK_ALL()

-(id)init {
    
    // Style flags
    NSUInteger windowStyle =
    (NSWindowStyleMaskTitled  |
         NSWindowStyleMaskClosable |
         NSWindowStyleMaskResizable);
    
    // Window bounds (x, y, width, height)
    NSRect windowRect = NSMakeRect(100, 100, 800, 600);
    
    NSWindow * window = [[NSWindow alloc] initWithContentRect:windowRect
                        styleMask:windowStyle
                        backing:NSBackingStoreBuffered
                        defer:NO];
    PLSView *view = [[PLSView alloc] init];
    self.plsView = view;
    window.contentView = view;
    
    self = [super initWithWindow:window];
    self.window.acceptsMouseMovedEvents = YES;
    return self;
}

- (void)startRunLoop {
    dispatch_queue_t loadingQueue = dispatch_queue_create("", DISPATCH_QUEUE_SERIAL);
    
    dispatch_async(loadingQueue, ^{
        gClient->SetMessagePumpProc(PumpMessageQueueProc);
        gClient.Start();
    });
    
    dispatch_async(loadingQueue, ^{
        [self setupRunLoop];
        [[NSNotificationCenter defaultCenter] addObserverForName:NSWindowDidChangeScreenNotification object:self.window queue:[NSOperationQueue mainQueue] usingBlock:^(NSNotification * _Nonnull note) {
            //if we change displays, setup a new draw loop. The new display might have a different or variable refresh rate.
            [self setupRunLoop];
        }];
    });
}

- (void)setupRunLoop {
    if(self.displayLink) {
        CVDisplayLinkStop(self.displayLink);
        CVDisplayLinkRelease(self.displayLink);
    }
    CVDisplayLinkCreateWithCGDisplay([self.window.screen.deviceDescription[@"NSScreenNumber"] intValue], &_displayLink);
    CVDisplayLinkSetOutputHandler(self.displayLink, ^CVReturn(CVDisplayLinkRef  _Nonnull displayLink, const CVTimeStamp * _Nonnull inNow, const CVTimeStamp * _Nonnull inOutputTime, CVOptionFlags flagsIn, CVOptionFlags * _Nonnull flagsOut) {
        @autoreleasepool
        {
            @synchronized (_renderLayer) {
                [self runLoop];
            }
        }
        return kCVReturnSuccess;
    });
    CVDisplayLinkStart(self.displayLink);
}

- (void)runLoop {
    //dispatch_sync(dispatch_get_main_queue(), ^{
    //    gClient->GetPipeline()->Resize(self.window.contentView.bounds.size.width, self.window.contentView.bounds.size.height);
    //});
    gClient->MainLoop();
    //PumpMessageQueueProc();

    if (gClient->GetDone()) {
        gClient.ShutdownEnd();
        [NSApp terminate:self];
    }
}

- (void)renderView:(PLSView *)view didChangeOutputSize:(CGSize)size scale:(NSUInteger)scale
{
    float aspectratio = (float)size.width / (float)size.height;
    @synchronized (_renderLayer) {
        pfGameGUIMgr::GetInstance()->SetAspectRatio( aspectratio );
        plMouseDevice::Instance()->SetDisplayResolution(size.width/scale, size.height/scale);
        gClient->GetPipeline()->Resize((int)size.width, (int)size.height);
    }
}

- (void)applicationDidFinishLaunching:(NSNotification *)notification
{
    PF_CONSOLE_INITIALIZE(Audio)
    
    self.plsView.delegate = self;
    
    if([NSBundle mainBundle] && [NSBundle.mainBundle pathForResource:@"resource" ofType:@"dat"]) {
        //if we're a proper app bundle, start the game using our resources dir
        chdir([[[NSBundle mainBundle] resourcePath] cStringUsingEncoding:NSUTF8StringEncoding]);
    }
    // Create a window:

    // Window controller
    [self.window setContentSize:NSMakeSize(800, 600)];
    [self.window orderFrontRegardless];
    self.renderLayer = self.window.contentView.layer;
    
    gClient.SetClientWindow((hsWindowHndl)(__bridge void *)self.window);
    gClient.SetClientDisplay((hsWindowHndl)NULL);
    
    // We should quite frankly be done initing the client by now. But, if not, spawn the good old
    // "Starting URU, please wait..." dialog (not so yay)
    while (!gClient.IsInited())
    {
        [[NSRunLoop mainRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate now]];
    }
    
#if PLASMA_PIPELINE_METAL
    plMetalPipeline *pipeline = (plMetalPipeline *)gClient->GetPipeline();
    pipeline->currentDrawableCallback = [self] {
        id< CAMetalDrawable > drawable;
        drawable = [((CAMetalLayer *) _renderLayer) nextDrawable];
        CA::MetalDrawable * mtlDrawable = ( __bridge CA::MetalDrawable* ) drawable;
        mtlDrawable->retain();
        return mtlDrawable;
    };
    
    NSString *productTitle = [NSString stringWithCString:plProduct::LongName().c_str() encoding:NSUTF8StringEncoding];
    id<MTLDevice> device = ((CAMetalLayer *) self.window.contentView.layer).device;
#ifdef HS_DEBUGGING
    [self.window setTitle:[NSString stringWithFormat:@"%@ - %@, %@",
                           productTitle,
#ifdef __arm64__
                           @"ARM64",
#else
                           @"x86_64",
#endif
                           device.name]];
#else
    [self.window setTitle:productTitle];
#endif
    
#else
    [self.window setTitle:[NSString stringWithCString:plProduct::LongName().c_str() encoding:NSUTF8StringEncoding]];
#endif
    
    if(!gClient) {
        exit(0);
    }
    
    self.eventMonitor = [[PLSKeyboardEventMonitor alloc] initWithView:self.window.contentView inputManager:&gClient];
    ((PLSView *)self.window.contentView).inputManager = gClient->GetInputManager();

    // Main loop
    if (gClient && !gClient->GetDone())
    {
        [self startRunLoop];
    }
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
    //macOS has requested we terminate. This could happen because the user asked us to quit, the system is going to restart, etc...
    //Do any cleanup we need to do. If we need to we can ask for more time, but right now nothing in our implementation requires that.
    CVDisplayLinkStop(self.displayLink);
    @synchronized (_renderLayer) {
        gClient.ShutdownEnd();
    }
    return NSTerminateNow;
}

@end

void PumpMessageQueueProc()
{
}

int main(int argc, const char** argv)
{
    [NSApplication sharedApplication];
    [NSBundle.mainBundle loadNibNamed:@"MainMenu" owner:NSApp topLevelObjects:nil];
    
    /*
     On Windows, init could be long enough that a loading screen has to be shown, but it's
     generally considered to be pretty short. The Mac doesn't really do quick interstial loading windows,
     it just bounces the dock icon until we're ready to go. So we'll do the init here before
     we mark the app as running. If Init ever gets heavier, feel free to revisit this assumption.
     */
    //argc, argv
    ((AppDelegate *)[NSApp delegate])->gClient.Init();
    
    [NSApp run];
}
