/*
* grid.cc
* 
* Copyright (c) 2018 Pradu Kannan. All rights reserved.
*/

#include "grid.hh"
#include "mempool.hh"

MemPool<InteriorNode, 1<<16 > iNodeMem;
MemPool<LeafNode, 1<<18 > lNodeMem;
Grid grid;
