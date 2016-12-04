#include<opencv2/opencv.hpp>
#include "basic.cpp"
#include "shape.cpp"
#include "home.cpp"
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

        Point startRay(int x, int y, int Width, int Height){
            return Point( float(x)/Width, float(y)/Height ) * (Rightbottom - Lefttop) + Lefttop;
        }

        cv::Mat render(Point O, Render& home){
            cv::Mat image(w, h, CV_8UC3, cv::Scalar(0, 0, 0));
            cv::Mat_<cv::Vec3b> _m = image;
            for(int i = 0; i<w; ++i){
                for(int j = 0; j<h; ++j){
                    Point direction = startRay(i + 0.5, j + 0.5, w, h);
                    Color color = home.rayTrace(make_pair(O, O + direction), 2, 1., 1.);
                    _m(j, i) = cv::Vec<unsigned char, 3>(color.x, color.y, color.z);
                }
            }
            image = _m;
            return image;
        }
};
