//
//  DataAccess.mm
//  names
//
//  Created by Bruce Hill on 2011/03/07.
//  Copyright 2011 Mobility at work. All rights reserved.
//

#import "DataAccess.h"

#define SELECT_STMT @"SELECT TOP 1 START AT %d name FROM Names ORDER BY name FOR UPDATE"

@implementation DataAccess
static DataAccess *        sharedInstance = nil;

- (ULConnection *)createDatabase:(const char *)connectionParms {
    const char *    CREATE_TABLE = 
    "CREATE TABLE Names ("
    "id UNIQUEIDENTIFIER DEFAULT NEWID() PRIMARY KEY,"
    "name VARCHAR(254) NOT NULL)";
    const char *    CREATE_INDEX = 
    "CREATE UNIQUE INDEX namesIndex ON Names(name ASC)";
    const char *    createParms = 
    "page_size=4k;utf8_encoding=true;collation=UTF8BIN";
    ULError            error;
    ULConnection *    conn;
	
    conn = ULDatabaseManager::CreateDatabase(
                                             connectionParms,
                                             createParms,
                                             &error);
    if (!conn) {
        NSLog(@"Error code creating the database: %ld",
              error.GetSQLCode());
    } else {
        NSLog(@"Creating Schema.");
        conn->ExecuteStatement(CREATE_TABLE);
        conn->ExecuteStatement(CREATE_INDEX);
    }
    return conn;
}

- (void)openConnection {
    NSLog(@"Connect to database.");
    if (ULDatabaseManager::Init()) {
        NSArray *	paths = NSSearchPathForDirectoriesInDomains(
																NSDocumentDirectory,
																NSUserDomainMask,
																YES);
        NSString *	documentsDirectory = [paths objectAtIndex:0];
        NSString *	writableDBPath = [documentsDirectory 
									  stringByAppendingPathComponent:
									  @"Names.udb"];
        ULConnection *    conn = nil;
        const char *    connectionParms;
        ULError            error;
		
        connectionParms = [[NSString stringWithFormat:@"DBF=%@",
                            writableDBPath]
						   UTF8String];
        
        // Attempt connection to the database
        conn = ULDatabaseManager::OpenConnection(
                                                 connectionParms,
                                                 &error);
        // If database file not found, create it and create the schema
        if (error.GetSQLCode() == SQLE_ULTRALITE_DATABASE_NOT_FOUND) {
            conn = [self createDatabase:connectionParms];
        }
        connection = conn;
    } else {
        NSLog(@"UL Database Manager initialization failed.");
        connection = nil;
    }
}

+ (DataAccess *)sharedInstance {
    // Create a new instance if none was created yet
    if (sharedInstance == nil) {
        sharedInstance = [[super alloc] init];
        [sharedInstance openConnection];
    }
    
    // Otherwise, just return the existing instance
    return sharedInstance;
}


+ (void)fini {
    [sharedInstance release];
}

- (void)dealloc {
    NSLog(@"Finalizing DB Manager.");
    connection->Close();
    ULDatabaseManager::Fini();
    
    [super dealloc];
}

- (void)addName:(NSString *)name {
    const char *            INSERT = "INSERT INTO Names(name) VALUES(?)";
    ULPreparedStatement *    prepStmt = connection->PrepareStatement(INSERT);
    
    if (prepStmt) {
        // Convert the NSString to a C-Style string using UTF8 Collation
        prepStmt->SetParameterString(1, [name UTF8String], [name length]);
        prepStmt->ExecuteStatement();
        prepStmt->Close();
        connection->Commit();
    } else {
        NSLog(@"Could not prepare INSERT statement.");
    }
}

- (NSInteger)tableView:(UITableView *)tableView 
 numberOfRowsInSection:(NSInteger)section {
    const char *            COUNT = "SELECT COUNT (*) FROM Names";
    ULPreparedStatement *    prepStmt = connection->PrepareStatement(COUNT);
    
    if (prepStmt) {
        ULResultSet    *resultSet = prepStmt->ExecuteQuery();
        int            numberOfNames;
        
        resultSet->First();
        numberOfNames = resultSet->GetInt(1);
        resultSet->Close();
        prepStmt->Close();
        return numberOfNames;
    } else {
        NSLog(@"Couldn't prepare COUNT.");
    }
    
    return 0;
}

- (UITableViewCell *)tableView:(UITableView *)tableView 
         cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    static NSString *        CellIdentifier = @"Cell";
    UITableViewCell *        cell = 
	[tableView dequeueReusableCellWithIdentifier: CellIdentifier];
    ULPreparedStatement *    prepStmt;
    
    if (cell == nil) {
        cell = [[[UITableViewCell alloc] 
                 initWithStyle:UITableViewCellStyleDefault 
                 reuseIdentifier:CellIdentifier] 
				autorelease];
    }
    
    // Make it so the cell cannot be selected. 
    cell.selectionStyle = UITableViewCellSelectionStyleNone;
    
    // +1 to the index since the DB uses a 1-based index rather than 0-based 
    prepStmt = connection->PrepareStatement(
											[[NSString stringWithFormat:SELECT_STMT, indexPath.row + 1] UTF8String]);
    if (prepStmt) {
        ULResultSet *    resultSet = prepStmt->ExecuteQuery();
        char            name[255];
        
        resultSet->First();
        resultSet->GetString("name", name, 255);
        resultSet->Close();
        prepStmt->Close();
        cell.textLabel.text = [NSString stringWithUTF8String:name];
    } else {
        NSLog(@"Couldn't prepare SELECT with index.");
    }
    
    return cell;
}
@end
