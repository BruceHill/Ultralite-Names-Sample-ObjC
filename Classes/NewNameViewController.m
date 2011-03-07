//
//  NewNameViewController.m
//  names
//
//  Created by Bruce Hill on 2011/03/07.
//  Copyright 2011 Mobility at work. All rights reserved.
//

#import "NewNameViewController.h"
#import "DataAccess.h"

@implementation NewNameViewController


@synthesize newNameField; 
- (IBAction)doneAdding:(id)sender 
{
	if (newNameField.text > 0) { 
        [[DataAccess sharedInstance] addName:newNameField.text]; 
    } 
    [self.navigationController popViewControllerAnimated:YES]; 
} 

/*
 // The designated initializer.  Override if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if ((self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])) {
        // Custom initialization
    }
    return self;
}
*/


// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
    [super viewDidLoad]; 
    
    // Set the title to display in the nav bar 
    self.title = @"Add Name"; 
    
    // Set the text field to the first responder to display the keyboard. 
    // Without this the user needs to tap on the text field. 
    [newNameField becomeFirstResponder]; 	
}


/*
// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
*/

- (void)didReceiveMemoryWarning {
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}


- (void)dealloc {
    [super dealloc];
}


@end
