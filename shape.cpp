#ifndef INLCUDESHAPE
#define INLCUDESHAPE
#include<vector>
#include<cmath>
#include<iostream>
#include "assert.h"
#include "basic.cpp"
using namespace std;

typedef Point Color;

class Object{
public:
    double reflect_value;
    double transmit_value, eta2;
    double diffuse_value;
    Point color;
    Point light;
    int isLight;

    virtual int intersection(const Line& ray, Point& p) const = 0;
    virtual Point calc_norm(const Line& ray, const Point& p) const = 0;
    virtual Color local(const Point& point) const = 0;
    virtual ~Object() = 0;

    Object(){
        transmit_value = 0;
        diffuse_value = 0;
        reflect_value = 0;
        eta2 = 1;
        isLight = 0;
    }

    void set_reflect_value(double a){
        reflect_value = a;
    }

    void set_diffuse_value(double a){
        diffuse_value = a;
    }

    void set_color(const Point& a){
        color = a;
    }

    void set_transmit_val(double a){
        transmit_value = a;
    }

    void set_eta2(const double& a){
        eta2 = a;
    }
};

Object::~Object(){}

class Light:public Object{
public:
    virtual Point sample(const Point& p, const Point& n, Point& out) const = 0;
    ld calc(const Point& d, const Point& n, const Point& n2) const{
        ld tmp = d.norm2();
        return dianji(d, n) * (-dianji(n2, d))/tmp/tmp;
    }
};

class Rectangle:public Light{
    public:
    Point a, b, c;
    Point normal;
    ld area, lb, lc;
    Rectangle(Point _a, Point _b, Point _c):a(_a), b(_b), c(_c){
        b-=a;
        c-=a;
        normal = chaji(b, c);
        area = normal.norm();
        normal/=area;
        lb = b.norm();
        lc = c.norm();
        area = lb * lc;
    }
    Point sample(const Point& u, const Point& n, Point& out)const{
        ld x = erand();
        ld y = erand();
        Point v = a + b * x + c*y;
        out = abs(calc(v-u, n, normal)) * light * area;
        return v;
    }

    int intersection(const Line& ray, Point& p)const{
        if(!intersectionPlane(Plane(a, normal), ray, p))
            return 0;
        ld x = dianji(p-a, b);
        ld y = dianji(p-a, c);
        return x<=lb*lb && y<=lc*lc && x>=0 && y>=0;
    }
    Point calc_norm(const Line& ray, const Point& p)const{
        return normal;
    }
    Color local(const Point& point)const{
        return light;
    }
};

class Ball:public Object{
public:
    Point O;
    //a, b record the points of intersection of last ray
    double Radius, a, b;
    Ball(const Point& _O, const double& _Radius, const double& _eta = 1, const Color _color = Point(0, 0, 0))
        :O(_O), Radius(_Radius){
        eta2 = _eta;
        set_transmit_val(0);
        set_reflect_value(0);
        set_color(_color);
    }

    int intersection(const Line& ray, Point& output) const{
        double a, b;
        if(!intersectionBall(ray, O, Radius, a, b))
            return 0;
        if(a>b)
            swap(a, b);
        if(max(a, b)<eps)
            return 0;
        output = ray.first + (ray.second - ray.first) * (a<eps?b:a);
        return 1;
    }

    Point calc_norm(const Line& ray, const Point& interp) const{
        return (interp - O).normalize();
    }

    Color local(const Point& point) const{
        return color;
    }

};
#endif
