//
//  PLSLoginWindowController.h
//  plClient
//
//  Created by Colin Cornaby on 2/20/22.
//

#import <Cocoa/Cocoa.h>

NS_ASSUME_NONNULL_BEGIN

@class PLSLoginWindowController;

@protocol PLSLoginWindowControllerDelegate <NSObject>
-(void)loginWindowControllerDidLogin:(PLSLoginWindowController *)sender;
@end

@interface PLSLoginWindowController : NSWindowController <NSURLSessionDelegate>
@property (weak) id<PLSLoginWindowControllerDelegate> delegate;
@end

NS_ASSUME_NONNULL_END
