#ifndef INCLUDEHOME
#define INCLUDEHOME
#include "shape.cpp"
#include<iostream>
typedef Point Color;
const float INF = 1e10;

class LightSource{
    public:
        Point p;
        Color color;
        LightSource(Point a, Color b):p(a), color(b){}
};

class Render{
public:
    vector<Object*> objs;
    vector<LightSource> Light;
    Color background;

    void addObj(Object* a){
        objs.push_back(a);
    }

    void addLight(Point p, Color c){
        Light.push_back(LightSource(p, c));
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

    Color getLight(Point a){
        Color now = Point(0, 0, 0);
        for(vector<LightSource>::iterator it = Light.begin(); it!=Light.end(); ++ it){
            Point interp;
            Object* tmp;
            if(findIntersection(make_pair(a, it->p), interp, tmp))
                continue;
            now += it->color;
        }
        return now;
    }

    Color rayTrace(Line ray, int depth, float weight, float eta){
        if(weight < 1e-10 || !depth){
            return Color(0, 0, 0);
        }
        Point p;
        Object* surface;
        if( findIntersection(ray, p, surface) ){
            Color color = surface->local(ray) * getLight(p)/127.5;
            Line tmp;
            float wt, wr;
            if((wr = surface->reflect(ray, tmp))>eps){
                Color haha = rayTrace(tmp, depth - 1, weight * wr, eta) * wr;
                color += haha;
            }
            if((wt = surface->transmit(ray, tmp, eta))>eps){
                color += rayTrace(tmp, depth - 1, weight * wt, (eta==1?surface->eta2:1)) * wt;
            }
            color.x = min(color.x, (ld)255.);
            color.y = min(color.y, (ld)255);
            color.z = min(color.z, (ld)255);
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
