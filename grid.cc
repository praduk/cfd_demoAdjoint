/*
* grid.cc
* 
* Copyright (c) 2018 Pradu Kannan. All rights reserved.
*/

#include "grid.hh"
#include "mempool.hh"

State FreeStream = { 290.0, 101325, 1.22, 0, 300.0 };
MemPool<InteriorNode, 1<<16 > iNodeMem;
MemPool<LeafNode, 1<<18 > lNodeMem;
Grid grid;
