//
//  wl_PathFinder.cpp
//  PathFinder
//
//  Created by Sid on 13/12/14.
//  Copyright (c) 2014 whackylabs. All rights reserved.
//

#include "wl_PathFinder.h"

namespace wl {
    
    
    std::list<Tile> FindPath(const GLKVector2 &start,
                             const GLKVector2 &end,
                             const wl::Map &map)
    {
        std::list<Tile> path;
        
        std::list<Tile*> open;
        std::list<Tile*> close;
        
        // open.push_back(map.GetTileAtCoordinate(start));
        
        return path;
    }
    
}
