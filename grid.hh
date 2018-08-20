/*
* grid.hh
* 
* Copyright (c) 2018 Pradu Kannan. All rights reserved.
*/

#ifndef ___GRID_HH
#define ___GRID_HH

#include "spline.hh"

struct Node
{
    enum { NODE_INTERIOR=0, NODE_BODY, NODE_CALC };
    //type
    int type;

    //define geometry
    double x;    //center
    double y;    //center
    double size; //box size
};

struct InteriorNode : public Node
{
    //Node Structure
    Node* P; //Parent
    Node* UL; //Up Left
    Node* UR; //Up Right
    Node* DL; //Down Left
    Node* DR; //Down Right

    InteriorNode(double x_in = 0.0, double y_in = 0.0, double size_in=1.0)
    {
        type = NODE_INTERIOR;
        P  = 0;
        UL = 0;
        UR = 0;
        DL = 0;
        DR = 0;
        x = x_in;
        y = x_in;
        size = size_in;
    }
};

struct Grid
{

};

#endif  // ___GRID_HH

