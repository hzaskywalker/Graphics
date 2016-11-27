#ifndef INLCUDESHAPE
#define INLCUDESHAPE
#include<vector>
#include<cmath>
#include "assert.h"
#include "basic.cpp"
using namespace std;

typedef Point Color;

class Object{
public:
    float reflect_value;
    float transmit_value, eta2;
    Point norm, interp;

    virtual int intersection(const Line& ray, const Point& p);
    virtual void calc_norm(const Line& ray, const Point& p);
    virtual Color local(const Line& ray);

    Point calc_transmit(const Point& a, const Point& n, float eta){
        float c1 = dianji(a, n)/a.norm()/n.norm();
        float c2 = sqrt(1- 1/(eta*eta) * (1-c1*c1));
        return -1/eta * a - (c2 - 1/eta * c1) * n;
    }

    Point calc_reflect(const Point& a, const Point& n){
        float c = dianji(a, n);
        return 2*dianji(a, n) * n - a;
    }

    float reflect(const Line& ray, Line& relectRay){
        if(reflect_value!=0){
            intersection(ray, interp);
            calc_norm(ray, interp);
            relectRay = make_pair( interp, calc_reflect(ray.first - interp, norm) );
        }
        return reflect_value;
    }
    float transmit(const Line& ray, Line& transmittedRay, float eta1){
        if(eta2 != 0){
            intersection(ray, interp);
            calc_norm(ray, interp);
            transmittedRay = make_pair(interp, calc_transmit(ray.first - interp, norm, eta2/eta1) );
        }
        return 0;
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
    Ball(const Point& _O, const float& _Radius, const float& _eta = 0):O(_O), Radius(_Radius){
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
        interp = ray.first + (ray.second - ray.first) * (a<0?b:a);
        output = interp;
        return 1;
    }

    void calc_norm(const Line& ray, const Point& interp){
        norm = interp - O;
        norm/= norm.norm();
    }

    virtual Color local(const Line& ray);
};
#endif
