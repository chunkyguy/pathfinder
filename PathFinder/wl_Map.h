//
//  wl_Map.h
//  PathFinder
//
//  Created by Sid on 13/12/14.
//  Copyright (c) 2014 whackylabs. All rights reserved.
//

#ifndef __PathFinder__wl_Map__
#define __PathFinder__wl_Map__
#include <vector>
#include <iosfwd>
#include <GLKit/GLKMath.h>
#include <list>

namespace wl {

#pragma mark -
#pragma mark Coordinate
#pragma mark -
    
    class Coordinate {
    public:
        Coordinate(const unsigned int r = 0, const unsigned c = 0);
        unsigned int GetRow() const;
        unsigned int GetColumn() const;
        
    private:
        unsigned int _row;
        unsigned int _col;
    };

    bool operator==(const Coordinate &a, const Coordinate &b);
    std::ostream &operator<<(std::ostream &os, const Coordinate &coord);
    
#pragma mark -
#pragma mark Tile
#pragma mark -
    
    class Tile {
    public:
        Tile(const Coordinate &c, const bool w);
        const Coordinate GetCoordinate() const;
        const bool IsWalkable() const;
        int GetScore() const;
        int GetDistance() const;
        Tile *GetParent() const;
        void UpdateParent(Tile *tile);
        void UpdateHeuristics(const int heuristics);
        
    private:

        Coordinate _coord;
        bool _walkable;
        int _distance;
        int _heuristic;
        Tile *_parent;
    };
    
    bool operator==(const Tile &a, const Tile &b);
    
#pragma mark -
#pragma mark Map
#pragma mark -

    class Map {
    public:
        Map();
        Map(const GLKVector2 &size, const std::vector<Tile> &data);
        const GLKVector2 GetSize() const;
        const Tile *GetTileAtCoordinate(const Coordinate &coords) const;
        Tile *GetTileAtCoordinate(const Coordinate &coords);
        
    private:
        GLKVector2 _size;
        std::vector<Tile> _data;
    };

#pragma mark -
#pragma mark Utility
#pragma mark -

    /* Manhattan distance */
    int ManhattanDistance(const Coordinate &a, const Coordinate &b);
    
    /* neighbour coordinates */
    std::vector<Coordinate> Neighbors(const Coordinate &coord);
    
    unsigned int TileIndex(const Tile &tile, const Map &map);
    
    Map CreateRandomMap(const GLKVector2 &size);
}
#endif /* defined(__PathFinder__wl_Map__) */
