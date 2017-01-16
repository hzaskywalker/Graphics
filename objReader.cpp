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


Point rotateXZ(Point a, ld theta){
    ld c = cos(theta);
    ld s = sin(theta);
    return Point(a.x * c - a.z * s, a.y, a.x * s + a.z * c);
}

class ObjObj{
    public:
        vector< Point > pts;
        vector< Point > normals;
        vector< vector<int> > faces;

        double minx, miny, maxx, maxy, minz, maxz;

        Triangle* getTriangle(int i){
            int a = faces[i][0];
            int b = faces[i][1];
            int c = faces[i][2];
            Triangle *t = new  Triangle(pts[a], pts[b], pts[c], normals[a], normals[b], normals[c]);
            return t;
        }

        void locate(double scale, double dx, double dy, double dz, double rotXZ = 0){
            minx = INF; maxx = -INF;
            miny = INF; maxy = -INF;
            minz = INF; maxz = -INF;
            Point mid;

            minx = INF; maxx = -INF;
            for(int i = 0;i< pts.size(); ++ i){

                pts[i].x = pts[i].x * scale + dx;
                pts[i].y = pts[i].y * scale + dy;
                pts[i].z = pts[i].z * scale + dz;
                mid += pts[i];

                minx = min(minx, pts[i].x);
                miny = min(miny, pts[i].y);
                minz = min(minz, pts[i].z);
                maxx = max(maxx, pts[i].x);
                maxy = max(maxy, pts[i].y);
                maxz = max(maxz, pts[i].z);
            }
            mid = mid/pts.size();
            for(int i = 0;i<pts.size();++i){
                pts[i] = rotateXZ(pts[i] - mid, rotXZ) + mid;
            }
            cout<<"x range: "<<minx<<" "<<maxx<<endl;
            cout<<"y range: "<<miny<<" "<<maxy<<endl;
            cout<<"z range: "<<minz<<" "<<maxz<<endl;
            int flag = 0;
            for(int i = 0;i<faces.size();++i){
                int a = faces[i][0];
                int b = faces[i][1];
                int c = faces[i][2];
                if(pts[a].y == maxy || pts[b].y == maxy || pts[c].y==maxy){
                    Point n = chaji(pts[b] - pts[a], pts[c] - pts[a]);
                    if(dianji(n, Point(0, 1, 0))<0){
                        flag = 1;
                        break;
                    }
                }
            }
            if(flag){
                for(int i = 0;i<faces.size();++i)
                    swap(faces[i][1], faces[i][2]);
            }
            normals = vector<Point>(pts.size());
            for(int i = 0; i< faces.size(); ++i){
                int a = faces[i][0], b = faces[i][1], c = faces[i][2];
                Point n = chaji(pts[b] - pts[a], pts[c] - pts[a]).normalize();
                normals[a] += n;
                normals[b] += n;
                normals[c] += n;
            }
            for(int i = 0; i<pts.size(); ++i){
                normals[i] = normals[i].normalize();
            }
        }

        ObjObj(vector<Point> _pts, vector<vector<int> > _faces){
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
};

#endif
