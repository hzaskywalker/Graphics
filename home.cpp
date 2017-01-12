#ifndef INCLUDEHOME
#define INCLUDEHOME
#include "shape.cpp"
#include<iostream>
#include<cmath>
typedef Point Color;

class Render{
public:
    vector<Object*> objs;
    vector<Light*> Lights;
    Color background;
    KDtree kdtree;
    int Depth;

    void addObj(Object* a){
        objs.push_back(a);
    }

    void addTriangle(Triangle* a){
        kdtree.addTriangle(a);
    }

    void addLight(Light* a){
        if(a->light.norm() > 0){
            a->isLight = 1;
            Lights.push_back(a);
        }
    }

    int findIntersection(Line ray, Point& interp, Object* & surface, double d = INF){
        Point tmp;
        int find = 0;
        find = kdtree.intersection(ray, interp, surface, d);
        for(vector<Object*>::iterator it=objs.begin();it!=objs.end();++it){
            if((*it)->intersection(ray, tmp)){
                double val = dist(ray.first, tmp);
                if(val + eps < d){
                    interp = tmp;
                    surface = *it;
                    find = 1;
                    d = val;
                }
            }
        }
        return find;
    }

    Color localLight(const Point& normal, const Point u, int glass){
        return Color(0, 0, 0);
        Object* tmp;
        Color ans;
        for(vector<Light*>::iterator it=Lights.begin();it!=Lights.end();++it){
            Point light;
            Point v;
            if(glass == 0)
                v = (*it)->sample(u, normal, light);
            else{
                if(!(*it)->intersection(make_pair(u, u + normal), v))
                    continue;
                light = (*it)->getDirectLight(u, v);
            }
            if(findIntersection(make_pair(u, v), v, tmp, dist(u, v) - eps))
                continue;
            ans+=light;
        }
        return ans;
    }

    ld transmit(const Point& a, const Point& n, const double& eta, Point& p){
        double c1 = dianji(a, n);
        if(1-c1*c1>=eta*eta)
            return 0;
        double c2 = sqrt(1- (1-c1*c1)/(eta*eta));
        p = -1/eta * a - (c2 - 1/eta * c1) * n;

        float Rs = (c1*eta - c2) / (c1*eta + c2); 
        float Rp = (c1 - c2*eta) / (c1 + c2*eta); 
        return 1 - (Rs * Rs + Rp * Rp) / 2;
    }

    Color rayTrace(Line ray, int depth, double weight, double eta1){
        if(weight < 1e-10){
            return Color(0, 0, 0);
        }
        Point p;
        Object* obj;

        //ray.first += (ray.second - ray.first)*eps;
        if( !findIntersection(ray, p, obj) ){
            return background;
        }

        ld c = max( max(obj->color.x, obj->color.y), obj->color.z);
        Color f = obj->color;

        ld t = erand();
        if(obj->isLight){
            return obj->light * abs( dianji(ray.first - ray.second, ((Rectangle*)obj)->normal) );
        }
        if(depth>Depth){
            if(t > 0.8){
                return obj->light;
            }
            f /= 0.8;
        }

        Line tmp;
        Point normal = obj->calc_norm(ray, p).normalize();
        Point inp = (p - ray.first).normalize();
        Color color;
        Color ll;

        double c1 = dianji(-inp, normal);
        Point reflectRay = 2*c1*normal + inp, transmitRay;

        ld kt = 0;
        if(obj->transmit_value){
            ld eta = (c1>0?obj->eta2/eta1: eta1/obj->eta2);
            kt = transmit(-inp, c1>0?normal:-normal, eta, transmitRay);
        }

        ld wr = obj->reflect_value, wt = obj->transmit_value;
        if(depth>2){
            if(erand() > kt){
                wr += obj->transmit_value;
                wt = 0;
            }
        }
        else{
            wr += obj->transmit_value * (1-kt);
            wt *= kt;
        }

        if( wr > eps ){
            color += rayTrace(make_pair(p, p + reflectRay), depth + 1, weight * wr, eta1) * wr;
            ll += localLight(reflectRay, p, 1) * wr;
        }
        if(obj->diffuse_value>eps){
            ld r1 = 2*M_PI * erand(), r2 = erand(), r2s = sqrt(r2);
            Point w = c1<0?-normal:normal;
            Point u = chaji( w.x>1?Point(0, 1, 0):Point(1, 0, 0), w ).normalize();
            Point v = chaji( w, u ); 
            Ball* t = (Ball*)obj;
            Point diffuseRay = u*cos(r1)*r2s + v*sin(r1)*r2s + w*sqrt(1-r2);
            Color haha = rayTrace(make_pair(p, p + diffuseRay.normalize()), depth + 1, weight, eta1);
            color +=  haha * obj->diffuse_value; 
            ll += localLight(normal, p, 0) * obj->diffuse_value;
        }
        if(wt != 0){
            color += rayTrace(make_pair(p, p + transmitRay), depth + 1, weight * wt, eta1) * wt;
            if(depth == 1)
            ll += localLight(transmitRay, p, 1) * wt;
        }

        return (color + ll)*f + obj->light;
    }

    Render(){
        background = Color(0, 0, 0);
    }
};
#endif
