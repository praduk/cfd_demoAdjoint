/*
* spline.hh
* 
* Copyright (c) 2018 Pradu Kannan. All rights reserved.
*/

#ifndef ___SPLINE_HH
#define ___SPLINE_HH

#include <vector>
#include <math.h>

struct SplinePoint
{
    double x; //position x
    double y; //position y
    double m; //slope
};

struct Spline : public std::vector<SplinePoint>
{

    template<typename T>
    void makeSpline( T f, double x0, double xf, int N )
    {
        double step = (xf-x0)/(N-1);
        double h = step/5.0;
        double invh = 1.0/h;
        double inv2h = 0.5/h;

        clear();

        SplinePoint p;
        p.x = x0;
        p.y = f(x0);
        p.m = invh*(f(x0+h)-p.y);
        push_back(p);
        for(int i=1; i<N-1; i++)
        {
            p.x = x0 + i*step;
            p.y = f(p.x);
            p.m = inv2h*(f(p.x+h)-f(p.x-h));
            push_back(p);
        }
        p.x = xf;
        p.y = f(xf);
        p.m = invh*(f(p.x)-f(p.x-h));
        push_back(p);
    }

    //The following functions can only be called with a spline with atlest 
    //2 points
    double minX() const { return (*this)[0].x; }
    double maxX() const { return (*this)[this->size()-1].x; }
    double interp(double x) const
    {
        if( x<=minX() )
            return  (*this)[0].y + (*this)[0].m*(x - minX());
        if( x>=maxX() )
            return  (*this)[this->size()-1].y + (*this)[this->size()-1].m*(x - maxX());

        //Perform Binary Search
        int l=0;
        int h=this->size()-1;
        int m = (h+l)/2;

        while( m>l )
        {
            if( x >= (*this)[m].x )
                l = m;
            else
                h = m;
            m = (h+l)/2;
        }

        //Perform interpolation between h and l
        x = x-(*this)[l].x;
        double DX = (*this)[l+1].x - (*this)[l].x;
        double yl = (*this)[l].y;
        double yh = (*this)[l+1].y;
        double ml = (*this)[l].m;
        double mh = (*this)[l+1].m;

        double xDX = x/DX;
        double xDX2 = xDX*xDX;
        double xDX3 = xDX*xDX2;

        double f = yl + ml*x + xDX3 *( DX*(ml + mh) + 2.0*(yl-yh) ) -
                   xDX2*(3.0*(yl-yh) + DX*(mh+2.0*ml));

        return f;
    }
};

struct Airfoil
{
    Spline top;
    Spline bot;

    static double circleTop(double x)
    {
        if( fabs(x)>=0.5 )
            return 0.0;
        double Rsquared = 0.5*0.5;
        return 0.5*sqrt(Rsquared-x*x);
    }
    
    static double circleBot(double x)
    {
        if( fabs(x)>=0.5 )
            return 0.0;
        double Rsquared = 0.5*0.5;
        return -0.5*sqrt(Rsquared-x*x);
    }

    Airfoil()
    {
        top.makeSpline(circleTop,-0.5,0.5,100);
        bot.makeSpline(circleBot,-0.5,0.5,100);
    }

    bool inside(double x, double y)
    {
        return !( x<-0.5 || x>0.5 || y>top.interp(x) || y<bot.interp(x) );
    }
};

extern Airfoil body;

#endif  // ___SPLINE_HH

