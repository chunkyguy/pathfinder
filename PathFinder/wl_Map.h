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
#include <GLKit/GLKMath.h>

namespace wl {
    
#pragma mark -
#pragma mark Tile
#pragma mark -
    
    class Tile {
    public:
        Tile(const unsigned int r, const unsigned int c, const bool w);
        const unsigned int GetRow() const;
        const unsigned int GetColumn() const;
        const bool IsWalkable() const;
        
    private:
        unsigned int _row;
        unsigned int _col;
        bool _walkable;
    };
    
#pragma mark -
#pragma mark Map
#pragma mark -

    class Map {
    public:
        Map();
        Map(const GLKVector2 &size, const std::vector<Tile> &data);
        const GLKVector2 GetSize() const;
        
    private:
        GLKVector2 _size;
        std::vector<Tile> _data;
    };

#pragma mark -
#pragma mark Utility
#pragma mark -

    unsigned int TileIndex(const Tile &tile, const Map &map);
    Map CreateRandomMap(const GLKVector2 &size);
}
#endif /* defined(__PathFinder__wl_Map__) */
