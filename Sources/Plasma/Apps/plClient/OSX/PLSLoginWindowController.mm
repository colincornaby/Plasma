//
//  PLSLoginWindowController.m
//  plClient
//
//  Created by Colin Cornaby on 2/20/22.
//

#import "PLSLoginWindowController.h"
#include <string_theory/string>
#include "plNetGameLib/plNetGameLib.h"
#include "plNetClient/plNetClientMgr.h"
#include "plNetGameLib/plNetGameLib.h"
#include "plProduct.h"
#include "pfPasswordStore/pfPasswordStore.h"

@interface PLSLoginParameters: NSObject
@property NSString *username;
@property NSString *password;
@property BOOL rememberPassword;
@end

@interface PLSLoginWindowController ()

@property (assign) IBOutlet NSTextField *accountNameTextField;
@property (assign) IBOutlet NSSecureTextField *passwordTextField;
@property (assign) IBOutlet NSTextField *statusTextField;
@property (assign) IBOutlet NSTextField *productTextField;
@property (assign) IBOutlet NSWindow *loggingInWindow;

@property (strong) PLSLoginParameters *loginParameters;
@property (strong) NSOperationQueue *loginOperationQueue;

@end

#define FAKE_PASS_STRING @"********"

@implementation PLSLoginParameters

-(void)mutableUserDefaults:(bool (^)(NSMutableDictionary *dictionary))callback {
    //windows segments by product name here. in since user defaults belong to this product, we don't need to do that.
    NSString *serverName = [NSString stringWithCString:GetServerDisplayName().c_str() encoding:NSUTF8StringEncoding];
    NSMutableDictionary *settingsDictionary = [[[NSUserDefaults standardUserDefaults] dictionaryForKey:serverName] mutableCopy];
    if(!settingsDictionary)
        settingsDictionary = [NSMutableDictionary dictionary];
    if(callback(settingsDictionary)) {
        [[NSUserDefaults standardUserDefaults] setObject:settingsDictionary forKey:serverName];
        [[NSUserDefaults standardUserDefaults] synchronize];
    }
}

-(void)save {
    //windows segments by product name here. in since user defaults belong to this product, we don't need to do that.
    NSString *serverName = [NSString stringWithCString:GetServerDisplayName().c_str() encoding:NSUTF8StringEncoding];
    NSMutableDictionary *settingsDictionary = [[[NSUserDefaults standardUserDefaults] dictionaryForKey:serverName] mutableCopy];
    if(!settingsDictionary)
        settingsDictionary = [NSMutableDictionary dictionary];
    [settingsDictionary setObject:self.username forKey:@"LastAccountName"];
    [settingsDictionary setObject:[NSNumber numberWithBool:self.rememberPassword] forKey:@"RememberPassword"];
    [[NSUserDefaults standardUserDefaults] setObject:settingsDictionary forKey:serverName];
    [[NSUserDefaults standardUserDefaults] synchronize];
    
    if(self.password && ![self.password isEqualToString:FAKE_PASS_STRING]) {
        ST::string username = ST::string([self.username cStringUsingEncoding:NSUTF8StringEncoding]);
        ST::string password = ST::string([self.password cStringUsingEncoding:NSUTF8StringEncoding]);
        
        pfPasswordStore* store = pfPasswordStore::Instance();
        if (self.rememberPassword)
            store->SetPassword(username, password);
        else
            store->SetPassword(username, ST::string());
    }
}

-(void)load {
    NSString *serverName = [NSString stringWithCString:GetServerDisplayName().c_str() encoding:NSUTF8StringEncoding];
    NSDictionary *settingsDictionary = [[NSUserDefaults standardUserDefaults] dictionaryForKey:serverName];
    self.username = [settingsDictionary objectForKey:@"LastAccountName"];
    self.rememberPassword = [[settingsDictionary objectForKey:@"RememberPassword"] boolValue];
    
    if(self.rememberPassword) {
        pfPasswordStore* store = pfPasswordStore::Instance();
        ST::string username = ST::string([self.username cStringUsingEncoding:NSUTF8StringEncoding]);
        ST::string password = store->GetPassword(username);
        self.password = [NSString stringWithCString:password.c_str() encoding:NSUTF8StringEncoding];
    }
}

-(void)storeHash:(ShaDigest &)namePassHash {
    ST::string password = ST::string([self.password cStringUsingEncoding:NSUTF8StringEncoding]);
    plSHA1Checksum shasum(password.size(), reinterpret_cast<const uint8_t*>(password.c_str()));
    uint32_t* dest = reinterpret_cast<uint32_t*>(namePassHash);
    const uint32_t* from = reinterpret_cast<const uint32_t*>(shasum.GetValue());
    dest[0] = hsToBE32(from[0]);
    dest[1] = hsToBE32(from[1]);
    dest[2] = hsToBE32(from[2]);
    dest[3] = hsToBE32(from[3]);
    dest[4] = hsToBE32(from[4]);
}

@end

@implementation PLSLoginWindowController

- (void)windowDidLoad {
    self.loginParameters = [[PLSLoginParameters alloc] init];
    [self.loginParameters load];
    
    self.loginOperationQueue = [[NSOperationQueue alloc] init];
    
    if(self.loginParameters.rememberPassword) {
        [self.passwordTextField setStringValue:FAKE_PASS_STRING];
    }
    
    [super windowDidLoad];
    
    [self.window center];
    [self loadServerStatus];
    [self.productTextField setStringValue:[NSString stringWithCString:plProduct::ProductString().c_str() encoding:NSUTF8StringEncoding]];
}

-(void)loadServerStatus {
    NSString *urlString = [NSString stringWithCString:GetServerStatusUrl().c_str() encoding:NSUTF8StringEncoding];
    NSURL *url = [NSURL URLWithString:urlString];
    NSURLSessionConfiguration *URLSessionConfiguration = [NSURLSessionConfiguration ephemeralSessionConfiguration];
    NSURLSession *session = [NSURLSession sessionWithConfiguration:URLSessionConfiguration delegate:self delegateQueue:NSOperationQueue.mainQueue];
    NSURLSessionTask *statusTask = [session dataTaskWithURL:url completionHandler:^(NSData * _Nullable data, NSURLResponse * _Nullable response, NSError * _Nullable error) {
        if(data) {
            NSString *statusString = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
            self.statusTextField.stringValue = statusString;
        }
    }];
    [statusTask resume];
}

- (NSNibName)windowNibName {
    return @"PLSLoginWindowController";
}

- (IBAction)quitButtonHit:(id)sender {
    [NSApp terminate:self];
}

- (IBAction)loginButtonHit:(id)sender {
    [self.loginParameters save];
    
    [self.window beginSheet:self.loggingInWindow completionHandler:^(NSModalResponse returnCode) {
        
    }];
    
    ShaDigest hash;
    [self.loginParameters storeHash:hash];
    
    NetCliAuthAutoReconnectEnable(false);
    NetCommStartup();
    ST::string username = ST::string([self.loginParameters.username cStringUsingEncoding:NSUTF8StringEncoding]);
    NetCommSetAccountUsernamePassword(username, hash);
    char16_t platform[] = u"mac";
    NetCommSetAuthTokenAndOS(nullptr, platform);
    
    
    if (!NetCliAuthQueryConnected())
        NetCommConnect();
    NetCommAuthenticate(nullptr);
    
    NSBlockOperation *operation = [[NSBlockOperation alloc] init];
    __weak NSBlockOperation *weakOperation = operation;
    [operation addExecutionBlock:^{
       while (!NetCommIsLoginComplete()) {
           if(weakOperation.cancelled) {
               return;
           }
           NetCommUpdate();
       }
       
       ENetError result = NetCommGetAuthResult();
       [NSOperationQueue.mainQueue addOperationWithBlock:^{
           [self loginAttemptEndedWithResult:result];
       }];
    }];
    [self.loginOperationQueue addOperation:operation];
}

-(void)loginAttemptEndedWithResult:(ENetError)result {
    [self.window endSheet:self.loggingInWindow];
    
    if(result == kNetSuccess) {
        [self.delegate loginWindowControllerDidLogin:self];
    } else {
        NetCommDisconnect();
        NSAlert *loginFailedAlert = [[NSAlert alloc] init];
        loginFailedAlert.messageText = @"Authentication Failed";
        loginFailedAlert.informativeText = @"Please try again.";
        loginFailedAlert.alertStyle = NSAlertStyleCritical;
        [loginFailedAlert addButtonWithTitle:@"OK"];
        [loginFailedAlert beginSheetModalForWindow:self.window completionHandler:nil];
    }
}

- (IBAction)needAccountButtonHit:(id)sender {
    NSString *urlString = [NSString stringWithCString:GetServerSignupUrl().c_str() encoding:NSUTF8StringEncoding];
    NSURL *url = [NSURL URLWithString:urlString];
    if(url) {
        [[NSWorkspace sharedWorkspace] openURL:url];
    }
}

- (void)URLSession:(NSURLSession *)session task:(NSURLSessionTask *)task didReceiveChallenge:(NSURLAuthenticationChallenge *)challenge completionHandler:(void (^)(NSURLSessionAuthChallengeDisposition disposition, NSURLCredential *credential))completionHandler
{
    //Some servers, including Cyans, support HTTPS on their status feeds, but with self signed certs.
    completionHandler(NSURLSessionAuthChallengeUseCredential, [NSURLCredential credentialForTrust:challenge.protectionSpace.serverTrust]);
}

- (IBAction)donateButtonHit:(id)sender {
}

@end
