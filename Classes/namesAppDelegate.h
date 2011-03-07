//
//  namesAppDelegate.h
//  names
//
//  Created by Bruce Hill on 2011/03/07.
//  Copyright Mobility at work 2011. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface namesAppDelegate : NSObject <UIApplicationDelegate> {
    
    UIWindow *window;
    UINavigationController *navigationController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet UINavigationController *navigationController;

@end

