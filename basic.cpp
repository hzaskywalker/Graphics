#ifndef INCLUDEBASIC
#define INCLUDEBASIC 
#include<iostream>
#include<assert.h>
#include<cmath>
typedef double ld;
using namespace std;

const double INF = 1e10;
const ld eps = 1e-6;

int sign(double a){
    return a>eps?1:(a>-eps?0:-1);
}

class Point{
    public:
        ld x, y, z;
        Point(ld a = 0, ld b = 0, ld c = 0):x(a), y(b), z(c){}
        Point& operator += (const Point& b){
            x += b.x;
            y += b.y;
            z += b.z;
            return *this;
        }
        Point& operator += (const double& b){
            x += b;
            y += b;
            z += b;
            return *this;
        }
        Point& operator -= (const Point& b){
            x -= b.x;
            y -= b.y;
            z -= b.z;
            return *this;
        }

        Point& operator -= (const double& b){
            x -= b;
            y -= b;
            z -= b;
            return *this;
        }
        void clamp(const double& a){
            x = min(x, a);
            y = min(y, a);
            z = min(z, a);
        }
        Point& operator *= (const Point& b){
            x *= b.x;
            y *= b.y;
            z *= b.z;
            return *this;
        }
        Point& operator *= (const double& b){
            x *= b;
            y *= b;
            z *= b;
            return *this;
        }
        Point& operator /= (const double& b){
            x /= b;
            y /= b;
            z /= b;
            return *this;
        }
        Point& operator /= (const Point& b){
            x /= b.x;
            y /= b.y;
            z /= b.z;
            return *this;
        }
        ld sum() const{
            return x + y + z;
        }
        ld norm() const{
            return sqrt(x * x + y * y + z * z);
        }
        Point& normalize(){
            ld a = norm();
            x/=a;
            y/=a;
            z/=a;
            return *this;
        }
        ld norm2() const{
            return x * x + y * y + z * z;
        }
};

Point operator + (const Point& a, const Point& b){
    return Point(a.x + b.x, a.y + b.y, a.z + b.z);
}

Point operator + (const Point& a, const double& b){
    return Point(a.x + b, a.y + b, a.z + b);
}

Point operator + (const double& b, const Point& a){
    return Point(a.x + b, a.y + b, a.z + b);
}

Point operator - (const Point& a, const Point& b){
    return Point(a.x - b.x, a.y - b.y, a.z - b.z);
}

Point operator - (const Point& a, const double& b){
    return Point(a.x - b, a.y - b, a.z - b);
}

Point operator - (const Point& a){
    return Point(-a.x, -a.y, -a.z);
}

Point operator - (const double& b, const Point& a){
    return Point(a.x - b, a.y - b, a.z - b);
}

Point operator * (const Point& a, const Point& b){
    return Point(a.x * b.x, a.y * b.y, a.z * b.z);
}

Point operator * (const Point& a, const double& b){
    return Point(a.x * b, a.y * b, a.z * b);
}

Point operator * (const double& b, const Point& a){
    return Point(a.x * b, a.y * b, a.z * b);
}

Point operator / (const Point& a, const Point& b){
    return Point(a.x / b.x, a.y / b.y, a.z / b.z);
}

Point operator / (const Point& a, const double& b){
    return Point(a.x / b, a.y / b, a.z / b);
}

ostream& operator << (ostream& a, const Point& b){
    a<<"("<<b.x<<","<<b.y<<","<<b.z<<")";
    return a;
}


Point chaji(const Point& a, const Point& b){
    return Point(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

ld dianji(const Point& a, const Point& b){
    return (a*b).sum();
}

class Plane{
public:
    Point src;
    Point dst;

    Plane(const Point& s, const Point& t):src(s), dst(t){}

    Plane(const Point& a, const Point& b, const Point& c){
        src = a;
        dst = chaji(b-a, c-a);
    }
    ld projectDelta(const Point& b) const{
        return dianji(b-src, dst);
    }

    Point project(const Point& b) const{
        return b-projectDelta(b) * dst/dst.norm2();
    }
};

typedef pair<Point, Point> Line;

ld dist(const Point& a, const Point& b){
    return (a-b).norm();
}

int intersection(const Plane&a, const Line& b, Point& interp){
    ld ds = a.projectDelta(b.first), dt = a.projectDelta(b.second);
    if(abs(dt - ds)< eps){
        return 0;
    }
    interp = (b.second - b.first) * (ds/(ds-dt))  + b.first;
    return 1;
}

int intersectionBall(const Line& ray, const Point& O, const double& Radius, double& a, double& b){
    Point c= ray.second - ray.first;
    ld length = c.norm();
    c = c/length;

    ld mid = dianji( O - ray.first, c );
    Point project = c * mid + ray.first;
    double d = dist(O, project); 
    if(d>Radius){
        return 0;
    }
    double l = sqrt(Radius * Radius - d*d);
    a = (mid - l)/length;
    b = (mid + l)/length;
    return 1;
}

int intersectionFace(const Line& ray, const Point& a, const Point& b, const Point& c, Point& p){
    if(!intersection(Plane(a, b, c), ray, p))
        return 0;
    Point s1 = chaji(a - p, b - p);
    Point s2 = chaji(b - p, c - p);
    Point s3 = chaji(c - p, a - p);
    int i = sign( dianji(s1, s2) );
    int j = sign( dianji(s2, s3) );
    return i>=0 && j>=0;
}

ostream& operator << (ostream& a, const Line& b){
    a<<"Line("<<b.first<<","<<b.second<<")";
    return a;
}

#endif
