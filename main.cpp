#include<cstdio>
#include "opencv2/opencv.hpp"
#include "basic.cpp"
#include "objReader.cpp"
#include "shape.cpp"
#include "camera.cpp"
#include "home.cpp"

void makeObject(Render& home){
    ld t6 = 1e6;
    ld delta = 1000;

    Ball* c = new Ball(Point(-500, -699, 600), 300, 2.);
    c->set_reflect_value(1.);
    c->set_transmit_val(0.);
    c->set_color(Color(1., 1., 1.));
    c->set_diffuse_value(0);

    Ball* d = new Ball(Point(1100, -799, 0), 200, 1.4);
    d->set_reflect_value(0);
    d->set_transmit_val(1.);
    d->set_color(Color(1., 1., 1.));
    d->set_diffuse_value(0);

    ld k = 1024./768;
    Ball* all[] = {
        new Ball(Point(0, 0, t6 + delta), t6, 0., Color(0.75, 0.75, 0.75)),
        new Ball(Point(0, 0, -t6 - delta*10), t6, 0., Color(0.75, 0.75, 0.25)),
        new Ball(Point(t6 + delta*k, 0, 0), t6, 0., Color(0.25, 0.25, 0.75)),
        new Ball(Point(-t6 -delta*k, 0, 0), t6, 0., Color(0.25, 0.75, 0.25)),
        new Ball(Point(0, -t6 - delta, 0), t6, 0., Color(0.75, 0.75, 0.75)),
        new Ball(Point(0, t6 + delta, 0), t6, 0., Color(0.75, 0.75, 0.75)),
    };

//    home.addObj(c);
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
    ld L = 300;
    ld H = 999;
    ld D = 0;
    Rectangle* rec = new Rectangle(
            Point(-L, H, D -L),
            Point(-L, H, D + L),
            Point(L, H, D - L));
    rec->light = Color(30, 30, 30);
    //ObjObj* ans = new ObjObj("objs/fixed.perfect.dragon.100K.0.07.obj");
    //法向插值啊亲
    ObjObj* ans = new ObjObj("dragon.obj");
//    ObjObj* ans = new ObjObj("objs/sphere.obj");
    ld theta = M_PI * 20 / 180 ;
    ans->locate(600, 700, -500, 500, theta);
//    ans->locate(300, 700, -500, 500, theta);


    for(int i = 0;i<ans->faces.size();++i){
        Triangle* t = ans->getTriangle(i);
        t->set_diffuse_value(0.);
        //t->set_reflect_value(1.);
        t->set_transmit_val(1.);
        t->set_eta2(1.3);
        t->set_color(Point(0.95, 0.95, 0.95));
        home.addTriangle(t);
    }


    ObjObj* ans2 = new ObjObj("objs/bunny.fine.obj");
    ans2->locate(3000, -900, -1199 + 100, 300, M_PI * 60/180.);

    for(int i = 0;i<ans2->faces.size();++i){
        Triangle* t = ans2->getTriangle(i);
        t->set_diffuse_value(1.);
        t->set_reflect_value(0.);
        t->set_transmit_val(0.);
        t->set_eta2(1.5);
        t->set_color(Point(0.95, 0.95, 0.95));
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
    ld k = 1024./768;
    width = int(height * k); 
//    cout<<width<<" "<<height<<endl;
    Render task;
//    ld x1 = (-150 + 300 * (675 - 3) / 1024) * k;
//    ld x2 = (-150 + 300 * (675 + 3) / 1024) * k;
//    ld y1 = (150 - 300 * (545 - 3) / 768);
//    ld y2 = (150 - 300 * (545 + 3) / 768);
//    ld midx = 600* (768*k);/// (100*k) ;
//    ld midy = 645. * 768;/// 100;
    ld midx = 600;
    ld midy = 645;
    ld x1 = (-150 + 300 * (midx-10) / 1024) * k;
    ld x2 = (-150 + 300 * (midx + 10) / 1024) * k;
    ld y1 = (150 - 300 * (midy-10) / 768);
    ld y2 = (150 - 300 * (midy+10) / 768);

//    Camera camera(Point(x1 , y1, 500), Point(x2, y2, 500), width, height);
    Camera camera(Point(-150*k , 150, 500), Point(150*k, -150, 500), width, height);
//    Camera camera(Point(50, -150/2, 500), Point(150*k/2 + 50, -150, 500), width, height);
//    Camera camera(Point(-200, 0, 500), Point(100, -300, 500), width, height);
    makeEnvironment2(task);
    cv::Mat result = camera.render(Point(0,0,-4000), task, 5, n, p);
    return 0;
}
