/*
* drawWindow.cc
* 
* Copyright (c) 2018 Pradu Kannan. All rights reserved.
*/


#include "drawWindow.hh"
#include <stdio.h>
#include <string.h>

DrawWindow::DrawWindow()
{
    //Create Surface
    if( (Xdis=XOpenDisplay(NULL))==NULL )
        exit(1);
    Xscreen = DefaultScreen(Xdis);
    Xda = XCreateSimpleWindow(Xdis,DefaultRootWindow(Xdis), 0, 0, W, H, 0, 0, 0);
    XSelectInput( Xdis,Xda, ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask | ExposureMask );
    XMapWindow(Xdis,Xda);
    sX11 = cairo_xlib_surface_create(Xdis, Xda, DefaultVisual(Xdis,Xscreen), W, H);
    cairo_xlib_surface_set_size(sX11,W,H);
    crX = cairo_create(sX11);

    sImg = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, W, H);
    cr = cairo_create(sImg);
}

void DrawWindow::writeToFile( std::string const& fileName )
{
    cairo_surface_flush(sX11);
    cairo_surface_flush(sImg);
    processEvents(false);
    if( CAIRO_STATUS_SUCCESS == cairo_surface_write_to_png(sImg,fileName.c_str()) )
        printf("Sucessfully Wrote Image\n");
}

void DrawWindow::writeXToFile( std::string const& fileName )
{
    cairo_surface_flush(sX11);
    processEvents(false);
    if( CAIRO_STATUS_SUCCESS == cairo_surface_write_to_png(sX11,fileName.c_str()) )
        printf("Sucessfully Wrote Image\n");
}

bool DrawWindow::processEvents(bool block)
{
    char keybuf[32] = {0};
    KeySym key;
    XEvent e;

    {
        cairo_set_source_surface(crX, sImg, 0, 0);
        cairo_paint(crX);
        cairo_surface_flush(sX11);
    }
    
    for(;;)
    {


        //cairo_set_source_rgb(cr, 0, 0, 0);
        //cairo_paint(cr);
        //cairo_set_line_width(cr, 0.25);
        //cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
        //cairo_rectangle (cr, 0.25, 0.25, 200.5, 200.5);
        //cairo_stroke (cr);

        //cairo_move_to(cr, 20, 20);
        //cairo_line_to(cr, 200, 400);
        //cairo_line_to(cr, 450, 100);
        //cairo_line_to(cr, 20, 20);
        //cairo_set_source_rgb(cr, 0, 0, 1);
        //cairo_fill_preserve(cr);
        //cairo_set_line_width(cr, 5);
        //cairo_set_source_rgb(cr, 1, 1, 0);
        //cairo_stroke(cr);
        //cairo_destroy(cr);

        if (block || XPending(Xdis))
        {
            XNextEvent(Xdis, &e);
            block = false;
        }
        else 
            return true;
        
        switch (e.type)
        {
            case Expose:
                break;
            
            case ButtonPress:
                //printf("ClickP %d\n",e.xbutton.button);
                break;
            case ButtonRelease:
                //printf("ClickR %d\n",e.xbutton.button);
                break;
            case KeyPress:
                //XLookupString(&e.xkey, keybuf, sizeof(keybuf), &key, NULL);
                //printf("KeyP %s %ld\n",keybuf,key);
                break;
            case KeyRelease:
                XLookupString(&e.xkey, keybuf, sizeof(keybuf), &key, NULL);
                //printf("KeyR %s %ld\n",keybuf,key);
                if ( key == 65307 ) // Exit on ESC
                    return false;
            default:
                break; //fprintf(stderr, "Dropped XEevent = %d.\n", e.type);
        }
    }

    return true;
}


void DrawWindow::pixCoord(double& xPix, double& yPix, double xWorld, double yWorld)
{
    xPix =  world_scale*(xWorld-world_xc) + (W/2.0);
    yPix = -world_scale*(yWorld-world_yc) + (H/2.0);
}

void DrawWindow::zoom(double xc, double yc, double width)
{
    world_xc = xc;
    world_yc = yc;
    world_scale = W/width;
}

void DrawWindow::clear()
{
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_paint(cr);
}

void DrawWindow::drawSpline(Spline const& s)
{
    double x0 = s.minX();
    double y0 = s[0].y;
    double step = (s.maxX()-s.minX())/1000.0;
    pixCoord(x0,y0,x0,y0);

    cairo_set_source_rgb(cr,1,1,1);
    cairo_set_line_width(cr,2.0);

    cairo_move_to(cr,x0,y0);

    //for(double i=s.minX()+step; i+0.5*step<s.maxX(); i+=step)
    for( int j=1; j<=1000; j++)
    {
        double i = s.minX() + step*j;
        double xf = i;
        double yf = s.interp(i);
        pixCoord(xf,yf,xf,yf);

        cairo_line_to(cr,xf,yf);

        x0 = xf;
        y0 = yf;
    }
    cairo_stroke(cr);
}
