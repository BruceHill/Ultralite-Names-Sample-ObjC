//
//  DataAccess.h
//  names
//
//  Created by Bruce Hill on 2011/03/07.
//  Copyright 2011 Mobility at work. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "ulcpp.h" 

@interface DataAccess : NSObject <UITableViewDataSource> {
	ULConnection *     connection;
}

// Release objects.
- (void)dealloc;

// Singleton instance of the DataAccess class. 
+ (DataAccess*)sharedInstance; 

// Finalize the Database Manager when done with the DB.
+ (void)fini;

// Adds the given name to the database. 
- (void)addName:(NSString *)name;
@end