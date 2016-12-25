#include<cstdio>
#include "opencv2/opencv.hpp"
#include "basic.cpp"
#include "shape.cpp"
#include "camera.cpp"
#include "home.cpp"

void makeEnvironment(Render& home){
    Parameter paramb(0.2, 0.3, 0.3, 6, 0.6, 0.3, 6);
    Parameter paramc(0., 0.0, 0.0, 6, 0.4, 0.1, 6);
    Parameter param2(0.5, 0.4, 0.4, 4, 0.4, 0.4, 4);

    Ball* b = new Ball(Point(100, -600, 800), 200, 1.2);
    b->set_reflect_value(0.8);
    b->set_transmit_val(0.2);
    b->set_color(Color(0, 127.5, 0));
    b->set_parameter(paramb);

    //Ball* c = new Ball(Point(-201, -300, 450), 100, 1.4);
    Ball* c = new Ball(Point(-400, -700, 600), 100, 1.8);
    c->set_reflect_value(0.1);
    c->set_transmit_val(0.9);
    c->set_color(Color(0, 0, 255));
    c->set_parameter(paramc);

    ld t6 = 1e6;
    ld delta = 1000;
    Ball* all[] = {
        new Ball(Point(0, 0, t6 + delta * 2), t6, 0., Color(0, 0, 255)),
        new Ball(Point(0, t6 + delta, 0), t6, 0., Color(0, 255, 255)),
        new Ball(Point(t6 + delta, 0, 0), t6, 0., Color(255, 0, 0)),
        new Ball(Point(0, 0, -t6 - delta * 2), t6, 0., Color(123, 123, 123)),
        new Ball(Point(0, -t6 - delta, 0), t6, 0., Color(123, 222, 0)),
        new Ball(Point(-t6 -delta, 0, 0), t6, 0., Color(255, 255, 0)),
    };

    home.addObj(b);
    home.addObj(c);
    for(int i = 0;i<6;++i){
        all[i]->set_parameter(param2);
        all[i]->set_reflect_value(0);
        home.addObj(all[i]);
    }
    home.addLight(Point( -400, -400., 400. ), Color(255, 255, 255));
}

int main(){
    Render task;
    Camera camera(Point(500, 500, 800), Point(-500, -500, 800), 512, 512);
    makeEnvironment(task);
    cv::Mat result = camera.render(Point(0,0,-1000), task, 10);
    cv::imwrite("1.jpg", result);
    return 0;
}
