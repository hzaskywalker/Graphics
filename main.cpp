#include<cstdio>
#include "opencv2/opencv.hpp"
#include "basic.cpp"
#include "shape.cpp"
#include "camera.cpp"
#include "home.cpp"

void makeEnvironment(Render& home){
    Ball* b = new Ball(Point(0, 0, 250), 100, 0.8);
    b->set_reflect_value(0.3);
    b->set_transmit_val(0.3);
    b->set_color(Color(255, 0, 255));
    home.addObj(b);
}

int main(){
    Render task;
    Camera camera(Point(250, 250, 250), Point(-250, -250, 250), 512, 512);
    makeEnvironment(task);
    cv::Mat result = camera.render(Point(0,0,0), task, 10);
    cv::imwrite("1.jpg", result);
    return 0;
}
