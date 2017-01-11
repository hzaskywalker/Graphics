#include<cstdio>
#include "opencv2/opencv.hpp"
#include "basic.cpp"
#include "shape.cpp"
#include "camera.cpp"
#include "home.cpp"

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
    c->set_reflect_value(1.);
    c->set_transmit_val(0.);
    c->set_color(Color(1., 1., 1.));
    c->set_diffuse_value(0);

    Ball* d = new Ball(Point(-10, -100, 100), 400, 1.5);
    d->set_reflect_value(0);
    d->set_transmit_val(1.);
    d->set_color(Color(1., 1., 1.));
    d->set_diffuse_value(0);

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

    Ball* light = new Ball(Point(0, 900, 300), 300, 1);
    light->light = Color(50, 50, 50);
    home.addObj(light);
}

void makeEnvironment2(Render& home){
}

int main(int argc, char* argv[]){
    if(argc == 1){
        cout<<"Usage: ./main times oup.png srand width height"<<endl;
        return 0;
    }
    int n = 10;
    if(argc>=2)
        n = atoi(argv[1]);
    char* p = 0;
    if(argc>=3)
        p = argv[2];
    if(argc>=4)
        srand(atoi(argv[3]));
    int width = 512, height = 512;
    if(argc>=6){
        width = atoi(argv[4]);
        height = atoi(argv[5]);
    }
    Render task;
    Camera camera(Point(500, 500, 800), Point(-500, -500, 800), width, height);
    makeEnvironment(task);
    cv::Mat result = camera.render(Point(0,0,-1000), task, 5, n, p);
    return 0;
}
