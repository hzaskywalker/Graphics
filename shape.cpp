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
    float reflect_value;
    float transmit_value, eta2;
    Point norm, interp;
    Point color;

    virtual int intersection(const Line& ray, Point& p) = 0;
    virtual void calc_norm(const Line& ray, const Point& p) = 0;
    virtual Color local(const Line& ray) = 0;
    int calc_transmit(const Point& a, const Point& n, float eta, Point& p){
        float c1 = dianji(a, n)/a.norm()/n.norm();
        if(1-c1*c1>eta*eta)
            return 0;
        float c2 = sqrt(1- 1/(eta*eta) * (1-c1*c1));
        p = -1/eta * a - (c2 - 1/eta * c1) * n;
        return 1;
    }

    Point calc_reflect(const Point& a, const Point& n){
        float c = dianji(a, n);
        return 2*dianji(a, n) * n - a;
    }

    float reflect(const Line& ray, Line& reflectRay){
        if(reflect_value!=0){
            intersection(ray, interp);
            calc_norm(ray, interp);
            reflectRay = make_pair( interp, calc_reflect(ray.first - interp, norm) );
        }
        return reflect_value;
    }
    float transmit(const Line& ray, Line& transmittedRay, float eta1){
        if(eta2 != 0){
            intersection(ray, interp);
            calc_norm(ray, interp);
            Point aim;
            if( calc_transmit(ray.first - interp, norm, eta2/eta1, aim) ){
                transmittedRay = make_pair(interp,  aim + interp);
                return transmit_value;
            }
        }
        return 0;
    }

    void set_reflect_value(float a){
        reflect_value = a;
    }

    void set_color(const Point& a){
        color = a;
    }


    void set_transmit_val(float a){
        transmit_value = a;
    }
};

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

class Ball:public Object{
public:
    Point O;
    //a, b record the points of intersection of last ray
    float Radius, a, b;
    Ball(const Point& _O, const float& _Radius, const float& _eta = 0)
        :O(_O), Radius(_Radius){
        eta2 = _eta;
    }

    int intersection(const Line& ray, Point& output){
        float a, b;
        if(!intersectionBall(ray, O, Radius, a, b))
            return 0;
        if(a>b)
            swap(a, b);
        if(max(a, b)<eps)
            return 0;
        interp = ray.first + (ray.second - ray.first) * (a<eps?b:a);
        output = interp;
        return 1;
    }

    void calc_norm(const Line& ray, const Point& interp){
        norm = interp - O;
        norm/= norm.norm();
    }

    Color local(const Line& ray){
        return color;
    }
};
#endif
