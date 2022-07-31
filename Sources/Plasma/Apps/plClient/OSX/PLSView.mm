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

#import "PLSView.h"
#include "plMessage/plInputEventMsg.h"
#include <Metal/Metal.h>
#include <QuartzCore/QuartzCore.h>

/*
 Plasma view for Cocoa
 
 This view doesn't interact with drawing yet. Ideally it should implement an OpenGL layer and use the still-deprecated-but-more-supported fully hardware accelerated path. Right now it's allowing the OpenGL context to drive it as a dumb view.
 
 Mouse input is handled here. The raw event stream is too wide for good mouse support, and sends events for a lot of unrelated views like the menu bar or window chrome. Using a view in the responder chain will automatically filter down to just our events.
 
 Game Controller in Big Sur also implements mouse support. Becuase Plasma uses a cursor, Cocoa mouse support might be adequate.
 */

@interface PLSView ()

@property NSTrackingArea *mouseTrackingArea;
#if PLASMA_PIPELINE_METAL
@property (weak) CAMetalLayer *metalLayer;
#endif

@end

@implementation PLSView

//MARK: View setup
-(id)initWithFrame:(NSRect)frameRect
{
    self = [super initWithFrame:frameRect];
#if PLASMA_PIPELINE_METAL
    CAMetalLayer *layer = [CAMetalLayer layer];
    layer.contentsScale = [[NSScreen mainScreen] backingScaleFactor];
    layer.maximumDrawableCount = 3;
    self.layer = self.metalLayer = layer;
#endif
    self.layer.backgroundColor = NSColor.blackColor.CGColor;
    return self;
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (BOOL)wantsLayer
{
    return YES;
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
}


//MARK: Left mouse button
-(void)mouseDown:(NSEvent *)event
{
    [self handleMouseButtonEvent:event];
}

-(void)mouseUp:(NSEvent *)event
{
    [self handleMouseButtonEvent:event];
}

-(void)mouseDragged:(NSEvent *)event
{
    [self updateClientMouseLocation:event];
}

//MARK: Right mouse button
-(void)rightMouseDown:(NSEvent *)event
{
    [self handleMouseButtonEvent:event];
}

-(void)rightMouseUp:(NSEvent *)event
{
    [self handleMouseButtonEvent:event];
}

-(void)rightMouseDragged:(NSEvent *)event
{
    [self updateClientMouseLocation:event];
}

//MARK: Mouse movement
-(void)mouseMoved:(NSEvent *)event {
    [self updateClientMouseLocation:event];
}

-(void)mouseEntered:(NSEvent *)event
{
    [NSCursor hide];
}

-(void)mouseExited:(NSEvent *)event
{
    //[super mouseExited:event];
    [NSCursor unhide];
}

//MARK: Cocoa region tracking
-(void)updateTrackingAreas
{
    if(self.mouseTrackingArea) {
        [self removeTrackingArea:self.mouseTrackingArea];
    }
    self.mouseTrackingArea = [[NSTrackingArea alloc] initWithRect:self.bounds options:NSTrackingMouseEnteredAndExited | NSTrackingActiveWhenFirstResponder owner:self userInfo:nil];
    [self addTrackingArea:self.mouseTrackingArea];
}

//MARK: Mouse click handler
-(void)handleMouseButtonEvent:(NSEvent *)event {
    [self updateClientMouseLocation:event];
    
    CGPoint windowLocation = [event locationInWindow];
    CGPoint viewLocation = [self convertPoint:windowLocation fromView:nil];
    
    plIMouseBEventMsg* pBMsg = new plIMouseBEventMsg;
    
    if(event.type == NSEventTypeLeftMouseUp) {
        pBMsg->fButton |= kLeftButtonUp;
    }
    else if(event.type == NSEventTypeRightMouseUp) {
        pBMsg->fButton |= kRightButtonUp;
    }
    else if(event.type == NSEventTypeLeftMouseDown) {
        pBMsg->fButton |= kLeftButtonDown;
    }
    else if(event.type == NSEventTypeRightMouseDown) {
        pBMsg->fButton |= kRightButtonDown;
    }
    
    @synchronized (self.layer) {
        self.inputManager->MsgReceive(pBMsg);
    }
    
    delete(pBMsg);
}

-(void)updateClientMouseLocation:(NSEvent *)event {
    CGPoint windowLocation = [event locationInWindow];
    CGPoint viewLocation = [self convertPoint:windowLocation fromView:nil];
    
    NSRect windowViewBounds = self.bounds;
    CGFloat deltaX = (windowLocation.x) / windowViewBounds.size.width;
    CGFloat deltaY = (windowViewBounds.size.height - windowLocation.y) / windowViewBounds.size.height;
    
    plIMouseXEventMsg* pXMsg = new plIMouseXEventMsg;
    plIMouseYEventMsg* pYMsg = new plIMouseYEventMsg;
    
    pXMsg->fWx = viewLocation.x;
    pXMsg->fX = deltaX;

    pYMsg->fWy = (windowViewBounds.size.height - windowLocation.y);
    pYMsg->fY = deltaY;
    
    @synchronized (self.layer) {
        if(self.inputManager) {
            self.inputManager->MsgReceive(pXMsg);
            self.inputManager->MsgReceive(pYMsg);
        }
    }
    
    delete(pXMsg);
    delete(pYMsg);
    
    /*
     For some reason the cusor has to get to the edge of the window before the
     client will properly interpret us moving back to center. Guessing that the
     large distance change triggers some condition in the client.
     */
    
    if(self.inputManager->RecenterMouse() && (pXMsg->fX <= 0.1 || pXMsg->fX >= 0.9) ) {
        CGRect bounds = self.bounds;
        CGPoint midPoint = CGPointMake(CGRectGetMidX(bounds), windowLocation.y);
        CGWarpMouseCursorPosition([self convertPoint:midPoint toView:nil]);
    }
    
    if(self.inputManager->RecenterMouse()  && (pYMsg->fY <= 0.1 || pYMsg->fY >= 0.9) ) {
        CGRect bounds = self.bounds;
        CGPoint midPoint = CGPointMake(windowLocation.x, CGRectGetMidY(bounds));
        CGWarpMouseCursorPosition([self convertPoint:midPoint toView:nil]);
    }
}

- (void)viewDidChangeBackingProperties
{
    [super viewDidChangeBackingProperties];
    [self resizeDrawable:self.window.screen.backingScaleFactor];
}

- (void)setFrameSize:(NSSize)size
{
    [super setFrameSize:size];
    [self resizeDrawable:self.window.screen.backingScaleFactor];
}

- (void)setBoundsSize:(NSSize)size
{
    [super setBoundsSize:size];
    [self resizeDrawable:self.window.screen.backingScaleFactor];
}

- (void)resizeDrawable:(CGFloat)scaleFactor
{
    CGSize newSize = [self convertRectToBacking:self.bounds].size;

    if(newSize.width <= 0 || newSize.width <= 0)
    {
        return;
    }
    
#if PLASMA_PIPELINE_METAL
    if(newSize.width == _metalLayer.drawableSize.width &&
       newSize.height == _metalLayer.drawableSize.height)
    {
        return;
    }

    _metalLayer.drawableSize = newSize;
#endif
    [self.delegate renderView:self didChangeOutputSize:newSize scale:scaleFactor];
}

@end
