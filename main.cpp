#include<cstdio>
#include "opencv2/opencv.hpp"
#include "basic.cpp"
#include "shape.cpp"
#include "camera.cpp"
#include "home.cpp"

void makeObject(Render& home){
    ld t6 = 1e6;
    ld delta = 1000;

    Ball* c = new Ball(Point(-200, 400, 500), 300, 2.);
    c->set_reflect_value(1.);
    c->set_transmit_val(0.);
    c->set_color(Color(1., 1., 1.));
    c->set_diffuse_value(0);

    Ball* d = new Ball(Point(200, -400, 400), 400, 1.3);
    d->set_reflect_value(0);
    d->set_transmit_val(1.);
    d->set_color(Color(1., 1., 1.));
    d->set_diffuse_value(0);

    Ball* all[] = {
        new Ball(Point(0, 0, t6 + delta), t6, 0., Color(0.75, 0.75, 0.25)),
        new Ball(Point(0, 0, -t6 - delta), t6, 0., Color(0.25, 0.25, 0.75)),
        new Ball(Point(t6 + delta, 0, 0), t6, 0., Color(0.25, 0.75, 0.25)),
        new Ball(Point(-t6 -delta, 0, 0), t6, 0., Color(0.75, 0.25, 0.25)),
        new Ball(Point(0, -t6 - delta, 0), t6, 0., Color(0.75, 0.75, 0.75)),
        new Ball(Point(0, t6 + delta, 0), t6, 0., Color(0, 0, 1.)),
    };

    home.addObj(c);
    home.addObj(d);
    for(int i = 0;i<6;++i){
        all[i]->set_reflect_value(0);
        all[i]->set_diffuse_value(1);
        home.addObj(all[i]);
    }

}

void makeEnvironment(Render& home){
    makeObject(home);
    Ball* light = new Ball(Point(0, 900, 500), 300, 1);
    light->light = Color(50, 50, 50);
    home.addObj(light);
}

void makeEnvironment2(Render& home){
    makeObject(home);
    ld L = 400;
    ld H = 999;
    ld D = 500;
    Rectangle* rec = new Rectangle(
            Point(-L, H, D -L/2),
            Point(-L, H, D + L/2),
            Point(L, H, D - L/2));
    rec->light = Color(30, 30, 30);
    ObjObj* ans = new ObjObj("objs/fixed.perfect.dragon.100K.0.07.obj");
    ans->locate(600, -200, -500, 800);

    for(int i = 0;i<ans->faces.size();++i){
        Point a = ans->pts[ans->faces[i][0]];
        Point b = ans->pts[ans->faces[i][1]];
        Point c = ans->pts[ans->faces[i][2]];
        //swap(a.x, a.z);
        //swap(b.x, b.z);
        //swap(c.x, c.z);
        Triangle* t = new Triangle(a, b, c);
        t->set_diffuse_value(1.);
        t->set_color(Point(0.75, 1, 0.75));
        home.addTriangle(t);
    }

    home.addObj(rec);
    home.addLight(rec);
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
    cout<<width<<" "<<height<<endl;
    Render task;
    Camera camera(Point(-400, 400, 500), Point(400, -400, 500), width, height);
    makeEnvironment2(task);
    cv::Mat result = camera.render(Point(0,0,-1000), task, 5, n, p);
    return 0;
}
