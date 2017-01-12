#include<opencv2/opencv.hpp>
#include <iomanip> 
#include<fstream>
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

        cv::Mat render(Point O, Render& home, int depth = 10, int n= 1000, const char* imgname = 0){
            home.Depth = depth;
            cv::Mat image(w, h, CV_8UC3, cv::Scalar(0, 0, 0));
            cv::Mat_<cv::Vec3b> _m = image;
            int num = 0, tot = w*h;
            cout<<"begin render"<<endl;
            ofstream fout((string(imgname) + string(".txt")).c_str());
            for(int i = 0; i<w; ++i){
                for(int j = 0; j<h; ++j){
                    cout<<"\b\r"<<((double)num)/tot * 100;
                    num += 1;
                    Color color;

                    for(int k = 0;k<n;++k){
                        double t = erand() * 2 * M_PI;
                        double r = erand() * 0.2;
                        Point direction = startRay(i + 0.5 + r*cos(t), j + 0.5 + r*sin(t), w, h).normalize();
                        Point p = O+direction;
                        color += home.rayTrace(make_pair(p-direction, p), 0, 1., 1.);
                    }
                    color/=n;

                    color = color*255;
                    if(imgname!=0)
                        fout<<std::setprecision(9)<<color.x<<" "<<color.y<<" "<<color.z<<" ";
                    color.clamp(255);
                    _m(j, i) = cv::Vec<unsigned char, 3>(color.x, color.y, color.z);
                }
                if(imgname!=0)
                    fout<<endl;
                image = _m;
                if(imgname!=0)
                    cv::imwrite(imgname, image);
                else
                    cv::imwrite("1.jpg", image);
            }
            cout<<"\b\r"<<"end"<<endl;
            image = _m;
            return image;
        }
};
