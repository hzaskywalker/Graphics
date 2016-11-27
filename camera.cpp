#include<opencv2/opencv.hpp>
#include "basic.cpp"
#include "shape.cpp"
class Camera{
    //Camera always begin at origin
    public:
        Point Lefttop, Rightbottom;
        float w, h;
        Camera(const Point& a, const Point& b, const float& x, const float& y){
            Lefttop = a;
            Rightbottom = b;
            w = x;
            h = y;
        }

        Point startRay(int x, int y, int Height, int Width){
            return Point( float(x)/Width, float(y)/Height ) * (Rightbottom - Lefttop) + Lefttop;
        }
};
