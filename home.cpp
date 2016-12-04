#ifndef INCLUDEHOME
#define INCLUDEHOME
#include "shape.cpp"
#include<iostream>
typedef Point Color;
const float INF = 1e10;

class LightSource{
    public:
        Point p;
        LightSource(Point a){
            p = a;
        }
};

class Render{
public:
    vector<Object*> objs;
    vector<LightSource> Light;
    Color background;

    void addObj(Object* a){
        objs.push_back(a);
    }

    void addLight(Point p){
        Light.push_back(LightSource(p));
    }

    int findIntersection(Line ray, Point& interp, Object* & surface){
        float d = INF;
        Point tmp;
        for(vector<Object*>::iterator it=objs.begin();it!=objs.end();++it){
            if((*it)->intersection(ray, tmp)){
                float val = dist(ray.first, tmp);
                if(val < d){
                    d = val;
                    interp = tmp;
                    surface = *it;
                }
            }
        }
        return d!=INF;
    }

    Color rayTrace(Line ray, int depth, float weight, float eta){
        if(weight < 1e-10 || !depth){
            return Color(0, 0, 0);
        }
        Point p;
        Object* surface;
        if( findIntersection(ray, p, surface) ){
            Color color = surface->local(ray);
            Line tmp;
            float wt, wr;
            if((wr = surface->reflect(ray, tmp))>eps)
                color += rayTrace(tmp, depth - 1, weight, eta) * wr;
            if((wt = surface->transmit(ray, tmp, eta))>eps){
                color += rayTrace(tmp, depth - 1, weight, (eta==1?surface->eta2:1)) * wt;
            }
            return color;
        }
        else
            return background;
    }

    Render(){
        background = Color(0, 0, 0);
    }
};
#endif
