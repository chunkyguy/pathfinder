//
//  wl_PathFinder.h
//  PathFinder
//
//  Created by Sid on 13/12/14.
//  Copyright (c) 2014 whackylabs. All rights reserved.
//

#ifndef __PathFinder__wl_PathFinder__
#define __PathFinder__wl_PathFinder__
#include "wl_Map.h"

namespace wl {

    std::list<Tile*> FindPath(const Coordinate &start,
                                   const Coordinate &end,
                                   Map &map);

}

#endif /* defined(__PathFinder__wl_PathFinder__) */
