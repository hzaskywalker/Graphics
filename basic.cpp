#ifndef INCLUDEBASIC
#define INCLUDEBASIC 
#include<iostream>
#include<assert.h>
#include<cmath>
typedef float ld;
using namespace std;
ld eps = 1e-10;
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
        Point& operator += (const float& b){
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

        Point& operator -= (const float& b){
            x -= b;
            y -= b;
            z -= b;
            return *this;
        }
        Point& operator *= (const Point& b){
            x *= b.x;
            y *= b.y;
            z *= b.z;
            return *this;
        }
        Point& operator *= (const float& b){
            x *= b;
            y *= b;
            z *= b;
            return *this;
        }
        Point& operator /= (const float& b){
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
        ld norm2() const{
            return x * x + y * y + z * z;
        }
};

Point operator + (const Point& a, const Point& b){
    return Point(a.x + b.x, a.y + b.y, a.z + b.z);
}

Point operator + (const Point& a, const float& b){
    return Point(a.x + b, a.y + b, a.z + b);
}

Point operator + (const float& b, const Point& a){
    return Point(a.x + b, a.y + b, a.z + b);
}

Point operator - (const Point& a, const Point& b){
    return Point(a.x - b.x, a.y - b.y, a.z - b.z);
}

Point operator - (const Point& a, const float& b){
    return Point(a.x - b, a.y - b, a.z - b);
}

Point operator - (const Point& a){
    return Point(-a.x, -a.y, -a.z);
}

Point operator - (const float& b, const Point& a){
    return Point(a.x - b, a.y - b, a.z - b);
}

Point operator * (const Point& a, const Point& b){
    return Point(a.x * b.x, a.y * b.y, a.z * b.z);
}

Point operator * (const Point& a, const float& b){
    return Point(a.x * b, a.y * b, a.z * b);
}

Point operator * (const float& b, const Point& a){
    return Point(a.x * b, a.y * b, a.z * b);
}

Point operator / (const Point& a, const Point& b){
    return Point(a.x / b.x, a.y / b.y, a.z / b.z);
}

Point operator / (const Point& a, const float& b){
    return Point(a.x / b, a.y / b, a.z / b);
}

ostream& operator << (ostream& a, const Point& b){
    a<<"("<<b.x<<","<<b.y<<","<<b.z<<")";
    return a;
}

Point chaji(const Point& a, const Point& b){
    return Point(a.x * b.y - a.y * b.x, a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z);
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
    assert (abs(dt - ds)> eps);
    interp = (b.second - b.first)/(dt-ds) * ds  + b.first;
    return 1;
}

int intersectionBall(const Line& ray, const Point& O, const float& Radius, float& a, float& b){
    Point c= ray.second - ray.first;
    ld length = c.norm();
    c = c/length;

    ld mid = dianji( O - ray.first, c );
    Point project = c * mid + ray.first;
    float d = dist(O, project); 
    if(d>Radius){
        return 0;
    }
    float l = sqrt(Radius * Radius - d*d);
    a = (mid - d)/length;
    b = (mid + d)/length;
    return 1;
}

#endif
