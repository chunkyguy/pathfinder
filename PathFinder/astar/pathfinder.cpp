//
//  pathfinder.cpp
//  PathFinder
//
//  Created by Sid on 09/01/14.
//  Copyright (c) 2014 whackylabs. All rights reserved.
//

/* Using A* path finding algorithm */
#include "pathfinder.h"

/* MARK: Tile */
typedef struct Tile_ {
 int row, col;
 int distance;
 int heuristic;
 struct Tile_ *parent;
 struct Tile_ *next;
} Tile;

/** Initialize the tile
 * @param t Tile
 * @param r row coordinate
 * @param c column coordinate
 */
static void Tile_Init(Tile *t, const int r, const int c)
{
 t->row = r;
 t->col = c;
 t->distance = 0;
 t->heuristic = 0;
 t->parent = 0;
 t->next = 0;
}

/** Calculate total score for a tile*/
static int Tile_Score(const Tile *t)
{
 return t->distance + t->heuristic;
}

/** Tests whether two tiles are at same location */
static bool Tile_Equal(const Tile *one, const Tile *two)
{
 return ((one->row == two->row) && (one->col == two->col));
}

/** Tests whether the tile is traversalable
 * @param t Tile
 * @param map Map
 * @param mapWidth map width
 * @param mapHeight mapHeight
 */
static bool Tile_IsTraversable(const Tile *t, const unsigned char *map, const int mapWidth, const int mapHeight)
{
 /*test if tile is within map*/
 if (t->row < 0 || t->col < 0 || t->row >= mapHeight || t->col >= mapWidth) {
  return false;
 }
 /* test if tile is walkable */
 int index = COORD_TO_INDEX(t->row, t->col, mapWidth);
 return (map[index] != 0);
}

/** Distance between tiles */
static int Tile_Distance(const Tile *start, const Tile *end)
{
 return 1;
}

/** The manhattan distance between two tiles */
static int Tile_Heuristic(const Tile *start, const Tile *end)
{
 int rdiff = end->row - start->row;
 rdiff *= (rdiff < 0) ? -1 : 1;
 int cdiff = end->col - start->col;
 cdiff *= (cdiff < 0) ? -1 : 1;
 return rdiff + cdiff;
}

/* MARK: List */
/** List is a collection of tiles */
typedef struct List_ {
 Tile *head;
 int size;
} List;

/** Initialize default values of a List */
void List_Init(List *l)
{
 l->head = 0;
 l->size = 0;
}

/** Add a Tile object to the list */
static void List_Add(List *l, Tile *t)
{
 l->size++;
 if (!l->head) {
  l->head = t;
  return;
 }
 t->next = l->head;
 l->head = t;
}

/** Remove the tile object from the list */
static void List_Remove(List *l, Tile *t)
{
 l->size--;
 if (Tile_Equal(t, l->head)) {
  l->head = l->head->next;
  return;
 }

 Tile *pre;
 for (pre = l->head; pre && pre->next != t; pre = pre->next) {
 }
 pre->next = pre->next->next;
}

/** Search and return the tile in the list with lowest score */
static Tile *List_SearchLowest(List *l)
{
 Tile *lowest = l->head;
 for (Tile *t = l->head->next; t; t = t->next) {
  if (Tile_Score(t) < Tile_Score(lowest)) {
   lowest = t;
  }
 }
 return lowest;
}

/** Test whether the list contains a tile 
 * returns if found the tile from list
 * else returns NULL
 */
static Tile *List_Contains(List *l, Tile *t)
{
 for (Tile *lt = l->head; lt; lt = lt->next) {
  if (Tile_Equal(lt, t)) {
   return lt;
  }
 }
 return 0;
}

/* MARK: Pathfinder */
int FindPath(const int startX,
                 const int startY,
                 const int targetX,
                 const int targetY,
                 const unsigned char* map,
                 const int mapWidth,
                 const int mapHeight,
                 int* outBuffer,
                 const int outBufferSize)
{
 /* Create open and close lists */
 List openList, closeList;
 List_Init(&openList);
 List_Init(&closeList);

 /* Create a tile buffer where all the tiles live 
  * Extra 2 is for start and target tile that we fill
  */
 int tbufferSize = outBufferSize+2;
 Tile tbuffer[tbufferSize];
 int tbufIndex = 0;

 /* Get the start tile */
 if (tbufIndex >= tbufferSize) {  return PATH_BUFOVERFLOW; }
 Tile *start = &tbuffer[tbufIndex++];
 Tile_Init(start, startX, startY);

 /* Get the target tile */
 if (tbufIndex >= tbufferSize) {  return PATH_BUFOVERFLOW; }
 Tile *target = &tbuffer[tbufIndex++];
 Tile_Init(target, targetX, targetY);

 /*start is on the non-traversable tile*/
 if (!Tile_IsTraversable(start, map, mapWidth, mapHeight)) {
  return PATH_NONE;
 }

 /*target is on the non-traversable tile*/
 if (!Tile_IsTraversable(target, map, mapWidth, mapHeight)) {
  return PATH_NONE;
 }

 /*already at the target*/
 if (Tile_Equal(start, target)) {
  outBuffer[0] = COORD_TO_INDEX(start->row, start->col, mapWidth);
  return 0;
 }

 /*add start to open list*/
 List_Add(&openList, start);
 
 /*while tiles in open list*/
 while (openList.size > 0) {

  /*get tile with lowest score from open list*/
  Tile *current = List_SearchLowest(&openList);

  /*move it from open list to close list*/
  List_Remove(&openList, current);
  List_Add(&closeList, current);
  
 /*if the moved tile was target: done*/
  if (Tile_Equal(current, target)) {
   int length = 0;
   for (Tile *path = current; path; path = path->parent) {
    outBuffer[length++] = COORD_TO_INDEX(path->row, path->col, mapWidth);
   }
   return length-1; /*balance the extra post-increment*/
  }
  
  /*for each walkable tiles*/
  for (int i = 0; i < 4; ++i) {
   if (tbufIndex >= tbufferSize) {  return PATH_BUFOVERFLOW; }
   Tile *adjacent = &tbuffer[tbufIndex++];
   switch (i) {
    case 0: Tile_Init(adjacent, current->row, current->col-1);  break; /*left*/
    case 1: Tile_Init(adjacent, current->row-1, current->col);  break; /*top*/
    case 2: Tile_Init(adjacent, current->row, current->col+1);  break; /*right*/
    case 3: Tile_Init(adjacent, current->row+1, current->col);  break; /*bottom*/
   }
   
   /*is tile valid*/
   if (!Tile_IsTraversable(adjacent, map, mapWidth, mapHeight)) {
    tbufIndex--;
    continue;
   }
   
   /*if already in close list: ignore*/
   if (List_Contains(&closeList, adjacent)){
    tbufIndex--;
    continue;
   }
   
   int distance = Tile_Distance(current, adjacent);
   Tile *savedAdjacent = List_Contains(&openList, adjacent);

   /* if already in open list: test if new score would be lower | if yes update parent*/
   if (savedAdjacent) {
    tbufIndex--;
    if (current->distance + distance < savedAdjacent->distance) {
     adjacent = savedAdjacent;
     adjacent->parent = current;
     adjacent->distance = current->distance + distance;
    }
   } else {
    /* else if not in open list compute score | set parent | add to open list */
    adjacent->parent = current;
    adjacent->distance = current->distance + distance;
    adjacent->heuristic = Tile_Heuristic(adjacent, target);
    List_Add(&openList, adjacent);
   }
  }
 }

 return PATH_NONE;
}
