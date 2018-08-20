/*
* main.cc
* 
* Copyright (c) 2018 Pradu Kannan. All rights reserved.
*/

#include "drawWindow.hh"

#include <time.h>
#include <math.h>

DrawWindow dw;

int main()
{
    dw.zoom(0.0,0.0,2.0);

    struct timespec ts = {0, 16666666};
    double t = 0.0;
    while( dw.processEvents() )
    {
        t = t+(6.28/60.0);
        dw.clear();
        dw.zoom(-0.5*(1.0-cos(t))/2.0,0.0, 0.5+3.0*(1.0+cos(t))/2.0);
        dw.drawSpline(body.top);
        dw.drawSpline(body.bot);

        
        cairo_t* cr = cairo_create(dw.sImg);
        //cairo_set_source_rgb(cr, 0, 0, 0);
        //cairo_paint(cr);
        cairo_set_line_width(cr, 0.25);
        cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
        cairo_rectangle (cr, 0.25, 0.25, 200.5, 200.5);
        cairo_stroke (cr);

        cairo_move_to(cr, 20, 20);
        cairo_line_to(cr, 200, 400);
        cairo_line_to(cr, 450+100*sin(t), 100+50*cos(t));
        cairo_line_to(cr, 20, 20);
        cairo_set_source_rgb(cr, 0, 0, 1);
        cairo_fill_preserve(cr);
        cairo_set_line_width(cr, 5);
        cairo_set_source_rgb(cr, 1, 1, 0);
        cairo_stroke(cr);
        cairo_destroy(cr);

        cairo_surface_flush(dw.sX11);
        nanosleep(&ts,NULL);
    }

    dw.writeToFile("test.png");
    return 0;
}


