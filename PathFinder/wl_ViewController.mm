//
//  wl_ViewController.m
//  PathFinder
//
//  Created by Sid on 09/01/14.
//  Copyright (c) 2014 whackylabs. All rights reserved.
//

#import "wl_ViewController.h"
#import "astar/pathfinder.h"
#import "wl_Map.h"
#import "wl_PathFinder.h"

#define DEFAULT_ROW 27
#define DEFAULT_COL 21

#define MAP_Y_OFFSET 60

///** Add/subtract some number to convert between index and tag
// * Why is this? Because it's not necessary that for index=0 we have a TileView::tag=0 available as subview.
// */
//#define INDEX_TO_TAG(i) (1000+i)
//#define TAG_TO_INDEX(t) (t-1000)

@interface TileView : UILabel {
    wl::Coordinate _coord;
}
@end

@implementation TileView
- (id)initWithFrame:(CGRect)frame coordinate:(wl::Coordinate)coord
{
    self = [super initWithFrame:frame];
    if (self) {
        self.textAlignment = NSTextAlignmentCenter;
        self.adjustsFontSizeToFitWidth = YES;
        _coord = coord;
    }
    return self;
}

- (wl::Coordinate)coordinates
{
    return _coord;
}

- (void)drawRect:(CGRect)rect
{
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGContextSetStrokeColorWithColor(context, [UIColor blackColor].CGColor);
    CGContextStrokeRectWithWidth(context, self.bounds, 1.0f);
    [super drawRect:rect];
}
@end

@interface wl_ViewController () {
    /* Values to be passed to the pathfinder */
    int _touchCount;
    wl::Coordinate _source;
    wl::Coordinate _destination;
    wl::Map _map;
}
@end

@implementation wl_ViewController

- (void)dealloc
{
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.rows.value = DEFAULT_ROW;
    self.columns.value = DEFAULT_COL;
    _touchCount = 0;
    [self updateUI:nil];
    [self updateStepper:self.rows];
    [self updateStepper:self.columns];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

/** Create a random map */
- (void)createMapWithRows:(int)nRows cols:(int)nCols
{
    _map = wl::CreateRandomMap(GLKVector2Make(nRows, nCols));
}

- (IBAction)updateUI:(id)sender
{
    /* Session configurations. Use these values for debugging any particular case */
    long seed = time(nullptr);
    srand(static_cast<unsigned int>(seed));
    
    int nRows = (int)self.rows.value;
    int nCols = (int)self.columns.value;
    [self log:[NSString stringWithFormat:@"seed = %ld rows = %d cols = %d",seed, nRows, nCols] withalert:NO];
    
    /** Create a new map*/
    [self createMapWithRows:nRows cols:nCols];
    
    /** Remove any added tiles */
    for (UIView *vw in self.view.subviews) {
        if ([vw isKindOfClass:[TileView class]]) {
            [vw removeFromSuperview];
        }
    }
    
    /* Add tiles on the view */
    CGFloat tileWidth = self.view.bounds.size.width / self.columns.value;
    CGFloat tileHeight = (self.view.bounds.size.height-MAP_Y_OFFSET) / self.rows.value;
    CGRect tileRect = CGRectMake(0, 0, tileWidth, tileHeight);
    
    for (int c = 0; c < nCols; ++c) {
        for (int r = 0; r < nRows; ++r) {
            TileView *tileVw = [[TileView alloc] initWithFrame:CGRectOffset(tileRect, c * tileWidth, r * tileHeight) coordinate:wl::Coordinate(r, c)];
            [self.view addSubview:tileVw];
            
            tileVw.backgroundColor = (_map.GetTileAtCoordinate(wl::Coordinate(r, c)).IsWalkable()) ? [UIColor brownColor] : [UIColor blueColor];
        }
    }
}

- (IBAction)updateStepper:(UIStepper *)sender {
    if (sender == self.rows) {
        self.rowLbl.text = [NSString stringWithFormat:@"%d",(int)sender.value];
    } else {
        self.columnLbl.text = [NSString stringWithFormat:@"%d",(int)sender.value];
    }
}

- (void)drawMap
{
    /* reset the tile colors */
    for (UIView *view in self.view.subviews) {
        if ([view isKindOfClass:[TileView class]]) {
            TileView *tileVw = (TileView *)view;
            tileVw.backgroundColor = (_map.GetTileAtCoordinate([tileVw coordinates]).IsWalkable()) ? [UIColor brownColor] : [UIColor blueColor];
            if ([tileVw coordinates] == _source) {
                tileVw.backgroundColor = [UIColor orangeColor];
            } else if ([tileVw coordinates] == _destination) {
                tileVw.backgroundColor = [UIColor purpleColor];
            }
        }
    }
}



- (void)drawPath
{
    /* save target coords*/
    //        [self log:[NSString stringWithFormat:@"{%d,%d} -> {%d,%d}", startX,startY,targetX,targetY] withalert:NO];
    
    
    //        /* get some output buffer */
    //        int outBufferSz = (int)self.rows.value * (int)self.columns.value;
    //        int *outBuffer = (int *)malloc(sizeof(int) * outBufferSz);
    //
    //        /* start path finding */
    //        int length = FindPath(startX, startY,
    //                              targetX, targetY,
    //                              _map, (int)self.columns.value, (int)self.rows.value,
    //                              outBuffer, outBufferSz);
    //        switch (length) {
    //            case PATH_NONE: [self log:@"No path found" withalert:YES]; break;
    //            case PATH_BUFOVERFLOW: [self log:@"outBuffer is small" withalert:YES]; break;
    //            default:
    //                int tileCount = length + 1;
    //                [self log:[NSString stringWithFormat:@"path length = %d\ttile count = %d", length, tileCount] withalert:NO];
    //                /* paint every path tile red */
    //                for (int i = 0; i < tileCount; ++i) {
    //                    printf("%d ", outBuffer[i]);
    //                    TileView *pathTile = (TileView *)[self.view viewWithTag:INDEX_TO_TAG(outBuffer[i])];
    //                    pathTile.backgroundColor = [UIColor redColor];
    //                }
    //                printf("\n");
    //                break;
    //        }
    //        free(outBuffer);
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];
    CGPoint tchPoint = [touch locationInView:self.view];
    
    /* clamp the values from screen coordinates to map coordinates */
    int r = tchPoint.y/(self.view.bounds.size.height-MAP_Y_OFFSET) * self.rows.value;
    int c = tchPoint.x/self.view.bounds.size.width * self.columns.value;
    
    /* toggle touch types */
    if (_touchCount == 0) {
        _source = wl::Coordinate(r, c);
    } else if (_touchCount == 1) {
        _destination = wl::Coordinate(r, c);
    }
    [self drawMap];
    
    _touchCount = (_touchCount+1)%2;
}

/** Log message to console. Optionally bring alertView on screen */
- (void)log:(NSString *)message withalert:(BOOL)shouldAlert
{
    printf("%s\n",[message cStringUsingEncoding:NSASCIIStringEncoding]);
    if (shouldAlert) {
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"!!" message:message delegate:nil cancelButtonTitle:@"Dismiss" otherButtonTitles:nil];
        [alert show];
    }
}
@end
