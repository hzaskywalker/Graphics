#include<cstdio>
#include "opencv2/opencv.hpp"
#include "basic.cpp"
#include "shape.cpp"
#include "camera.cpp"
#include "home.cpp"

void makeEnvironment(Render& home){
    Ball* b = new Ball(Point(0, 0, 250), 100, 0.8);
    b->set_reflect_value(0.9);
    b->set_transmit_val(1.);
    b->set_color(Color(0, 127.5, 0));
    home.addObj(b);

    home.addLight(Point( 250, 0, 250), Color(255, 255, 255));
}

int main(){
    Render task;
    Camera camera(Point(250, 250, 250), Point(-250, -250, 250), 512, 512);
    makeEnvironment(task);
    cv::Mat result = camera.render(Point(0,0,0), task, 20);
    cv::imwrite("1.jpg", result);
    return 0;
}
