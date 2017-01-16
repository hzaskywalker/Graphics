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
    double reflect_value;
    double transmit_value, eta2;
    double diffuse_value;
    Point color;
    Point light;
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

    void set_eta2(const double& a){
        eta2 = a;
    }
};

Object::~Object(){}

class Light:public Object{
public:
    virtual Point sample(const Point& p, const Point& n, Point& out) const = 0;
    virtual Point getDirectLight(const Point& u, const Point& v) const = 0;
    ld calc(const Point& d, const Point& n, const Point& n2) const{
        ld tmp = d.norm2();
        return dianji(d, n) * (-dianji(n2, d))/tmp/tmp;
    }
};

class Rectangle:public Light{
    public:
    Point a, b, c;
    Point normal;
    ld area, lb, lc;
    Rectangle(Point _a, Point _b, Point _c):a(_a), b(_b), c(_c){
        b-=a;
        c-=a;
        normal = chaji(b, c);
        area = normal.norm();
        normal/=area;
        lb = b.norm();
        lc = c.norm();
        area = lb * lc;
    }

    Point getDirectLight(const Point& u, const Point& v) const{
        return light * abs(dianji((u-v).normalize(), normal)) * area;
    }

    Point sample(const Point& u, const Point& n, Point& out)const{
        ld x = erand();
        ld y = erand();
        Point v = a + b * x + c*y;
        out = abs(calc(v-u, n, normal)) * light * area;
        return v;
    }

    int intersection(const Line& ray, Point& p)const{
        if(!intersectionPlane(Plane(a, normal), ray, p))
            return 0;
        ld x = dianji(p-a, b);
        ld y = dianji(p-a, c);
        return x<=lb*lb && y<=lc*lc && x>=0 && y>=0;
    }
    Point calc_norm(const Line& ray, const Point& p)const{
        return normal;
    }
    Color local(const Point& point)const{
        return light;
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

class Triangle: public Object{
    public:
        Point a, b, c, u, v, n;
        Point normal, color, mid;
        Point na, nb, nc;

        ld uu, vv, uv, D;

        int intersection(const Line& ray, Point& p) const{
            Point s0 = ray.first;
            Point dir = ray.second - ray.first;
            ld r;
            if(!((Triangle*)this)->intersection(s0, dir, r))
                return 0;
            p = s0 + dir * r;
            return 1;
        }
        Triangle( const Point& _a, const Point& _b, const Point& _c, const Point& _na, const Point& _nb, const Point& _nc):a(_a), b(_b), c(_c), na(_na), nb(_nb), nc(_nc){
            mid = (a+b+c)/3;
            u = b-a;
            v = c-a;
            normal = chaji(u, v).normalize();
            uu= dianji(u, u);
            vv= dianji(v, v);
            uv= dianji(u, v);
            D = uv * uv - uu * vv;
        }

        Point calc_norm(const Line& ray, const Point& p) const{
            Point x = a - p;
            Point y = b - p;
            Point z = c - p;

            ld aa = chaji(y, z).norm();
            ld bb = chaji(z, x).norm();
            ld cc = chaji(x, y).norm();
            return (na * aa + nb * bb + nc * cc)/(aa + bb + cc);
        }

        Color local(const Point& point) const{
            return color;
        }

        Point getMid() const{
            return mid;
        }

        int intersection(const Point& s0, const Point& dir, ld& r){
            //http://geomalgorithms.com/a06-_intersect-2.html#intersect3D_RayTriangle()
            Point w0 = s0 - a;
            ld aa = -dianji(normal, w0);
            ld bb = dianji(normal, dir);
            if(abs(bb)<eps){
                if(abs(aa)>eps)
                    return 0;
//                cout<<r<<" "<<a<<" "<<b<<" "<<c<<" "<<normal<<" "<<dir<<endl;
                return 0;
            }
            r = aa/bb;
            if(r<eps){
//                if(r>-eps) cout<<dianji( normal, dir)<<endl;
                return 0;
            }
            Point p = s0 + r * dir;
            Point w = p-a;
            /*
            if(dianji(p-a, normal)>1e-5){
                cout<<endl<<dianji(p-a, normal)<<" "<<r<<" "<<aa<<" "<<bb<<endl;
                exit(0);
            }
            assert(dianji(p-a, normal)<1e-5);
            */
            ld wu = dianji(w, u);
            ld wv = dianji(w, v);
            ld s = (uv * wv - vv * wu)/D;
            ld t = (uv * wu - uu * wv) / D;
//            cout<<s*u + t*v + a<<" "<<p<<endl;
//            if(s>-eps * 10 && s<1.0 + eps * 10&& t>-eps * 10&& t+s < 1.0 + eps * 10)
//                cout<<r<<" "<<a<<" "<<b<<" "<<c<<endl;
                /*
            if((s>-1 &&  s<1  && t>-1 && t+s<2.) || abs(D)<1){
                cout<<D<<endl;
                cout<<s<<" "<<t<<" "<<a<<" "<<b<<" "<<c<<endl;
            }
            */
            
            if (s < 0.0 || s > 1.0)
                return 0;
            if (t < 0.0 || (s + t) > 1.0)
                return 0;
            return 1;    
        }
};

struct Bbox{
    ld minx, miny, minz;
    ld maxx, maxy, maxz;

    ld min(const ld& a, const ld& b){
        return a<b?a:b;
    }

    ld max(const ld& a, const ld& b){
        return a>b?a:b;
    }

    ld min(const ld& a, const ld& b, const ld& c){
        return min( min(a, b), c );
    }

    ld max(const ld& a, const ld& b, const ld& c){
        return max( max(a, b), c );
    }

    Bbox(){
        minx = INF;
        miny = INF;
        minz = INF;

        maxx = -INF;
        maxy = -INF;
        maxz = -INF;
    }
    Bbox(const Triangle* a){
        minx = min(a->a.x, a->b.x, a->c.x)-eps;
        miny = min(a->a.y, a->b.y, a->c.y)-eps;
        minz = min(a->a.z, a->b.z, a->c.z)-eps;

        maxx = max(a->a.x, a->b.x, a->c.x)+eps;
        maxy = max(a->a.y, a->b.y, a->c.y)+eps;
        maxz = max(a->a.z, a->b.z, a->c.z)+eps;
    }

    Bbox& operator += (const Bbox& b){
        minx = min(minx, b.minx);
        miny = min(miny, b.miny);
        minz = min(minz, b.minz);
        maxx = max(maxx, b.maxx);
        maxy = max(maxy, b.maxy);
        maxz = max(maxz, b.maxz);
        return *this;
    }

    int intersection(const Point& a, const Point& b, ld& t1, ld& t2){
//        cout<<"miny "<<miny<<" maxy "<<maxy<<" "<<a.y<<endl;
        ld mintx = (minx-a.x)/b.x;
        ld maxtx = (maxx-a.x)/b.x;
        if(mintx>maxtx)swap(mintx, maxtx);

        ld minty = (miny-a.y)/b.y;
        ld maxty = (maxy-a.y)/b.y;
        if(minty>maxty)swap(minty, maxty);

        ld mintz = (minz-a.z)/b.z;
        ld maxtz = (maxz-a.z)/b.z;
        if(mintz>maxtz)swap(mintz, maxtz);
//        cout<<"miny "<<miny<<" maxy "<<maxy<<" "<<a.y<<endl;
//        cout<<mintx<<" "<<minty<<" "<<mintz<<endl;
//        cout<<maxtx<<" "<<maxty<<" "<<maxtz<<endl;
        t1 = max(mintx, minty, mintz);
        t2 = min(maxtx, maxty, maxtz);
        return t1 < t2 && t2 > 0;
    }
};

ostream& operator << (ostream& a, const Bbox& b){
    a<<"("<<b.minx<<" "<<b.maxx<<" "<<b.miny<<" "<<b.maxy<<" "<<b.minz<<" "<<b.maxz<<")";
    return a;
}

struct KDNode{
    KDNode *lc, *rc;
    Bbox bbox;
    Triangle* tri;
    KDNode(){
        lc = rc = 0;
        tri = 0;
    }
};

class KDtree{
    public:
    vector<Triangle*> tris;
    KDNode* root;

    void buildKDtree(){
        cout<<"initialize kd tree"<<endl;
        root = split(tris, 0);
        cout<<endl;
    }

    KDNode* split(vector<Triangle*>& tris, int depth){
        if(tris.size() == 0)
            return 0;
        KDNode* u = new KDNode();
        u->bbox = Bbox( tris[0] );
        if(tris.size() == 1){
            u->tri = tris[0];
            return u;
        }
        Point mid;
        for(int i = 0;i<tris.size();++i){
            mid += tris[i]->getMid();
        }
        mid/=tris.size();
        vector<Triangle*> left, right;
        for(int i =0;i<tris.size();++i){
            int flag;
            if(depth == 0)
                flag = tris[i]->getMid().x<mid.x;
            else if(depth == 1)
                flag = tris[i]->getMid().y<mid.y;
            else
                flag = tris[i]->getMid().z<mid.z;
            if(flag)
                left.push_back(tris[i]);
            else right.push_back(tris[i]);
        }
        u->lc = split(left, (depth+1)%3);
        u->rc = split(right, (depth+1)%3);
        if(u->lc)u->bbox += u->lc->bbox;
        if(u->rc)u->bbox += u->rc->bbox;
        return u;
    }

    int brutehit(KDNode* u, const Point& a, const Point& b, ld& p, Triangle* &obj){
        if(!u)
            return 0;
        if(u->tri!=0){
            ld r;
            if(!u->tri->intersection(a, b, r))
                return 0;
            if(r<p){
                obj = u->tri;
                p = r;
            }
            return 1;
        }
        return brutehit(u->lc, a, b, p, obj) | brutehit(u->rc, a, b, p, obj);
    }

    int hit(KDNode* u, const Point& a, const Point& b, ld& p, Triangle* &obj){
        ld t1=-1, t2=-1;
        if(!u->bbox.intersection(a, b, t1, t2)){
            //debug
//            assert(brutehit(u, a, b, p, obj) == 0);
            return 0;
        }
        if(u->tri!=0){
            ld r;
            if(!u->tri->intersection(a, b, r))
                return 0;
            if(r<p){
                obj = u->tri;
                p = r;
            }
            return 1;
        }
        ld tl1, tl2, tr1, tr2;
//        cout<<"====="<<endl;
//        if(u->lc)cout<<u->lc->bbox<<endl;
        int flag1 = u->lc && u->lc->bbox.intersection(a, b, tl1, tl2);
//        cout<<a+tl1 * b<<endl;
//        cout<<a+tl2 * b<<endl;
//        assert(u->lc ==0 || flag1 || brutehit(u->lc, a, b, p, obj) == 0);
        int flag2 = u->rc && u->rc->bbox.intersection(a, b, tr1, tr2);
//        assert(u->rc ==0 || flag2 || brutehit(u->rc, a, b, p, obj) == 0);
        Triangle* aim;
        int ans = 0;
        if(tl1 < tr1 || !flag2){
            if(flag1)
                ans|=hit(u->lc, a, b, p, obj);
            if(flag2 && ( obj==0 || p > tr1 ))
                ans|=hit(u->rc, a, b, p, obj);
        }
        else{
            if(flag2)
                ans|=hit(u->rc, a, b, p, obj);
            if(flag1 && ( obj==0 || p > tl1 ))
                ans|=hit(u->lc, a, b, p, obj);
        }
        return ans;
    }

public:
    KDtree(){
        tris = tris;
        root = 0;
    }

    void addTriangle(Triangle* a){
        tris.push_back(a);
    }

    int intersection(Line ray, Point& p, Object* &surface, ld& d){
        if(tris.size() && root == 0){
            buildKDtree();
        }
        if(root == 0){
            return 0;
        }
        Point a = ray.first;
        Point b = (ray.second - ray.first).normalize();
        ld r = INF;
        Triangle* t=0;

        /*
        for(int i = 0;i<tris.size();++i){
            ld r2;
            if(tris[i]->intersection(a, b, r2)){
                if(r2<r){
                    r = r2;
                    t = tris[i];
                }
            }
        }
        */

        if(!hit(root, a, b, r, t))
            return 0;
        /*
        ld r1=INF;
        Triangle* t1=0;
        int flag = hit(root, a, b, r1, t1);
        assert(abs(max(r, 0.)-max(r1, 0.))<eps);
        */
        if(r>d || t==0)
            return 0;
        surface = t;
        d = r;
        p = a + b * r;
        return 1;
    }
};
#endif
