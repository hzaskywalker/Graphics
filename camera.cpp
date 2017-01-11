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

        Point startRay(double x, double y, int Width, int Height){
            return Point( x/Width, y/Height ) * (Rightbottom - Lefttop) + Lefttop;
        }

        cv::Mat render(Point O, Render& home, int depth = 10, int MC = 0){
            home.Depth = depth;
            cv::Mat image(w, h, CV_8UC3, cv::Scalar(0, 0, 0));
            cv::Mat_<cv::Vec3b> _m = image;
            int num = 0, tot = w*h;
            cout<<"begin render"<<endl;
            for(int i = 0; i<w; ++i){
                for(int j = 0; j<h; ++j){
                    home.initialize();
                    cout<<"\b\r"<<((double)num)/tot * 100;
                    num += 1;
                    Color color;

                    int n = 1000;
                    for(int k = 0;k<n;++k){
                        double t = erand() * 2 * M_PI;
                        double r = erand() * 2;
                        Point direction = startRay(i + 0.5 + r*cos(t), j + 0.5 + r*sin(t), w, h).normalize();
                        Point p = O+direction;
                        color += home.rayTrace(make_pair(p-direction, p), 0, 1., 1.);
                    }
                    color/=n;

                    color.clamp(1);
                    color = color*255;
                    _m(j, i) = cv::Vec<unsigned char, 3>(color.x, color.y, color.z);
                }
                image = _m;
                cv::imwrite("tmp.png", image);
            }
            cout<<"\b\r"<<"end"<<endl;
            image = _m;
            return image;
        }
};
