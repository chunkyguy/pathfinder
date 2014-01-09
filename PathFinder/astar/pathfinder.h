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
 Implement a path-finding algorithm in C++ that finds and outputs a shortest path between start and target using the following function declaration:
 int FindPath( const int nStartX, const int nStartY, const int nTargetX, const int nTargetY, const unsigned char* pMap, const int nMapWidth, const int nMapHeight, int* pOutBuffer, const int nOutBufferSize );
 
 - nStartX and nStartY are the coordinates of the start position.
 - nTargetX and nTargetY are the coordinates of the target position.
 - pMap is a nMapWidth * nMapHeight large grid where traversable locations are marked with 1 and non-traversable as 0. Locations are considered to be adjacent horizontally and vertically but not diagonally.
 - pOutBuffer is where you should output the found path and nOutBufferSize is the maximum length of a path that can be written to pOutBuffer. Entries in pOutBuffer are indices into pMap.
 - The function should return the length of the path between Start and Target or -1 if no path exists. If the path is larger than nOutBufferSize, the calling function can either abort or choose to call FindPath again with a larger out buffer.
 - Consider performance, memory usage and that your code may be called from a multi-threaded environment.
 */

/*
 * Assuming coordinates {nStartX, nStartY}, {nTargetX, nTargetY} start with 0 based indices
 *
 * Assuming 'length of the path' == 'number of tiles in path Start -> Target' - 1
 * So, path length between {n,m} and {n,m+1} is 1
 *
 * Returns -2 when path is larger than nOutBufferSize
 * The pOutBuffer has indices in reverse order, from target to source. 
 */

int FindPath(const int nStartX,
             const int nStartY,
             const int nTargetX,
             const int nTargetY,
             const unsigned char* pMap,
             const int nMapWidth,
             const int nMapHeight,
             int* pOutBuffer,
             const int nOutBufferSize);
