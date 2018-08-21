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

    grid.adapt(&grid.root);
    printf("Usage = %f\n",lNodeMem.usage());

    struct timespec ts = {0, 16666666};
    double t = 0.0;
    while( dw.processEvents() )
    {
        t = t+(6.28/60.0)/16.0;
        dw.clear();
        double tau = (0.5+0.5*cos(t));
        tau*=tau*tau;
        dw.zoom(-0.25*(1.0-tau),0.21650635094*(1.0-tau), 0.01+1*(tau));
        //dw.drawSpline(body.top);
        dw.drawSpline(body.top);
        dw.drawSpline(body.bot);
        dw.drawGrid();


        
        //cairo_t* cr = cairo_create(dw.sImg);
        ////cairo_set_source_rgb(cr, 0, 0, 0);
        ////cairo_paint(cr);
        //cairo_set_line_width(cr, 0.25);
        //cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
        //cairo_rectangle (cr, 0.25, 0.25, 200.5, 200.5);
        //cairo_stroke (cr);

        //cairo_move_to(cr, 20, 20);
        //cairo_line_to(cr, 200, 400);
        //cairo_line_to(cr, 450+100*sin(t), 100+50*cos(t));
        //cairo_line_to(cr, 20, 20);
        //cairo_set_source_rgb(cr, 0, 0, 1);
        //cairo_fill_preserve(cr);
        //cairo_set_line_width(cr, 5);
        //cairo_set_source_rgb(cr, 1, 1, 0);
        //cairo_stroke(cr);
        //cairo_destroy(cr);

        cairo_surface_flush(dw.sX11);
        nanosleep(&ts,NULL);
    }

    dw.writeToFile("test.png");
    return 0;
}


