//
//  pathfinder.h
//  PathFinder
//
//  Created by Sid on 09/01/14.
//  Copyright (c) 2014 whackylabs. All rights reserved.
//

#define PATH_NONE -1
#define PATH_BUFOVERFLOW -2

/** Get map index from map coordinates
 * @param row The row of the map coordinate
 * @param col The column of the map coordinate
 * @param width The width of the map
 * @note The index not guaranteed to be a valid.
 */
#define COORD_TO_INDEX(row, col, width) ((row*width) + col)

/*
 * Shortest path between start and target
 * @param nStartX and nStartY are the coordinates of the start position.
 * @param nTargetX and nTargetY are the coordinates of the target position.
 * @param pMap is a nMapWidth * nMapHeight large grid where traversable locations are marked with 1 and non-traversable as 0. Locations are considered to be adjacent horizontally and vertically but not diagonally.
 * @param pOutBuffer is where you should output the found path and nOutBufferSize is the maximum length of a path that can be written to pOutBuffer. Entries in pOutBuffer are indices into pMap.
 * @return The length of the path between Start and Target. PATH_NONE if no path exists. PATH_BUFOVERFLOW if pOutBuffer is smaller than output.
*/
int FindPath(const int startX,
             const int startY,
             const int targetX,
             const int targetY,
             const unsigned char* map,
             const int mapWidth,
             const int mapHeight,
             int* outBuffer,
             const int outBufferSize);
