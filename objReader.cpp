#ifndef OBJREADER
#define OBJREADER
#include "shape.cpp"
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "basic.cpp"
using namespace std;

vector<string> split(string s, char tt = ' '){
    vector<string> elems;
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, tt)) {
        elems.push_back(item);
    }
    return elems;
}

class ObjObj:public Object{
    public:
        vector< Point > pts;
        vector< vector<int> > faces;

        mutable int intersection_face;
        double minx, miny, maxx, maxy, minz, maxz;

        void locate(double scale, double dx, double dy, double dz){
            minx = INF; maxx = -INF;
            miny = INF; maxy = -INF;
            minz = INF; maxz = -INF;
            for(int i = 0;i< pts.size(); ++ i){

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

        ObjObj(vector<Point> _pts, vector< vector<int> > _faces){
            pts = _pts;
            faces = _faces;
        }

        ObjObj(const string& filename){
            ifstream fin(filename.c_str());
            string tmp;
            while(getline(fin, tmp)){
                vector<string> splited = split(tmp);
                if(splited.size()==0)
                    continue;
                if(splited[0] == "v"){
                    pts.push_back(Point(
                                atof(splited[1].c_str()),
                                atof(splited[2].c_str()),
                                atof(splited[3].c_str())));
                }
                else if(splited[0] == "f"){
                    vector<int> ans;
                    for(int j = 0; j<3; ++j)
                        ans.push_back(atoi( split(splited[j+1], '/')[0].c_str() ) - 1 );
                    faces.push_back(ans);
                }
            }
        }

        void output(const string& filename){
            ofstream fout(filename.c_str());
            for(size_t i = 0; i<pts.size(); ++i){
                fout<<"v "<<pts[i].x<<" "<<pts[i].y<<" "<<pts[i].z<<endl;
            }
            for(size_t i = 0; i<faces.size();++i){
                fout<<"f "<<faces[i][0]+1<<" "<<faces[i][1]+1<<" "<<faces[i][2]+1<<endl;
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
            for(int i = 0;i<faces.size(); ++ i){
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
