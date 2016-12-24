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
    Point color;
    Point light;
    Parameter parameter;

    virtual int intersection(const Line& ray, Point& p) const = 0;
    virtual Point calc_norm(const Line& ray, const Point& p) const = 0;
    virtual Color local(const Point& point) const = 0;
    virtual ~Object() = 0;

    int _transmit(Point a, Point n, double eta, Point& p){
        a = a.normalize();
        n = n.normalize();
        double c1 = dianji(a, n);
        if(1-c1*c1>=eta*eta)
            return 0;
        double c2 = sqrt(1- 1/(eta*eta) * (1-c1*c1));
        p = -1/eta * a - (c2 - 1/eta * c1) * n;
        return 1;
    }

    Point _reflect(Point a, Point n){
        a = a.normalize();
        n = n.normalize();
        double c = dianji(a, n);
        return 2*c * n - a;
    }

    double reflect(const Line& ray, Line& reflectRay, const Point& interp, const Point& norm){
        if(reflect_value!=0){
            reflectRay = make_pair( interp, _reflect(ray.first - interp, norm) + interp );
        }
        return reflect_value;
    }
    double transmit(const Line& ray, Line& transmittedRay, double eta1, const Point& interp, const Point& norm){
        if(eta2 != 0){
            int beOut = dianji(norm, ray.first - interp)>0;
            double eta = beOut?eta1/eta2:eta2/eta1;
            Point aim;
            if( _transmit(ray.first - interp, norm, eta2/eta1, aim) ){
                transmittedRay = make_pair(interp,  aim + interp);
                return transmit_value;
            }
        }
        return 0;
    }

    void set_reflect_value(double a){
        reflect_value = a;
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
};

Object::~Object(){}

class Surface:Object{
    vector<Point> pts;
    Surface(vector<Point> a){
        pts = a;
    }
    Surface(Point a[], int num){
        assert (num>=3);
        for(int i =0 ;i<num;++i){
            pts.push_back(a[i]);
        }
    }
};

class Meshes:Object{
    vector<Point> pts;
    vector< vector<int> > meshes;

    void init(string filename){
    }
};

class Light{
public:
    Point loc;
    Color light;
    Light(const Point& _loc, const Color& _light):loc(_loc), light(_light){}
};

class Ball:public Object{
public:
    Point O;
    //a, b record the points of intersection of last ray
    double Radius, a, b;
    Ball(const Point& _O, const double& _Radius, const double& _eta = 0, const Color _color = Point(0, 0, 0))
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
