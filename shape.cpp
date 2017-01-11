#ifndef INLCUDESHAPE
#define INLCUDESHAPE
#include<vector>
#include<cmath>
#include<iostream>
#include "assert.h"
#include "basic.cpp"
using namespace std;

typedef Point Color;

class Parameter{
public:
    double ka, kds, ks, ns, kdt, kt, nt;
    Parameter(double _ka = 0, double _kds = 0, double _ks = 0, double _ns = 0, double _kdt = 0, double _kt = 0, double _nt = 0):ka(_ka), kds(_kds), ks(_ks), ns(_ns), kdt(_kdt), kt(_kt), nt(_nt){}
};

class Object{
public:
    double reflect_value;
    double transmit_value, eta2;
    double diffuse_value;
    Point color;
    Point light;
    Parameter parameter;
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

    void set_parameter(const Parameter& a){
        parameter = a;
    }

    void set_eta2(const double& a){
        eta2 = a;
    }
};

Object::~Object(){}

class Light:public Object{
public:
    virtual Point sample(const Point& p, const Point& n, Point& out) const = 0;
    Color calc(const Point& d, const Point& n, const Point& n2) const{
        ld tmp = d.norm2();
        return dianji(d, n) * (-dianji(n2, d))/tmp/tmp;
    }
};

class Rectangle:Light{
    Point a, b, c;
    Point normal;
    ld area;
    Rectangle(Point _a, Point _b, Point _c):a(_a), b(_b), c(_c){
        b-=a;
        c-=a;
        normal = chaji(b, c);
        area = normal.norm();
        normal/=area;
    }
    Point sample(const Point& p, const Point& n, Point& out)const{
        ld x = rand();
        ld y = rand();
        Point v = a + b * x + c*y;
        out = calc(v-p, n, normal) * light;
        return v;
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
