/*
* drawWindow.hh
* 
* Copyright (c) 2018 Pradu Kannan. All rights reserved.
*/

#ifndef ___DRAWWINDOW_HH
#define ___DRAWWINDOW_HH

#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <X11/Xlocale.h>
#include <X11/Xos.h>
#include <string>

#include "spline.hh"

//Window Width and Height
#define W 900
#define H 900

struct DrawWindow
{
    Display* Xdis;
    Drawable Xda;
    int Xscreen;

    //world coordinate parametsrs
    double world_xc; //center point x
    double world_yc; //center point y
    double world_scale;  //x width

    cairo_surface_t* sX11; //X11   surface
    cairo_surface_t* sImg; //Image surface
    cairo_t* cr;           //cairo context
    cairo_t* crX;          //cairo context


    DrawWindow();

    void clear();

    void zoom(double xc, double yc, double width);
    void pixCoord(double& xPix, double& yPix, double xWorld, double yWorld);
    bool processEvents(bool block=true);
    void writeToFile(std::string const& fileName);
    void writeXToFile(std::string const& fileName);
    void drawSpline(Spline const& s);
};

#endif  // ___DRAWWINDOW_HH

