/*
* grid.hh
* 
* Copyright (c) 2018 Pradu Kannan. All rights reserved.
*/

#ifndef ___GRID_HH
#define ___GRID_HH

#include "spline.hh"
#include "mempool.hh"

struct State
{
    double T;
    double p;
    double rho;
    double vx;
    double vy;
    static void avg(State const& a, State const& b);
};


struct Node
{
    enum { NONE, REFINE, COARSEN };
    enum { NODE_INTERIOR=0, NODE_BODY, NODE_CALC };
    //type
    char type;
    char totalDivision;
    char childDivision;
    char selfDivision;

    //define geometry
    double x;    //center
    double y;    //center
    double size; //box size

    Node* P; //Parent
};

struct InteriorNode : public Node
{
    //Node Structure
    Node* UL; //Up Left
    Node* UR; //Up Right
    Node* DL; //Down Left
    Node* DR; //Down Right

    InteriorNode(double x_in = 0.0, double y_in = 0.0, double size_in=1.0, Node* parent=0)
    {
        type = NODE_INTERIOR;
        P  = parent;
        UL = 0;
        UR = 0;
        DL = 0;
        DR = 0;
        x = x_in;
        y = x_in;
        size = size_in;
    }
};

#define LBOUND 1  // Freestream
#define UBOUND 2  // Free
#define DBOUND 4  // Free
#define RBOUND 8  // Free
#define BBOUND 16 // Body
#define ULMASK ( LBOUND | UBOUND | BBOUND )
#define URMASK ( RBOUND | UBOUND | BBOUND )
#define DLMASK ( LBOUND | DBOUND | BBOUND )
#define DRMASK ( RBOUND | DBOUND | BBOUND )
struct LeafNode : public Node
{
    State x;
    unsigned char bflags; //Boundary Condition Flags
};


struct Grid
{
    InteriorNode root;
    Grid() : root(0,0,)
    {
    }
};

extern MemPool<InteriorNode, 1<<15 > iNodeMem;
extern MemPool<LeafNode, 1<<17 > lNodeMem;


#endif  // ___GRID_HH

