#ifndef INCLUDEHOME
#define INCLUDEHOME
#include "shape.cpp"
#include<iostream>
#include<cmath>
typedef Point Color;

Color localLight(
        const Object* obj, const Point& v,
        const Point& p, Point& normal, Color Ia, vector<Light*> lights,
        double eta1){
    Point V = (v - p).normalize();
    const Parameter* pa = &(obj->parameter);
    Color ans = Ia * pa->ka;
    for(vector<Light*>::iterator it = lights.begin();it!=lights.end();++it){
        Point L = ((*it)->loc - p).normalize();
        if(sign(dianji(V, normal)) == sign(dianji(L , normal))){
            Point H = (L + V).normalize();
            ans += (*it)->light * (pa->kds*dianji(L, normal) + pa->ks * pow(max(0., dianji(H, normal)), pa->ns));
        }
        else{
            if(pa->kdt ==0 && pa->kt == 0)
                continue;
            Point H = sign(eta1-obj->eta2)*(obj->eta2 * L + eta1 * V).normalize();
            ans += (*it)->light * (pa->kdt*(dianji(normal, L)) + pa->kt * pow(max(0., -dianji(H, normal)), pa->nt));
        }
    }
    return ans;
}

class Render{
public:
    vector<Object*> objs;
    vector<Light> Lights;
    Color background;
    int Depth;

    void addObj(Object* a){
        objs.push_back(a);
    }

    void addLight(Point loc, Color color){
        Lights.push_back(Light(loc, color));
    }

    int findIntersection(Line ray, Point& interp, Object* & surface, double d = INF){
        Point tmp;
        int find = 0;
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

    vector<Light*> getLight(Point a){
        Point tmp;
        Object* tmp2;
        vector<Light*> lights;
        for(vector<Light>::iterator it = Lights.begin();it != Lights.end();++it){
            if(findIntersection(make_pair(a, it->loc), tmp, tmp2, dist(a, it->loc)))
                continue;
            lights.push_back(&(*it));
        }
        return lights;
    }

    void initialize(){
        for(vector<Object*>::iterator it=objs.begin();it!=objs.end();++it)
            (*it)->intersection_times = 0;
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

    Color rayTrace(Line ray, int depth, double weight, double eta1, int MC){
        if(weight < 1e-10){
            return Color(0, 0, 0);
        }
        Point p;
        Object* obj;

        if( !findIntersection(ray, p, obj) )
            return background;

        ld c = max( max(obj->color.x, obj->color.y), obj->color.z);
        Color f = obj->color;
        if(depth>Depth){
            if(erand() > 0.8){
                return obj->light;
            }
            f /= 0.8;
        }

        Line tmp;
        Point normal = obj->calc_norm(ray, p).normalize();
        Point inp = (p - ray.first).normalize();
        Color color;

        double c1 = dianji(-inp, normal);
        Point reflectRay = 2*c1*normal + inp, transmitRay;

        ld kt = 0;
        if(obj->transmit_value){
            ld eta = (c1>0?obj->eta2/eta1: eta1/obj->eta2);
            kt = transmit(-inp, c1>0?normal:-normal, eta, transmitRay);
        }

        ld wr = obj->reflect_value, wt = obj->transmit_value;
        if(MC && depth>2){
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
            if(obj->diffuse_value == 0)
                color += rayTrace(make_pair(p, p + reflectRay), depth + 1, weight * wr, eta1, MC) * wr;
            else{
                ld r1 = 2*M_PI * erand(), r2 = erand(), r2s = sqrt(r2);
                Point w = c1<0?-normal:normal;
                Point u = chaji( w.x>1?Point(0, 1, 0):Point(1, 0, 0), w );
                Point v = chaji( w, u ); 
                Point diffuseRay = w*cos(r1)*r2s + v*sin(r1)*r2s + w*sqrt(1-r2);
                Color haha = rayTrace(make_pair(p, p + diffuseRay), depth + 1, weight, eta1, MC);
                color +=  haha* obj->diffuse_value; 
            }
        }
        if(wt != 0){
            color += rayTrace(make_pair(p, p + transmitRay), depth + 1, weight * wt, eta1, MC) * wt;
        }

        return color*f + obj->light;
    }

    Render(){
        background = Color(0, 0, 0);
    }
};
#endif
