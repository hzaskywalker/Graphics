#include<opencv2/opencv.hpp>
#include <algorithm>
#include <queue>
#include "objReader.cpp"
#include <vector>
#include "assert.h"

double calcE(const Point& v, const Matrix& Q){
    return (v.getMatrix(1) * Q * v.getMatrix(3))[0][0];
}

Point solveV(const Matrix& Q, const Point& a, const Point& b){
    Point v = (a+b)/2;

    ld va = calcE(a, Q);
    ld vb = calcE(a, Q);
    ld vv = calcE(v, Q);
    if(va<vb && va<vv)
        return a;
    if(vb < vv)
        return b;
    return v;
    /*

    if(dist(v, a) + dist(v, b) < dist(a, b)*2 ){
        return v;
    }
    return (a+b)/2;
    */
}
struct Edge;

struct Node{
    Matrix Q;
    Point pts;
    Edge* begin;
    double E;
    int del;
    Node(){
        Q = buildMatrix(4, 4);
        begin = 0;
        del = 0;
    }
};

struct Edge{
    Edge* next, *hold;
    Edge* rev, *ne;
    double E;
    Node* t;
    Point v;
    Matrix Q;
    int face, used, fix;
    Edge(){
        next =rev = 0;
        used = 0;
        face = -1;
        fix = 0;
    }
    void update(){
        Q = t->Q + rev->t->Q;
        v = solveV(Q, t->pts, rev->t->pts);
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


    Matrix planeFunction(Point a, Point b, Point c){
        Point n = chaji(b-a, c-a);
        n.normalize();
        Matrix d = buildMatrix(4, 1);
        d[0][0] = n.x;
        d[1][0] = n.y;
        d[2][0] = n.z;
        d[3][0] = -n.x*a.x - n.y *a.y - n.z*a.z;
        return d;
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
                assert(uedge.size()==3);
                for(int i = 0;i<uedge.size();++i){
                    Edge* now = uedge[i];
                    uedge[(i+1)%uedge.size()]->next = now;

                    Edge* r = now->rev;
                    if(r->used==0 && r->face!=-1){
                        int v = r->face;
                        que.push(v);

                        for(int i = 0;i<faceEdges[v].size();++i)
                            faceEdges[v][i]->used = 1;
                        assert(r->used == 1);
                        if(r->t == now->t){
                            for(int j = 0;j<faceEdges[v].size();++j)
                                faceEdges[v][j]->t = nodes + faces[v][(j + 1)%faces[v].size()];
                            std::reverse(faces[v].begin()+1, faces[v].end());
                            std::reverse(faceEdges[v].begin(), faceEdges[v].end());
                        }
                    }
                    else{
                        assert(r->rev == now);
                        if(r->t == now->t || r->t == 0){
                            r->used = 1;
                            assert(r->face == -1);
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
                assert(edges[aim].face==-1);
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
            Matrix p = planeFunction(
                    pts[faces[i][0]], 
                    pts[faces[i][1]], 
                    pts[faces[i][2]]);
            Matrix K = p*transpose(p);
            for(vector<int>::iterator it = faces[i].begin();it!=faces[i].end();++it){
                nodes[*it].Q = nodes[*it].Q + K;
            }
            //for debug
            if(1){
                for(int j = 0;j<3;++j){
                    assert(faceEdges[i][j]->t == nodes + faces[i][j]);
                    assert(faceEdges[i][j]->rev->t == nodes + faces[i][(j+1)%3]);
                }
            }
        }

        for(int i = 0;i<pts.size();++i){
            nodes[i].pts = pts[i];
            nodes[i].E = calcE(pts[i], nodes[i].Q);
        }
    }

    void link(Edge* a, Edge* b){
        assert(a->rev!=0);
        assert(b->rev!=0);
        a->rev = b;
        b->rev = a;
        a->hold = a;
        b->hold = a;
    }

    void deleteEdge(Edge* i){
        if(i->face==-1)
            return;
        link(i->next->rev, i->next->next->rev);
        i->next->rev = 0;
        i->next->next->rev = 0;
    }

    Edge* update(Node* e, Node* newa){
        while(e->begin && e->begin->rev == 0){
            e->begin = e->begin->ne;
        }
        Edge* i = e->begin;
        while(i){
            assert(i->rev);
            assert(i->t == e);
            assert(i->rev->t != newa);
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
            //if(i-edges == 11441)
                //cout<<i->t-nodes<<" "<<i->next->rev->t-nodes<<" "<<i-edges<<" "<<i->next->rev-edges<<endl;
            if(i->rev == 0)
                break;
            i->ne = i->t->begin; i->t->begin = i;
            if(i->hold!=i)
                continue;
            i->update();
            heap.insert(make_pair(i->E, i));
        }
        int need = (1. - rate) * numFace/2;
        set<int> tt;
        while(need-- && !heap.empty()){
            //dd.clear();
            pair<double, Edge*> tmp;
            while(!heap.empty()){
                tmp = *heap.begin();
                heap.erase(tmp);
                if(tmp.first == tmp.second->E && tmp.second->rev && tmp.second->hold == tmp.second){
                    break;
                }
            }
            Edge* i = tmp.second;
            assert(i->rev!=0);
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
                need++;
                continue;
            }
            b->pts = i->v;
            b->Q = i->Q;

            assert(a!=b);
            deleteEdge(i->rev);
            deleteEdge(i);
            i->rev->rev = 0;
            i->rev = 0;

            Edge* p = update(b, b);
            update(a, b);
            a->del = 1;
            if(p){
                assert(p->ne == 0);
                p->ne = a->begin;
            }
            
            /*
            Node* dd = nodes + 282;
                for(Edge* t = dd->begin;t;t=t->ne){
                    cout<<t-edges<<" ";}
                cout<<endl;
                */
        }
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

int main(){
    ObjObj a = ObjObj("tmp2.obj");
    Simplify t(a.pts, a.faces, 1);
    t.output(a.pts, a.faces);
    a.output("tmp2.obj");
    return 0;
}
