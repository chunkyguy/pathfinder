//
//  wl_ViewController.m
//  PathFinder
//
//  Created by Sid on 09/01/14.
//  Copyright (c) 2014 whackylabs. All rights reserved.
//

#import "wl_ViewController.h"
#include <iostream>
#import "astar/pathfinder.h"
#import "wl_Map.h"
#import "wl_PathFinder.h"

#define DEFAULT_ROW 27
#define DEFAULT_COL 21

#define MAP_Y_OFFSET 60

void LogPath(const std::list<wl::Tile *> &path)
{
    std::cout << "path-size: " << path.size() << std::endl;
    std::for_each(path.begin(), path.end(), [](const wl::Tile *tile) {
        std::cout << tile->GetCoordinate() << std::endl;
    });
}


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
    wl::Coordinate _lastTouch;
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
            
            tileVw.backgroundColor = (_map.GetTileAtCoordinate(wl::Coordinate(r, c))->IsWalkable()) ? [UIColor brownColor] : [UIColor blueColor];
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

- (BOOL)path:(const std::list<wl::Tile *> &)path contains:(wl::Coordinate)coordinate
{
    return (std::find_if(path.begin(), path.end(), [&](const wl::Tile *pathTile){
        return pathTile->GetCoordinate() == coordinate;
    }) != path.end());
}

- (void)drawMapWithSource:(wl::Coordinate *)source
              destination:(wl::Coordinate *)destination
                     path:(const std::list<wl::Tile *> &)path
{
    /* reset the tile colors */
    for (UIView *view in self.view.subviews) {
        if ([view isKindOfClass:[TileView class]]) {
            TileView *tileVw = (TileView *)view;
            tileVw.backgroundColor = (_map.GetTileAtCoordinate([tileVw coordinates])->IsWalkable()) ? [UIColor brownColor] : [UIColor blueColor];
            if ((source != nullptr) && [tileVw coordinates] == *source) {
                tileVw.backgroundColor = [UIColor whiteColor];
            } else if ((destination != nullptr) && [tileVw coordinates] == *destination) {
                tileVw.backgroundColor = [UIColor blackColor];
            } else if (!path.empty() && [self path:path contains:[tileVw coordinates]]) {
                tileVw.backgroundColor = [UIColor orangeColor];
            }
        }
    }
}

- (void)drawMapWithSource:(wl::Coordinate *)source
              destination:(wl::Coordinate *)destination
{
    [self drawMapWithSource:source destination:destination path:{}];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];
    CGPoint tchPoint = [touch locationInView:self.view];
    
    /* clamp the values from screen coordinates to map coordinates */
    wl::Coordinate touchCoord(tchPoint.y/(self.view.bounds.size.height-MAP_Y_OFFSET) * self.rows.value,
                              tchPoint.x/self.view.bounds.size.width * self.columns.value);
    
    /* toggle touch types */
    if (_touchCount == 0) {
        [self drawMapWithSource:&touchCoord destination:nullptr];
    } else if (_touchCount == 1) {
        std::list<wl::Tile *> path = wl::FindPath(_lastTouch, touchCoord, _map);
        std::for_each(path.begin(), path.end(), [](const wl::Tile *tile) {
            std::cout << tile->GetCoordinate() << std::endl;
        });
        //        LogPath(path);
        [self drawMapWithSource:&_lastTouch destination:&touchCoord path:path];
    }
    
    _lastTouch = touchCoord;
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
