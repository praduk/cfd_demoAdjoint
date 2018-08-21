/*
* grid.hh
* 
* Copyright (c) 2018 Pradu Kannan. All rights reserved.
*/

#ifndef ___GRID_HH
#define ___GRID_HH

#include <stdio.h>

#include "spline.hh"
#include "mempool.hh"

#define MIN_REFINEMENT 5
#define MAX_REFINEMENT 17

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

    bool needsRefinement()
    {
        return containsBody();
    }

    void calcNeighbors()
    {
    }
};

struct LeafNode;

struct NeighborList
{
    enum { CALC=0, FREESTREAM, FREE, BODY };
    LeafNode* n[2]; //Pointers to Neighbor Nodes
    char d[2]; //depth of neighbor nodes
    char t[2]; //type of neighbor node
    char N;    //Number of Neighbors
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

    //NeighborList getLeft()
    //{
    //    NeighborList ret;

    //    if( link == C_UR )
    //    {
    //        ret.N=1;
    //        ret.n[0] = P->UL;
    //        return ret;
    //    }
    //    if( link == C_DR )
    //    {
    //        ret.N = 1;
    //        ret.n[0] = P->DL;
    //    }
    //    if( link == C_UL )
    //    {
    //        if( P->link == )
    //    }

    //    return ret;
    //}
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

    void adapt(InteriorNode* n, int depth=1)
    {
        //Refine Interior Nodes one Above Leaf Level
        if( depth<MAX_REFINEMENT )
        {
            if( n->UL->type == Node::LEAF && ( depth<MIN_REFINEMENT || n->UL->needsRefinement() ) )
            {
                InteriorNode* in = iNodeMem.alloc();
                LeafNode* ln = static_cast<LeafNode*>(n->UL);
                n->UL = in;
                n->UL->setUL(n);

                in->UL = lNodeMem.alloc();
                in->UR = lNodeMem.alloc();
                in->DL = lNodeMem.alloc();
                in->DR = lNodeMem.alloc();

                static_cast<LeafNode*>(in->UL)->s = ln->s;
                static_cast<LeafNode*>(in->UR)->s = ln->s;
                static_cast<LeafNode*>(in->DL)->s = ln->s;
                static_cast<LeafNode*>(in->DR)->s = ln->s;

                in->UL->setUL(in);
                in->UR->setUR(in);
                in->DL->setDL(in);
                in->DR->setDR(in);

                in->UL->calcNeighbors();
                in->UR->calcNeighbors();
                in->DL->calcNeighbors();
                in->DR->calcNeighbors();

                lNodeMem.free(ln);
            }
            if( n->DL->type == Node::LEAF && ( depth<MIN_REFINEMENT || n->DL->needsRefinement() ) )
            {
                InteriorNode* in = iNodeMem.alloc();
                LeafNode* ln = static_cast<LeafNode*>(n->DL);
                n->DL = in;
                n->DL->setDL(n);

                in->UL = lNodeMem.alloc();
                in->UR = lNodeMem.alloc();
                in->DL = lNodeMem.alloc();
                in->DR = lNodeMem.alloc();

                static_cast<LeafNode*>(in->UL)->s = ln->s;
                static_cast<LeafNode*>(in->UR)->s = ln->s;
                static_cast<LeafNode*>(in->DL)->s = ln->s;
                static_cast<LeafNode*>(in->DR)->s = ln->s;

                in->UL->setUL(in);
                in->UR->setUR(in);
                in->DL->setDL(in);
                in->DR->setDR(in);

                in->UL->calcNeighbors();
                in->UR->calcNeighbors();
                in->DL->calcNeighbors();
                in->DR->calcNeighbors();

                lNodeMem.free(ln);
            }
            if( n->DR->type == Node::LEAF && ( depth<MIN_REFINEMENT || n->DR->needsRefinement() ) )
            {
                InteriorNode* in = iNodeMem.alloc();
                LeafNode* ln = static_cast<LeafNode*>(n->DR);
                n->DR = in;
                n->DR->setDR(n);

                in->UL = lNodeMem.alloc();
                in->UR = lNodeMem.alloc();
                in->DL = lNodeMem.alloc();
                in->DR = lNodeMem.alloc();

                static_cast<LeafNode*>(in->UL)->s = ln->s;
                static_cast<LeafNode*>(in->UR)->s = ln->s;
                static_cast<LeafNode*>(in->DL)->s = ln->s;
                static_cast<LeafNode*>(in->DR)->s = ln->s;

                in->UL->setUL(in);
                in->UR->setUR(in);
                in->DL->setDL(in);
                in->DR->setDR(in);

                in->UL->calcNeighbors();
                in->UR->calcNeighbors();
                in->DL->calcNeighbors();
                in->DR->calcNeighbors();

                lNodeMem.free(ln);
            }
            if( n->UR->type == Node::LEAF && ( depth<MIN_REFINEMENT || n->UR->needsRefinement() ) )
            {
                InteriorNode* in = iNodeMem.alloc();
                LeafNode* ln = static_cast<LeafNode*>(n->UR);
                n->UR = in;
                n->UR->setUR(n);

                in->UL = lNodeMem.alloc();
                in->UR = lNodeMem.alloc();
                in->DL = lNodeMem.alloc();
                in->DR = lNodeMem.alloc();

                static_cast<LeafNode*>(in->UL)->s = ln->s;
                static_cast<LeafNode*>(in->UR)->s = ln->s;
                static_cast<LeafNode*>(in->DL)->s = ln->s;
                static_cast<LeafNode*>(in->DR)->s = ln->s;

                in->UL->setUL(in);
                in->UR->setUR(in);
                in->DL->setDL(in);
                in->DR->setDR(in);

                in->UL->calcNeighbors();
                in->UR->calcNeighbors();
                in->DL->calcNeighbors();
                in->DR->calcNeighbors();

                lNodeMem.free(ln);
            }
        }

        //Prune Leaves Totally Inside Body
        if( n->UL->insideBody() )
        {
            freeNode(n->UL);
            n->UL = 0;
        }
        if( n->DL->insideBody() )
        {
            freeNode(n->DL);
            n->DL = 0;
        }
        if( n->UR->insideBody() )
        {
            freeNode(n->UR);
            n->UR = 0;
        }
        if( n->DR->insideBody() )
        {
            freeNode(n->DR);
            n->DR = 0;
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
        return queryNodeHelper(n->DL,x,y);
    }

    int queryNode(double x, double y, LeafNode*& n)
    {
        n=0;

        if( x<=-10 )
            return NeighborList::FREESTREAM;
        if( x>=10 || y>=10 || y<=-10 )
            return NeighborList::FREE;

        n = queryNodeHelper(&root,x,y);
        
        if( n==NULL )
            return NeighborList::BODY;
        else
            return NeighborList::CALC;
    }
};

extern Grid grid;


#endif  // ___GRID_HH

