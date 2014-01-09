//
//  he_ViewController.m
//  PathFinder
//
//  Created by Sid on 09/01/14.
//  Copyright (c) 2014 whackylabs. All rights reserved.
//

#import "he_ViewController.h"
#import "astar/pathfinder.h"

#define DEFAULT_ROW 27
#define DEFAULT_COL 21

#define MAP_Y_OFFSET 60

/** Add/subtract some number to convert between index and tag 
 * Why is this? Because it's not necessary that for index=0 we have a TileView::tag=0 available as subview.
 */
#define INDEX_TO_TAG(i) (1000+i)
#define TAG_TO_INDEX(t) (t-1000)

@interface TileView : UILabel
@end

@implementation TileView
- (id)initWithFrame:(CGRect)frame index:(NSInteger)index
{
 self = [super initWithFrame:frame];
 if (self) {
  self.tag = INDEX_TO_TAG(index);
  self.text = [NSString stringWithFormat:@"%d",index];
  self.textAlignment = NSTextAlignmentCenter;
  self.adjustsFontSizeToFitWidth = YES;
 }
 return self;
}

- (void)drawRect:(CGRect)rect
{
 CGContextRef context = UIGraphicsGetCurrentContext();
 CGContextSetStrokeColorWithColor(context, [UIColor blackColor].CGColor);
 CGContextStrokeRectWithWidth(context, self.bounds, 1.0f);
 [super drawRect:rect];
}
@end

@interface he_ViewController () {
 /* Values to be passed to the pathfinder */
 unsigned char *map;
 int touchCount;
 int startX;
 int startY;
 int targetX;
 int targetY;
}
@end

@implementation he_ViewController

- (void)dealloc
{
 if (map) {
  free(map);
 }
}

- (void)viewDidLoad
{
 [super viewDidLoad];
 self.rows.value = DEFAULT_ROW;
 self.columns.value = DEFAULT_COL;
 map = NULL;
 touchCount = 0;
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
 if (map) {
  free(map);
  map = NULL;
 }
 
 unsigned char v;
 map = (unsigned char *)malloc(sizeof(unsigned char) * nRows * nCols);
 for (int r = 0; r < nRows; ++r) {
  for (int c = 0; c < nCols; ++c) {
   v = (rand()%3 > 0) ? 1 : 0;
   printf("%d",v);
   map[COORD_TO_INDEX(r, c, nCols)] = v;
  }
  printf("\n");
 }
}

- (IBAction)updateUI:(id)sender
{
 /* Session configurations. Use these values for debugging any particular case */
 long seed = time(0);
 srand(seed);

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
   NSInteger index = r*nCols+c;
   TileView *tileVw = [[TileView alloc] initWithFrame:CGRectOffset(tileRect, c * tileWidth, r * tileHeight) index:index];
   [self.view addSubview:tileVw];
   tileVw.backgroundColor = (map[index])?[UIColor greenColor]:[UIColor blackColor];
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

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
 UITouch *touch = [touches anyObject];
 CGPoint tchPoint = [touch locationInView:self.view];
 
 /* clamp the values from screen coordinates to map coordinates */
 int r = tchPoint.y/(self.view.bounds.size.height-MAP_Y_OFFSET) * self.rows.value;
 int c = tchPoint.x/self.view.bounds.size.width * self.columns.value;

 /* toggle touch types */
 if (touchCount == 0) {
  /* save start coords*/
  startX = r;
  startY = c;
  /* reset the tile colors */
  for (UIView *tileVw in self.view.subviews) {
   if ([tileVw isKindOfClass:[TileView class]]) {
    NSInteger index = TAG_TO_INDEX(tileVw.tag);
    tileVw.backgroundColor = (map[index])?[UIColor greenColor]:[UIColor blackColor];
   }
  }
  int index = COORD_TO_INDEX(startX, startY, (int)self.columns.value);
  TileView *selectedVw = (TileView *)[self.view viewWithTag:INDEX_TO_TAG(index)];
  selectedVw.backgroundColor = [UIColor blueColor];
 } else if (touchCount == 1) {
  /* save target coords*/
  targetX = r;
  targetY = c;
  [self log:[NSString stringWithFormat:@"{%d,%d} -> {%d,%d}", startX,startY,targetX,targetY] withalert:NO];
  /* get some output buffer */
  int outBufferSz = (int)self.rows.value * (int)self.columns.value;
  int *outBuffer = (int *)malloc(sizeof(int) * outBufferSz);
  
  /* start path finding */
  int length = FindPath(startX, startY,
                     targetX, targetY,
                     map, (int)self.columns.value, (int)self.rows.value,
                     outBuffer, outBufferSz);
  switch (length) {
   case PATH_NONE: [self log:@"No path found" withalert:YES]; break;
   case PATH_BUFOVERFLOW: [self log:@"outBuffer is small" withalert:YES]; break;
   default:
    int tileCount = length + 1;
    [self log:[NSString stringWithFormat:@"path length = %d\ttile count = %d", length, tileCount] withalert:NO];
    /* paint every path tile red */
    for (int i = 0; i < tileCount; ++i) {
     printf("%d ", outBuffer[i]);
     TileView *pathTile = (TileView *)[self.view viewWithTag:INDEX_TO_TAG(outBuffer[i])];
     pathTile.backgroundColor = [UIColor redColor];
    }
    printf("\n");
    break;
  }
  free(outBuffer);
 }

 touchCount = (touchCount+1)%2;
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
