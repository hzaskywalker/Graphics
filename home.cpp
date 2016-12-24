#ifndef INCLUDEHOME
#define INCLUDEHOME
#include "shape.cpp"
#include<iostream>
typedef Point Color;
const double INF = 1e10;

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

    Color rayTrace(Line ray, int depth, double weight, double eta){
        if(weight < 1e-10 || !depth){
            return Color(0, 0, 0);
        }
        Point p;
        Object* obj;
        if( findIntersection(ray, p, obj) ){
            Line tmp;
            double wt, wr;
            Point normal = obj->calc_norm(ray, p);
            Color color = localLight(obj, ray.first, p, normal, obj->local(p), getLight(p), eta);
            int beOut = dianji(normal, (ray.first - p).normalize())>-eps;
            if((wr = obj->reflect(ray, tmp, p, normal))>eps && beOut){
                Color haha = rayTrace(tmp, depth - 1, weight * wr, eta) * wr;
                color += haha;
            }
            if((wt = obj->transmit(ray, tmp, eta, p, normal))>eps){
                Color haha = rayTrace(tmp, depth - 1, weight * wt, eta) * wt;
                color += haha;
            }
            color.clamp(255.);
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
