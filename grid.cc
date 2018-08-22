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

void LeafNode::calcNeighbors()
{
    if( containsBody() )
        bodyNode = true;
    else
        bodyNode = false;

    LeafNode* n;

    //Calculate Left Neighbors
    L.N=0;
    L.type = grid.queryNode(x-0.75*size, y-0.25*size,n);
    if(L.type == NeighborList::MIXED )
    {
        L.n[0]=n;
        L.N=1;
        grid.queryNode(x-0.75*size, y+0.25*size,n);
        if( n && n!=L.n[0] )
        {
            L.n[1]=n;
            L.N=2;
        }
    }

    //Calculate Right Neighbors
    R.N=0;
    R.type = grid.queryNode(x+0.75*size, y-0.25*size,n);
    if(R.type == NeighborList::MIXED )
    {
        R.n[0]=n;
        R.N=1;
        grid.queryNode(x+0.75*size, y+0.25*size,n);
        if( n && n!=R.n[0] )
        {
            R.n[1]=n;
            R.N=2;
        }
    }

    //Calculate Up Neighbors
    U.N=0;
    U.type = grid.queryNode(x+0.25*size, y-0.75*size,n);
    if(U.type == NeighborList::MIXED )
    {
        U.n[0]=n;
        U.N=1;
        grid.queryNode(x-0.25*size, y-0.75*size,n);
        if( n && n!=U.n[0] )
        {
            U.n[1]=n;
            U.N=2;
        }
    }

    //Calculate Down Neighbors
    D.N=0;
    D.type = grid.queryNode(x+0.25*size, y+0.75*size,n);
    if(D.type == NeighborList::MIXED )
    {
        D.n[0]=n;
        D.N=1;
        grid.queryNode(x-0.25*size, y+0.75*size,n);
        if( n && n!=D.n[0] )
        {
            D.n[1]=n;
            D.N=2;
        }
    }
}
