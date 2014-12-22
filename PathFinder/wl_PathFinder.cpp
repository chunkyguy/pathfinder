//
//  wl_PathFinder.cpp
//  PathFinder
//
//  Created by Sid on 13/12/14.
//  Copyright (c) 2014 whackylabs. All rights reserved.
//

#include "wl_PathFinder.h"
#include <iostream>

namespace wl {
    
    
    std::list<Tile*> FindPath(const Coordinate &start,
                                   const Coordinate &end,
                                   Map &map)
    {
        /* get start and end tile*/
        Tile *source = map.GetTileAtCoordinate(start);
        Tile *dest = map.GetTileAtCoordinate(end);
        assert(source != nullptr);
        assert(dest != nullptr);
        
        /* Create open and close lists */
        std::list<Tile*> openList;
        std::list<Tile*> closeList;
        
        /* start is on the non-traversable tile*/
        if (!source->IsWalkable()) {
            std::cout << "Source is on a non-traversable tile" << std::endl;
            return {};
        }
        
        /* target is on the non-traversable tile*/
        if (!dest->IsWalkable()) {
            std::cout << "Destination is on a non-traversable tile" << std::endl;
            return {};
        }
        
        /* already at the target */
        if (start == end) {
            std::cout << "Source is already on destination" << std::endl;
            return {};
        }
        
        /* add start to open list */
        openList.push_back(source);
        
        /* while tiles in open list */
        bool found = false;
        while (!found && !openList.empty()) {
            
            /* get tile with lowest score from open list */
            Tile *current = *(std::min_element(openList.begin(), openList.end(), [](const Tile *tile, const Tile *min){
                return tile->GetScore() < min->GetScore();
            }));
            assert(current);
            
            /* move it from open list to close list */
            openList.remove(current);
            closeList.push_back(current);
            
            /* if the moved tile was target: done */
            if (current->GetCoordinate() == end) {
                found = true;
            } else {
                /* for each walkable tiles */
                std::vector<Coordinate> neighbors = Neighbors(current->GetCoordinate());
                for (std::vector<Coordinate>::const_iterator it = neighbors.begin(); it != neighbors.end(); ++it) {
                    
                    Tile *adjacent = map.GetTileAtCoordinate(*it);
                    
                    /* is tile traversable */
                    if (adjacent == nullptr || !adjacent->IsWalkable()) {
                        continue;
                    }
                    
                    /* if already in close list: ignore */
                    if (std::find(closeList.begin(), closeList.end(), adjacent) != closeList.end()) {
                        continue;
                    }
                    
                    std::list<Tile*>::iterator savedAdjacentIt = std::find(openList.begin(), openList.end(), adjacent);
                    
                    if (savedAdjacentIt != openList.end()) {
                        Tile *savedAdjacent = *savedAdjacentIt;
                        /* if already in open list: test if new score would be lower */
                        if (current->GetDistance() < savedAdjacent->GetDistance()) {
                            /* if yes update parent*/
                            savedAdjacent->UpdateParent(current);
                        }
                    } else {
                        /* if not in open list compute score + set parent + add to open list */
                        adjacent->UpdateParent(current);
                        adjacent->UpdateHeuristics(ManhattanDistance(adjacent->GetCoordinate(), end));
                        openList.push_back(adjacent);
                    }
                }
            }
        }
        
        std::list<Tile*> path;
        if (found) {
            for (Tile *step = dest; step != source; step = step->GetParent()) {
                path.push_back(step);
                
            }
        }
        
        return path;
    }

    
}
