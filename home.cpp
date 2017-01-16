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
        if(1-c1*c1>=eta*eta){
            return 0;
        }
        double c2 = sqrt(1- (1-c1*c1)/(eta*eta));
        p = -1/eta * a - (c2 - 1/eta * c1) * n;
        float Rs = (c1*eta - c2) / (c1*eta + c2); 
        float Rp = (c1 - c2*eta) / (c1 + c2*eta); 
        return 1 - (Rs * Rs + Rp * Rp) / 2;
    }

    Color rayTrace(Line ray, int depth, double weight, double eta1, int out = 1){
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
//            cout<<"  light"<<obj->light<<" "<<p<<endl;
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
        /*
        if(out!=(c1>=0)){
            cout<<"Wrong"<<endl;
            Triangle* tt = (Triangle*)obj;
            cout<<tt->a<<" "<<tt->b<<" "<<tt->c<<" "<<tt->normal.normalize()<<endl;
            for(int i = 0;i<kdtree.tris.size();++i){
                ld r1=0, r2=0;
                if(kdtree.tris[i]->intersection(ray.first, inp, r1)){
                    cout<<"###"<<r1<<endl;
                }
            }
            cout<<depth<<" "<<out<<" "<<ray.first<<" "<<p<<" "<<inp<<endl;
            exit(0);
            return rayTrace(make_pair(p, p+inp), depth +1, weight, eta1, out);
        }
        */
        Point reflectRay = 2*c1*normal + inp, transmitRay;

        ld kt = 0;
        if(obj->transmit_value){
            ld eta = (c1>0?obj->eta2/eta1: eta1/obj->eta2);
            kt = transmit(-inp, c1>0?normal:-normal, eta, transmitRay);
        }

        ld wr = obj->reflect_value, wt = obj->transmit_value;
        if(depth>2){
            ld P = 0.25 + 0.5 * (1-kt);
            if(erand() < P ){
                wr += obj->transmit_value * (1-kt) /P;
                wt = 0;
            }
            else{
                wt = wt * kt/(1-P);
            }
        }
        else{
            wr += obj->transmit_value * (1-kt);
            wt *= kt;
        }
        Point re, tr;
        if( wr > eps ){
            re = rayTrace(make_pair(p, p + reflectRay), depth + 1, weight * wr, eta1, out) * wr;
            color += re;
//            ll += localLight(reflectRay, p, 1) * wr;
        }
        if(obj->diffuse_value>eps){
            out = 1;
            ld r1 = 2*M_PI * erand(), r2 = erand(), r2s = sqrt(r2);
            Point w = c1<0?-normal:normal;
            Point u = chaji( fabs(w.x)>1.?Point(0, 1, 0):Point(1, 0, 0), w ).normalize();
            Point v = chaji( w, u ); 
            Ball* t = (Ball*)obj;
            Point diffuseRay = u*cos(r1)*r2s + v*sin(r1)*r2s + w*sqrt(1-r2);
            Color haha = rayTrace(make_pair(p, p + diffuseRay.normalize()), depth + 1, weight, eta1, out);
            color +=  haha * obj->diffuse_value; 
//            if(depth == 0)
//                cout<<color<<endl;
//            ll += localLight(normal, p, 0) * obj->diffuse_value;
        }
        if(wt != 0){
            tr = rayTrace(make_pair(p + transmitRay * eps, p + transmitRay), depth + 1, weight * wt, eta1, out^1) * wt;
            color += tr;
        }
        /*
        if(color.norm()>0||depth <=2){
            cout<<"++"<<depth<<" "<<color<<" "<<p<<" "<<tr<<" "<<re<<" "<<wr<<" "<<kt<<" "<<wt<<" "<<obj->eta2<<endl;
        }
        */
        return (color + ll)*f + obj->light;
    }

    Render(){
        background = Color(0, 0, 0);
    }
};
#endif
