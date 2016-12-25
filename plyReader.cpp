#ifndef PLYREADER
#define PLYREADER
#include "shape.cpp"
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "basic.cpp"
using namespace std;

vector<string> split(string s){
    vector<string> elems;
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, ' ')) {
        elems.push_back(item);
    }
    return elems;
}

class PlyObj:public Object{
    public:
        vector<Point> pts;
        vector< vector<int> > faces;
        mutable int intersection_face;
        int num_vertex;
        int num_face;

        double minx, miny, maxx, maxy, minz, maxz;

        void locate(double scale, double dx, double dy, double dz){
            minx = INF; maxx = -INF;
            miny = INF; maxy = -INF;
            minz = INF; maxz = -INF;
            for(int i = 0;i< num_vertex; ++ i){

                pts[i].x = pts[i].x * scale + dx;
                pts[i].y = pts[i].y * scale + dy;
                pts[i].z = pts[i].z * scale + dz;

                minx = min(minx, pts[i].x);
                miny = min(miny, pts[i].y);
                minz = min(minz, pts[i].z);
                maxx = max(maxx, pts[i].x);
                maxy = max(maxy, pts[i].y);
                maxz = max(maxz, pts[i].z);
            }
            cout<<"x range: "<<minx<<" "<<maxx<<endl;
            cout<<"y range: "<<miny<<" "<<maxy<<endl;
            cout<<"z range: "<<minz<<" "<<maxz<<endl;
        }

        PlyObj(const string& filename){
            ifstream fin(filename.c_str());
            string tmp;
            num_vertex = 0;
            num_face = 0;
            int num_property = 0;
            int x_idx, y_idx, z_idx;

            while(getline(fin, tmp)){
                vector<string> splited = split(tmp);
                if(splited[0] == "end_header")
                    break;
                if(splited[0] == "element"){
                    if(splited[1] == "vertex")
                        num_vertex = atoi(splited[2].c_str());
                    if(splited[1] == "face")
                        num_face = atoi(splited[2].c_str());
                }
                if(splited[0] == "property"){
                    if(splited[2] == "x")
                        x_idx = num_property;
                    if(splited[2] == "y")
                        y_idx = num_property;
                    if(splited[2] == "z")
                        z_idx = num_property;
                    num_property += 1;
                }
            }
            for(int i = 0;i<num_vertex;++i){
                getline(fin, tmp);
                vector<string> xs = split(tmp);
                double x = atof(xs[x_idx].c_str());
                double y = atof(xs[y_idx].c_str());
                double z = atof(xs[z_idx].c_str());
                pts.push_back(Point(x, y, z));
            }
            for(int i = 0; i<num_face; ++i){
                getline(fin, tmp);
                vector<string> tt = split(tmp);
                vector<int> ans;
                int num = atoi(tt[0].c_str());
                for(int j = 0; j<num; ++j){
                    ans.push_back(atoi( tt[j+1].c_str() ) );
                }
                faces.push_back(ans);
            }
        }

    Plane get_face(int i) const{
        return Plane( pts[faces[i][0]], 
                        pts[faces[i][1]], 
                        pts[faces[i][2]]);
    }

    int intersection(const Line& ray, Point& output) const{
        Point t;
        double d = INF;
        for(int i = 0;i<num_face; ++ i){
            if( intersectionFace(ray, 
                        pts[faces[i][0]], 
                        pts[faces[i][1]], 
                        pts[faces[i][2]], t) ){
                double val = dianji(t - ray.first, ray.second - ray.first);
                if(val < d && val > 1e-3){
                    intersection_face = i;
                    d = val;
                    output = t;
                }
            }
        }
        return d != INF;
    }

    Point calc_norm(const Line& ray, const Point& interp) const{
        Point normal =  get_face(intersection_face).dst.normalize();
        int beOut = sign( dianji(normal, ray.first - interp) );
        if((beOut >= 0) != (intersection_times % 2 == 1))
            normal *= -1;
        return normal;
    }

    Color local(const Point& point) const{
        return Point(0, 0, 0);
    }
};

#endif
