#include <algorithm>
#include <queue>
#include <cstring>
#include <cstdlib>
#include <map>
#include <set>
#include "objReader.cpp"
#include <vector>
#include "assert.h"

double calcE(const Point& v, ld Q[16]){
    return (v.x * Q[0] + v.y * Q[4] + v.z * Q[8] + Q[12])*v.x
        +(v.x * Q[1] + v.y * Q[5] + v.z * Q[9] + Q[13])*v.y
        +(v.x * Q[2] + v.y * Q[6] + v.z * Q[10] + Q[14])*v.z
        +(v.x * Q[3] + v.y * Q[7] + v.z * Q[11] + Q[15]);
}

void solveV(ld P[16], Point& v, const Point& a, const Point& b){
    ld tmp;
    ld Q[16];
    memcpy(Q, P, sizeof Q);
    if(fabs(Q[0])>eps){
        tmp = Q[4]/Q[0];
        if(fabs(tmp)>eps){Q[4] -= tmp * Q[0];Q[5] -= tmp * Q[1];Q[6] -= tmp * Q[2];Q[7] -= tmp * Q[3];}
        tmp = Q[8]/Q[0];
        if(fabs(tmp)>eps){Q[8] -= tmp * Q[0];Q[9] -= tmp * Q[1];Q[10] -= tmp * Q[2];Q[11] -= tmp * Q[3];}
    }
    if(Q[1]){
        tmp = Q[9]/Q[5];
        if(fabs(tmp)>eps){Q[8] -= tmp * Q[4];Q[9] -= tmp * Q[5];Q[10] -= tmp * Q[6];Q[11] -= tmp * Q[7];}
    }
    if(fabs(Q[10])>eps)v.z = -Q[11]/(Q[10]);else v.z = (a.z+b.z)/2;
    if(fabs(Q[5])>eps)v.y = (-Q[7]-(Q[6] * v.z))/Q[5];else v.z = (a.y+b.y)/2;
    if(fabs(Q[0])>eps)v.x = (-Q[3]-(Q[2] * v.z + Q[1] * v.y))/Q[0];else v.x = (a.x+b.x)/2;
}

struct Edge;

struct Node{
    ld Q[16];
    Point pts;
    Edge* begin;
    Node(){
        memset(Q, 0, sizeof Q);
        begin = 0;
    }
};

struct Edge{
    Edge* next, *hold;
    Edge* rev, *ne;
    double E;
    Node* t;
    Point v;
    ld Q[16];
    int face, used, fix;
    Edge(){
        next =rev = 0;
        used = 0;
        face = -1;
        fix = 0;
    }

    void add(ld Q1[16], ld Q2[16]){
        for(int i = 0;i<16;++i)Q[i] = Q1[i] + Q2[i];
    }
    void update(){
        add(t->Q, rev->t->Q);
        solveV(Q, v, t->pts, rev->t->pts);
        E = calcE(v, Q);
    }
};

class Simplify{
    vector<Point> pts;
    vector< vector<int> > faces;
    vector<int> realFaces; // for debug
    vector< vector<Edge*> > faceEdges;

    map< pair<int, int>, int> label;
    set< pair<ld, Edge*> > heap;
    Edge* edges;
    Node* nodes;
    int totE, numFace;
    void outputFace(Edge* i){
        cout<<i->t-nodes<<" "<<i->next->t-nodes<<" "<<i->next->next->t - nodes<<endl;
    }


    void bfs(){
        numFace = 0;
        for(int s =0;s<faces.size();++s){
            if(faceEdges[s][0]->used)
                continue;
            queue<int> que;
            for(int i = 0;i<faceEdges[s].size();++i)
                faceEdges[s][i]->used = 1;
            que.push(s);
            while(!que.empty()){
                numFace += 1;
                int u = que.front();
                realFaces.push_back(u);
                que.pop();

                vector<Edge*>& uedge = faceEdges[u];
                for(int i = 0;i<uedge.size();++i){
                    Edge* now = uedge[i];
                    uedge[(i+1)%uedge.size()]->next = now;

                    Edge* r = now->rev;
                    if(r->used==0 && r->face!=-1){
                        int v = r->face;
                        que.push(v);

                        for(int i = 0;i<faceEdges[v].size();++i)
                            faceEdges[v][i]->used = 1;
                        if(r->t == now->t){
                            for(int j = 0;j<faceEdges[v].size();++j)
                                faceEdges[v][j]->t = nodes + faces[v][(j + 1)%faces[v].size()];
                            std::reverse(faces[v].begin()+1, faces[v].end());
                            std::reverse(faceEdges[v].begin(), faceEdges[v].end());
                        }
                    }
                    else{
                        if(r->t == now->t || r->t == 0){
                            r->used = 1;
                            r->t = uedge[(i+1+uedge.size())%uedge.size()]->t;
                        }
                    }
                }
            }
        }
        cout<<numFace<<" "<<faces.size()<<endl;
    }

    void build(){
        map< pair<int, int>,int >::iterator it;

        int numE = 0;
        for(int i = 0; i < faces.size(); ++i){
            numE += faces[i].size();
        }

        edges = new Edge[numE*2 + 10];
        nodes = new Node[pts.size() + 10];

        totE = 0;
        for(int i = 0;i<faces.size();++i){
            vector<Edge*> tmp;
            for(int j =0;j<(int)faces[i].size();++j){
                int a = faces[i][j];
                int b = faces[i][(j+1)%faces[i].size()];
                if(a>b)
                    swap(a, b);
                pair<int, int> t = make_pair(a, b);
                int& p = label[t];
                int aim = p*2 - 1;
                if(p==0){
                    p=++totE;
                    aim = p*2 - 2;
                }
                edges[aim].face = i;
                edges[aim].t = nodes + faces[i][j];
                tmp.push_back(edges + aim);
            }
            faceEdges.push_back(tmp);
        }

        for(int i =0;i<2*totE;++i){
            edges[i].rev = edges + (i^1);
            edges[i].hold = edges + (i - (i & 1));
        }
        bfs();
        for(int i = 0;i<faces.size();++i){

            Point& a = pts[faces[i][0]];
            Point n = chaji(pts[faces[i][1]]-a, pts[faces[i][2]]-a).normalize();
            ld x = n.x, y = n.y, z = n.z, w = -n.x*a.x - n.y *a.y - n.z*a.z;
            ld Q[16] = {x*x, x*y, x*z, x*w,y*x, y*y, y*z, y*w,z*x, z*y, z*z, z*w, w*x, w*y, w*z, w*w};
            for(int j =0;j<3;++j){
                ld* t = nodes[faces[i][j]].Q;
                for(int i = 0;i<16;++i)
                    t[i] += Q[i]; 
            }
        }
        for(int i = 0;i<pts.size();++i)
            nodes[i].pts = pts[i];
    }

    void link(Edge* a, Edge* b){
        a->rev = b;
        b->rev = a;
        a->hold = a;
        b->hold = a;
    }

    int deleteEdge(Edge* i){
        if(i->face==-1)
            return 0;
        link(i->next->rev, i->next->next->rev);
        i->next->rev = 0;
        i->next->next->rev = 0;
        return 1;
    }

    Edge* update(Node* e, Node* newa){
        while(e->begin && e->begin->rev == 0){
            e->begin = e->begin->ne;
        }
        Edge* i = e->begin;
        while(i){
            i->t = newa;

            Edge* p = i->hold;
            if(p->fix == 0){
                p->update();
                heap.insert( make_pair(p->E, p) );
            }

            while(i->ne && i->ne->rev == 0)
                i->ne = i->ne->ne;
            if(!i->ne)
                break;
            i = i->ne;
        }
        return i;
    }

    void work(double rate){
        int num = 0;
        Edge* i;
        for(i = edges;;i++){
            num += 1;
            if(i->rev == 0)
                break;
            i->ne = i->t->begin; i->t->begin = i;
            if(i->hold!=i)
                continue;
            i->update();
            heap.insert(make_pair(i->E, i));
        }
        int need = (1. - rate) * numFace;
        set<int> tt;
        int maxHeap = 0;
        cout<<need<<endl;
        while(need && !heap.empty()){
            pair<double, Edge*> tmp;
            maxHeap = max(maxHeap, (int)heap.size());
            while(!heap.empty()){
                tmp = *heap.begin();
                heap.erase(tmp);
                if(tmp.first == tmp.second->E && tmp.second->rev && tmp.second->hold == tmp.second){
                    break;
                }
            }
            Edge* i = tmp.second;
            Node* b = i->t;
            Node* a = i->rev->t;
            Edge* j;
            tt.clear();
            for(j = b->begin;j;j=j->ne){
                if(j->rev){
                    if(i->next == j->rev)
                        continue;
                    if(j->next == i->rev)
                        continue;
                    tt.insert(j->rev->t- nodes);
                }
            }
            for(j = a->begin;j;j=j->ne){
                if(j->rev && tt.find(j->rev->t-nodes)!=tt.end()){
                    if(i->rev->next == j->rev)
                        continue;
                    if(j->next == i)
                        continue;
                    break;
                }
            }
            if(j){
                i->fix = 1;
            }
            if(i->fix){
                continue;
            }
            b->pts = i->v;
            memcpy(b->Q, i->Q, sizeof b->Q);

            need -= deleteEdge(i->rev);
            need -= deleteEdge(i);
            i->rev->rev = 0;
            i->rev = 0;

            Edge* p = update(b, b);
            update(a, b);
            if(p)
                p->ne = a->begin;
        }
        cout<<"maxHeap size "<<maxHeap<<endl;
    }

    void outputAll(vector<Point>& pts, vector< vector<int> >& faces){
        map<Node*, int> haha;
        int tot = 0;
        for(int i = 0;i<faceEdges.size();++i){
            if(faceEdges[i][0]->rev==0){
                continue;
            }
            vector<int> tmp;
            for(int j = 0;j<faceEdges[i].size();++j){
                int& p = haha[faceEdges[i][j]->t];
                if(p==0){
                    p = ++tot;
                    pts.push_back(faceEdges[i][j]->t->pts);
                }
                tmp.push_back(p-1);
            }
            faces.push_back(tmp);
        }
    }

    public:
        Simplify(vector<Point> _pts, vector< vector<int> > _faces, double rate){
            edges = 0;
            nodes = 0;
            pts = _pts;
            faces = _faces;

            build();
            work(rate);
        }

        void output(vector<Point>& _pts, vector< vector<int> >& _faces){
            vector< Point > pts;
            vector< vector<int> > face;
            outputAll(pts, face);
            _pts = pts;
            _faces = face;
            cout<<_pts.size()<<" "<<_faces.size()<<endl;

            if(0){
                _pts = pts;
                vector< vector<int> > face;
                for(int i =0;i<realFaces.size();++i)
                    face.push_back(faces[realFaces[i]]);
                _faces = face;
            }
        }

        ~Simplify(){
            if(edges!=0){
                delete[] edges;
            }
            if(nodes!=0){
                delete[] nodes;
            }
        }
};

int main(int argv, char* argc[]){
    if(argv!=4){
        cout<<"Usage: ./simplify inp.obj rate oup.obj"<<endl;
        exit(0);
    }
    ObjObj a = ObjObj(argc[1]);
    ld rate = atof(argc[2]);
    Simplify t(a.pts, a.faces, rate);
    t.output(a.pts, a.faces);
    a.output(argc[3]);
    return 0;
}
