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
    Ball* b = new Ball(Point(0, t6+delta - 1, 0), t6, 0.);
    b->set_reflect_value(1.);
    b->set_transmit_val(0.);
    b->set_diffuse_value(1);
    b->light = Color(1,1,1);

    Ball* c = new Ball(Point(-400, -700, 600), 100, 2.);
    c->set_reflect_value(0.);
    c->set_transmit_val(1.);
    c->set_color(Color(1., 1., 1.));
    c->set_diffuse_value(1);

    Ball* d = new Ball(Point(-10, -100, 100), 400, 1.4);
    d->set_reflect_value(0);
    d->set_transmit_val(1.);
    d->set_color(Color(0.75, 0.75, 0.75));
    d->set_diffuse_value(1);

    Ball* all[] = {
        new Ball(Point(0, 0, t6 + delta * 2), t6, 0., Color(0.75, 0.75, 0.75)),
        new Ball(Point(0, t6 + delta, 0), t6, 0., Color(0.75, 0, 0)),
        new Ball(Point(t6 + delta, 0, 0), t6, 0., Color(0, 0.75, 0)),
        new Ball(Point(0, 0, -t6 - delta * 2), t6, 0., Color(0.75, 0.75, 0.75)),
        new Ball(Point(0, -t6 - delta, 0), t6, 0., Color(0.75, 0.75, 0.75)),
        new Ball(Point(-t6 -delta, 0, 0), t6, 0., Color(0, 0, 0)),
    };

    home.addObj(b);
    home.addObj(c);
    home.addObj(d);
    for(int i = 0;i<6;++i){
        all[i]->set_reflect_value(0.3);
        all[i]->set_diffuse_value(1);
        home.addObj(all[i]);
    }
}

int main(){
    Render task;
    Camera camera(Point(500, 500, 800), Point(-500, -500, 800), 512, 512);
    makeEnvironment(task);
    cv::Mat result = camera.render(Point(0,0,-1000), task, 5, 0);
    cv::imwrite("1.jpg", result);
    return 0;
}
