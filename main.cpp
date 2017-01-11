#include<cstdio>
#include "opencv2/opencv.hpp"
#include "basic.cpp"
#include "shape.cpp"
#include "camera.cpp"
#include "home.cpp"
#include "plyReader.cpp"

void makeEnvironment(Render& home){
    ld t6 = 1e6;
    ld delta = 1000;
    /*
    Ball* b = new Ball(Point(0, t6+delta - 1, 0), t6, 0.);
    b->set_reflect_value(1.);
    b->set_transmit_val(0.);
    b->set_diffuse_value(1);
    b->light = Color(3,3,3);
    */

    Ball* c = new Ball(Point(-400, -700, 600), 100, 2.);
    c->set_reflect_value(0.);
    c->set_transmit_val(1.);
    c->set_color(Color(1., 1., 1.));
    c->set_diffuse_value(0);

    Ball* d = new Ball(Point(-10, -100, 100), 400, 0.9);
    d->set_reflect_value(0);
    d->set_transmit_val(0.);
    d->set_color(Color(1., 1., 1.));
    d->set_diffuse_value(1);

    Ball* all[] = {
        new Ball(Point(0, 0, t6 + delta * 2), t6, 0., Color(0.75, 0.75, 0.25)),
        new Ball(Point(0, 0, -t6 - delta * 2), t6, 0., Color(0.25, 0.25, 0.75)),
        new Ball(Point(t6 + delta, 0, 0), t6, 0., Color(0.25, 0.75, 0.25)),
        new Ball(Point(-t6 -delta, 0, 0), t6, 0., Color(0.75, 0.25, 0.25)),
        new Ball(Point(0, -t6 - delta, 0), t6, 0., Color(0.75, 0.75, 0.75)),
        new Ball(Point(0, t6 + delta, 0), t6, 0., Color(0, 0, 0)),
    };

    home.addObj(c);
    home.addObj(d);
    for(int i = 0;i<6;++i){
        all[i]->set_reflect_value(0);
        all[i]->set_diffuse_value(1);
        home.addObj(all[i]);
    }
    all[5]->light = Color(1, 1, 1);
}

int main(){
    Render task;
    Camera camera(Point(500, 500, 800), Point(-500, -500, 800), 512, 512);
    makeEnvironment(task);
    cv::Mat result = camera.render(Point(0,0,-1000), task, 5);
    cv::imwrite("1.png", result);
    return 0;
}
