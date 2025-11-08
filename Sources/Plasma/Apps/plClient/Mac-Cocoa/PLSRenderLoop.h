//
//  PLSRenderLoop.h
//  plClient
//
//  Created by Colin Cornaby on 11/6/25.
//

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

NS_ASSUME_NONNULL_BEGIN

@class PLSRenderLoop;

@protocol PLSRenderLoopDelegate

- (void)renderInRenderLoop:(PLSRenderLoop*)renderLoop;

@end

@interface PLSRenderLoop : NSObject

@property (nonnull, readonly) NSThread* renderThread;
@property (nullable, weak) id delegate;

-(void)startLoop:(NSView*)view;
-(void)stopLoop;
// For subclassers - do not call directly
-(void)setupLoop;

@end

NS_ASSUME_NONNULL_END
