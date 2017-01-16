#include<opencv2/opencv.hpp>
#include <ctime>
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
            cv::Mat image(h, w, CV_8UC3, cv::Scalar(0, 0, 0));
            cv::Mat_<cv::Vec3b> _m = image;
            int num = 0, tot = w*h;
            cout<<"begin render"<<endl;
            ofstream fout((string(imgname) + string(".txt")).c_str());
            clock_t begin = clock();
            for(int i = 0; i<w; ++i){
                for(int j = 0; j<h; ++j){
                    cout<<"\b\r"<<((double)num)/tot * 100<<" "<<(tot-num)/(double)num * (clock() - begin)/CLOCKS_PER_SEC;
                    num += 1;
                    Color color;

                    /*
                    if(i<670 || i>680 || j<540 || j>550)
                        color = Color();
                    else{
                    */
                        for(int k = 0;k<n;++k){
                            for(int sx=0;sx<2;++sx )
                                for(int sy = 0;sy<2;++sy){
                                    double r1=2*erand(), dx=r1<1 ? sqrt(r1)-1: 1-sqrt(2-r1);
                                    double r2=2*erand(), dy=r2<1 ? sqrt(r2)-1: 1-sqrt(2-r2);
                                    Point direction = startRay(i + 0.5 + (sx + dx)/2, j + 0.5 + (sy+dy)/2, w, h).normalize();
                                    Point p = O+direction;
                                    color += home.rayTrace(make_pair(p-direction, p), 0, 1., 1.) * 0.25;
                                }
                        }
                        color/=n;

//                        cout<<"##"<<i<<" "<<j<<" "<<color<<endl;
                        color = color*255;
//                    }
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
