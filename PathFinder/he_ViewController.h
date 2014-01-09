//
//  he_ViewController.h
//  PathFinder
//
//  Created by Sid on 09/01/14.
//  Copyright (c) 2014 whackylabs. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface he_ViewController : UIViewController
@property (weak, nonatomic) IBOutlet UIStepper *columns;
@property (weak, nonatomic) IBOutlet UIStepper *rows;
@property (weak, nonatomic) IBOutlet UILabel *columnLbl;
@property (weak, nonatomic) IBOutlet UILabel *rowLbl;
- (IBAction)updateUI:(id)sender;

@end
