//
//  NewNameViewController.h
//  names
//
//  Created by Bruce Hill on 2011/03/07.
//  Copyright 2011 Mobility at work. All rights reserved.
//

#import <UIKit/UIKit.h>


@interface NewNameViewController : UIViewController {
	UITextField *newNameField; 
} 
@property (retain) IBOutlet UITextField *newNameField; 
- (IBAction)doneAdding:(id)sender; 

@end
