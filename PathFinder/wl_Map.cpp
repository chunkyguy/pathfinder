//
//  wl_Map.cpp
//  PathFinder
//
//  Created by Sid on 13/12/14.
//  Copyright (c) 2014 whackylabs. All rights reserved.
//

#include "wl_Map.h"

namespace wl {
    
#pragma mark -
#pragma mark Tile
#pragma mark -
    
    Tile::Tile(const unsigned int r, const unsigned int c, const bool w) :
    _row(r),
    _col(c),
    _walkable(w)
    {}
    
    const unsigned int Tile::GetRow() const
    {
        return _row;
    }
    
    const unsigned int Tile::GetColumn() const
    {
        return _col;
    }
    
    const bool Tile::IsWalkable() const
    {
        return _walkable;
    }
    
#pragma mark -
#pragma mark Map
#pragma mark -
    
    Map::Map() :
    _size(GLKVector2Make(0, 0)),
    _data()
    {}
    
    Map::Map(const GLKVector2 &size, const std::vector<Tile> &data) :
    _size(size),
    _data(data)
    {}
    
    const GLKVector2 Map::GetSize() const
    {
        return _size;
    }
    
#pragma mark -
#pragma mark Utility
#pragma mark -
    
    unsigned int TileIndex(const Tile &tile, const Map &map)
    {
        return tile.GetRow() * map.GetSize().x + tile.GetColumn();
    }
    
    Map CreateRandomMap(const GLKVector2 &size)
    {
        std::vector<Tile> data;
        for (unsigned int r = 0; r < size.x; ++r) {
            for (unsigned int c = 0; c < size.y; ++c) {
                Tile t(r, c, rand()%2==0);
                data.push_back(t);
            }
        }
        
        return Map(size, data);
    }
    
}