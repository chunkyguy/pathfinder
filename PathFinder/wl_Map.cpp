//
//  wl_Map.cpp
//  PathFinder
//
//  Created by Sid on 13/12/14.
//  Copyright (c) 2014 whackylabs. All rights reserved.
//

#include "wl_Map.h"
#include <cassert>
#include <iostream>

namespace wl {
    
#pragma mark -
#pragma mark Coordinate
#pragma mark -
    Coordinate::Coordinate(const unsigned int r, const unsigned c) :
    _row(r),
    _col(c)
    {}
    
    unsigned int Coordinate::GetRow() const
    {
        return _row;
    }
    
    unsigned int Coordinate::GetColumn() const
    {
        return _col;
    }
    
//    unsigned int Coordinate::GetIndex(const Map &map) const
//    {
//        return _row * map.GetSize().x + _col;
//    }
    
    bool operator==(const Coordinate &a, const Coordinate &b)
    {
        return ((a.GetRow() == b.GetRow()) &&
                a.GetColumn() == b.GetColumn());
    }
    
    std::ostream &operator<<(std::ostream &os, const Coordinate &coord)
    {
        os << "{" << coord.GetRow() << ", " << coord.GetColumn() << "}";
        return os;
    }

    
#pragma mark -
#pragma mark Tile
#pragma mark -
    
    Tile::Tile(const Coordinate &c, const bool w) :
    _coord(c),
    _walkable(w),
    _distance(0),
    _heuristic(0),
    _parent(nullptr)
    {}
    
    const Coordinate Tile::GetCoordinate() const
    {
        return _coord;
    }
    
    const bool Tile::IsWalkable() const
    {
        return _walkable;
    }
    
    bool operator==(const Tile &a, const Tile &b)
    {
        return (a.GetCoordinate() ==  b.GetCoordinate());
    }
    
    int Tile::GetScore() const
    {
        return _distance + _heuristic;
    }
    
    int Tile::GetDistance() const
    {
        return _distance;
    }
    
    void Tile::UpdateParent(Tile *tile)
    {
        _parent = tile;
        _distance = tile->_distance + 1;
    }
    
    void Tile::UpdateHeuristics(const int heuristics)
    {
        _heuristic = heuristics;
    }
    
    Tile *Tile::GetParent() const
    {
        return _parent;
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
    
    const Tile * Map::GetTileAtCoordinate(const Coordinate &coords) const
    {
        std::vector<Tile>::const_iterator result =
        std::find_if(_data.begin(), _data.end(), [&](const Tile &tile){
            return (tile.GetCoordinate() == coords);
        });
        if (result == _data.end()) {
            return nullptr;
        }
        
        return &(*result);
    }
    
    Tile *Map::GetTileAtCoordinate(const Coordinate &coords)
    {
        std::vector<Tile>::iterator result =
        std::find_if(_data.begin(), _data.end(), [&](Tile &tile){
            return (tile.GetCoordinate() == coords);
        });
        if (result == _data.end()) {
            return nullptr;
        }
        
        return &(*result);
    }
    
#pragma mark -
#pragma mark Utility
#pragma mark -
    
    Map CreateRandomMap(const GLKVector2 &size)
    {
        std::vector<Tile> data;
        for (unsigned int r = 0; r < size.x; ++r) {
            for (unsigned int c = 0; c < size.y; ++c) {
                Tile t(Coordinate(r, c), rand()%5!=0);
                data.push_back(t);
            }
        }
        
        return Map(size, data);
    }
    
    std::vector<Coordinate> Neighbors(const Coordinate &coord)
    {
        std::vector<Coordinate> neighbors;
        unsigned int r = coord.GetRow();
        unsigned int c = coord.GetColumn();
        neighbors.push_back(Coordinate(r, c-1)); /* left */
        neighbors.push_back(Coordinate(r-1, c)); /* top */
        neighbors.push_back(Coordinate(r, c+1)); /* right */
        neighbors.push_back(Coordinate(r+1, c)); /* bottom */
        return neighbors;
    }

    int ManhattanDistance(const Coordinate &a, const Coordinate &b)
    {
        int rdiff = b.GetRow() - a.GetRow();
        rdiff *= (rdiff < 0) ? -1 : 1;
        int cdiff = b.GetColumn() - a.GetColumn();
        cdiff *= (cdiff < 0) ? -1 : 1;
        return rdiff + cdiff;
    }
}