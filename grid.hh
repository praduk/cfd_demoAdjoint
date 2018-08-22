/*
* grid.hh
* 
* Copyright (c) 2018 Pradu Kannan. All rights reserved.
*/

#ifndef ___GRID_HH
#define ___GRID_HH

#include <stdio.h>
#include <stdlib.h>

#include "spline.hh"
#include "mempool.hh"

#define MIN_REFINEMENT 5
#define MAX_REFINEMENT 18

#define ICAST(x) static_cast<InteriorNode*>(x)
#define LCAST(x) static_cast<LeafNode*>(x)

struct State
{
    double T;
    double p;
    double rho;
    double vx;
    double vy;
    static void avg(State const& a, State const& b);
};

extern State FreeStream;

struct Node
{
    enum { INTERIOR=0, LEAF };
    enum { C_ROOT=0, C_UR, C_UL, C_DR, C_DL };

    //states
    State s;

    //define geometry
    double x;    //center
    double y;    //center
    double size; //box size

    Node* P; //Parent
    
    char depth; //depth to root
    char link;  //type of link to parent
    char type;  //type

    void setUL(Node* n)
    {
        double hSize = 0.5*n->size;
        double qSize = 0.25*n->size;
        size = hSize;
        x = n->x - qSize;
        y = n->y + qSize;
        P = n;
        link = C_UL;
        depth = n->depth+1;
    }
    void setUR(Node* n)
    {
        double hSize = 0.5*n->size;
        double qSize = 0.25*n->size;
        size = hSize;
        x = n->x + qSize;
        y = n->y + qSize;
        P = n;
        link = C_UR;
        depth = n->depth+1;
    }
    void setDL(Node* n)
    {
        double hSize = 0.5*n->size;
        double qSize = 0.25*n->size;
        size = hSize;
        x = n->x - qSize;
        y = n->y - qSize;
        P = n;
        link = C_DL;
        depth = n->depth+1;
    }
    void setDR(Node* n)
    {
        double hSize = 0.5*n->size;
        double qSize = 0.25*n->size;
        size = hSize;
        x = n->x + qSize;
        y = n->y - qSize;
        P = n;
        link = C_DR;
        depth = n->depth+1;
    }

    bool containsBody()
    {
        double hSize = 0.5*size;
        return  body.inside(x-hSize,y-hSize) ||
                body.inside(x+hSize,y-hSize) ||
                body.inside(x-hSize,y+hSize) ||
                body.inside(x+hSize,y+hSize);
    }

    bool insideBody()
    {
        double hSize = 0.5*size;
        return  body.inside(x-hSize,y-hSize) &&
                body.inside(x+hSize,y-hSize) &&
                body.inside(x-hSize,y+hSize) &&
                body.inside(x+hSize,y+hSize);
    }
};

struct LeafNode;

struct NeighborList
{
    enum { MIXED=0, FREESTREAM, FREE, BODY }; //GLOBAL TYPES

    LeafNode* n[2]; //Pointers to Neighbor Nodes
    char N;    //Number of Neighbors
    char type; //Global Type
};

struct InteriorNode : public Node
{
    //Node Structure
    Node* UL; //Up Left
    Node* UR; //Up Right
    Node* DL; //Down Left
    Node* DR; //Down Right

    InteriorNode() { clear(); }

    void clear()
    {
        type = Node::INTERIOR;
        P  = 0;
        UL = 0;
        UR = 0;
        DL = 0;
        DR = 0;
        x = 0.0;
        y = 0.0;
        size = 1.0;
    }

};

struct LeafNode : public Node
{
    State s;

    NeighborList L;
    NeighborList R;
    NeighborList U;
    NeighborList D;

    //Body Intersection Parameters
    bool bodyNode;
    //double m; //slope
    //double y; //offset

    LeafNode() { clear(); }

    void clear()
    {
        type = Node::LEAF;
        P  = 0;
        x = 0.0;
        y = 0.0;
        size = 1.0;
        bodyNode = false;
    }

    bool needsRefinement()
    {
        return containsBody();
    }

    void calcNeighbors();
};


extern MemPool<InteriorNode, 1<<16 > iNodeMem;
extern MemPool<LeafNode, 1<<18 > lNodeMem;
struct Grid
{
    InteriorNode root;

    Grid()
    {
        root.x = 0.0;
        root.y = 0.0;
        root.size = 20.0;

        root.UL = lNodeMem.alloc();
        root.UR = lNodeMem.alloc();
        root.DL = lNodeMem.alloc();
        root.DR = lNodeMem.alloc();

        static_cast<LeafNode*>(root.UL)->s = FreeStream;
        static_cast<LeafNode*>(root.UR)->s = FreeStream;
        static_cast<LeafNode*>(root.DL)->s = FreeStream;
        static_cast<LeafNode*>(root.DR)->s = FreeStream;

        root.UL->setUL(&root);
        root.UR->setUR(&root);
        root.DL->setDL(&root);
        root.DR->setDR(&root);
    }

    void refine(LeafNode* n)
    {
        n->calcNeighbors();
        //If a neighboring node is less than 1 level refined than us, refine it
        if(n->L.type == NeighborList::MIXED && n->L.n[0]->depth < n->depth)
            refine(n->L.n[0]);
        if(n->R.type == NeighborList::MIXED && n->R.n[0]->depth < n->depth)
            refine(n->R.n[0]);
        if(n->U.type == NeighborList::MIXED && n->U.n[0]->depth < n->depth)
            refine(n->U.n[0]);
        if(n->D.type == NeighborList::MIXED && n->D.n[0]->depth < n->depth)
            refine(n->D.n[0]);
        

        //Refine ourselves
        InteriorNode* in = iNodeMem.alloc();
        InteriorNode* parent = ICAST(n->P);
        if( parent->UL==n )
        {
            parent->UL = in;
            parent->UL->setUL(parent);
        }
        else if( parent->UR==n )
        {
            parent->UR = in;
            parent->UR->setUR(parent);
        }
        else if( parent->DL==n )
        {
            parent->DL = in;
            parent->DL->setDL(parent);
        }
        else if( ICAST(n->P)->DR==n )
        {
            parent->DR = in;
            parent->DR->setDR(parent);
        }

        in->UL = lNodeMem.alloc();
        in->UR = lNodeMem.alloc();
        in->DL = lNodeMem.alloc();
        in->DR = lNodeMem.alloc();
        
        static_cast<LeafNode*>(in->UL)->s = n->s;
        static_cast<LeafNode*>(in->UR)->s = n->s;
        static_cast<LeafNode*>(in->DL)->s = n->s;
        static_cast<LeafNode*>(in->DR)->s = n->s;

        lNodeMem.free(n);
        
        in->UL->setUL(in);
        in->UR->setUR(in);
        in->DL->setDL(in);
        in->DR->setDR(in);

        //Prune Leaves Totally Inside Body
        if( in->UL->insideBody() )
        {
            freeNode(in->UL);
            in->UL = 0;
        }
        if( in->DL->insideBody() )
        {
            freeNode(in->DL);
            in->DL = 0;
        }
        if( in->UR->insideBody() )
        {
            freeNode(in->UR);
            in->UR = 0;
        }
        if( in->DR->insideBody() )
        {
            freeNode(in->DR);
            in->DR = 0;
        }
    }

    void adapt(InteriorNode* n, int depth=1)
    {
        //Refine Interior Nodes one Above Leaf Level
        if( depth<MAX_REFINEMENT )
        {
            if( n->UL && n->UL->type == Node::LEAF && ( depth<MIN_REFINEMENT || LCAST(n->UL)->needsRefinement() ) )
                refine(LCAST(n->UL));
            if( n->DL && n->DL->type == Node::LEAF && ( depth<MIN_REFINEMENT || LCAST(n->DL)->needsRefinement() ) )
                refine(LCAST(n->DL));
            if( n->DR && n->DR->type == Node::LEAF && ( depth<MIN_REFINEMENT || LCAST(n->DR)->needsRefinement() ) )
                refine(LCAST(n->DR));
            if( n->UR && n->UR->type == Node::LEAF && ( depth<MIN_REFINEMENT || LCAST(n->UR)->needsRefinement() ) )
                refine(LCAST(n->UR));
        }

        //Iterate through every node and adapt them
        if( n->UL && n->UL->type == Node::INTERIOR )
            adapt(ICAST(n->UL),depth+1);
        if( n->UR && n->UR->type == Node::INTERIOR )
            adapt(ICAST(n->UR),depth+1);
        if( n->DL && n->DL->type == Node::INTERIOR )
            adapt(ICAST(n->DL),depth+1);
        if( n->DR && n->DR->type == Node::INTERIOR )
            adapt(ICAST(n->DR),depth+1);
    }

    static void freeNode(Node* n)
    {
        if( n->type == Node::INTERIOR)
            iNodeMem.free(static_cast<InteriorNode*>(n));
        else
            lNodeMem.free(static_cast<LeafNode*>(n));
    }

    LeafNode* queryNodeHelper(Node* nin, double x, double y)
    {
        if( !nin )
            return 0;
        if( nin->type == Node::LEAF )
            return LCAST(nin);
        InteriorNode* n = ICAST(nin);
        if( x < n->x )
        {
            if( y < n->y )
                return queryNodeHelper(n->DL,x,y);
            else
                return queryNodeHelper(n->UL,x,y);
        }
        if( y < n->y )
            return queryNodeHelper(n->DR,x,y);
        return queryNodeHelper(n->UR,x,y);
    }

    int queryNode(double x, double y, LeafNode*& n)
    {
        n=0;

        if( x<=-10 )
            return NeighborList::FREESTREAM;
        if( x>=10 || y>=10 || y<=-10 )
            return NeighborList::FREE;

        n = queryNodeHelper(&root,x,y);
        if( !n )
            return NeighborList::BODY;

        return NeighborList::MIXED;
    }
};

extern Grid grid;


#endif  // ___GRID_HH

