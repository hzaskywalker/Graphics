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
    Matrix bb;
    bb.push_back(Q[0]);
    bb.push_back(Q[1]);
    bb.push_back(Q[2]);
    bb.push_back( Point(0, 0, 0).getMatrix(1)[0] );
    Matrix invA = Inv(bb);
    Point v( invA * Point(0, 0, 0).getMatrix(3) );

    if(dist(v, a) + dist(v, b) < dist(a, b) * 3 ){
        return v;
    }
    return (a+b)/2;
}
struct Edge;

struct Node{
    Matrix Q;
    Point pts;
    double E;
    int fix;
    Node(){
        Q = buildMatrix(4, 4);
        fix = 0;
    }
};

struct Edge{
    Edge* next;
    Edge* rev;
    double E;
    Node* t;
    int face, used;
    Edge(){
        next =rev = 0;
        used = 0;
        face = -1;
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

    void bfs(int s = 0){
        queue<int> que;
        for(int i = 0;i<faceEdges[s].size();++i)
            faceEdges[s][i]->used = 1;
        que.push(s);
        numFace = 0;
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
                        std::reverse(faces[v].begin(), faces[v].end());
                        std::reverse(faceEdges[v].begin(), faceEdges[v].end());
                    }
                }
                else{
                    int flag = (r->t==0);
                    if(r->t == now->t || r->t == 0){
                        r->used = 1;
                        assert(r->face == -1);
                        r->t = uedge[(i-1+uedge.size())%uedge.size()]->t;
                    }
                    if(flag){
                        r->t->fix = 1;
                        now->t->fix = 1;
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
                edges[aim].rev = edges + (aim^1);
                tmp.push_back(edges + aim);
            }
            faceEdges.push_back(tmp);
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
            if(0){
                for(int j = 0;j<3;++j){
                    assert(faceEdges[i][j]->t == nodes + faces[i][j]);
                    assert(faceEdges[i][j]->rev->t == nodes + faces[i][(j+1)%3]);
                    assert(faceEdges[i][j]->next == faceEdges[i][(j-1 + 3)%3]);
                    assert(faceEdges[i][j]->next->rev->t == faceEdges[i][j]->t);
                }
            }
        }

        for(int i = 0;i<pts.size();++i){
            nodes[i].pts = pts[i];
            nodes[i].E = calcE(pts[i], nodes[i].Q);
        }
    }

    void link(Edge* a, Edge* b){
        a->rev = b;
        b->rev = a;
    }

    void deleteEdge(Edge* i){
        if(i->face == -1)
            return;
        assert(i->next->next->next == i);

        link(i->next->rev, i->next->next->rev);
        i->next->rev = 0;
        i->next->next->rev = 0;
    }

    void update(Edge* e, Node* newa){
        assert(e->t->fix == 0);
        for(Edge* i = e->next->rev;i!=e;i=i->next->rev){
            //cout<<i-edges<<" "<<i->t-nodes<<" "<<i->next->rev->t-nodes<<" "<<i-edges<<" "<<i->next->rev-edges<<" "<<newa-nodes<<endl;
            assert(i->t == e->t);
            i->t = newa;
            assert(i->rev!=0);
            Edge* p = i->t>i->rev->t?i:i->rev;
            if(p->used || p->rev->used){
                p->E = i->t->E + i->rev->t->E;
                p->used = 1;
                p->rev->used = 0;
                if(p->t->fix == 0 && p->rev->t->fix == 0)
                    heap.insert( make_pair(p->E, p) );
            }
        }
    }

    void work(double rate){
        int num = 0;
        for(Edge* i = edges;;i++){
            num += 1;
            //if(i-edges == 11441)
                //cout<<i->t-nodes<<" "<<i->next->rev->t-nodes<<" "<<i-edges<<" "<<i->next->rev-edges<<endl;
            if(i->rev == 0 || (i->face == -1 && i->rev->face == -1))
                break;
            if(!i->used)
                continue;
            Node* b = i->t;
            Node* a = i->rev->t;
            if(a->fix || b->fix)
                continue;
            //debug
            assert(i->next->rev->t == b);
            if(a<b){
                i->E = a->E + b->E;
                i->used = 1;
                i->rev->used = 0;
                heap.insert(make_pair(i->E, i));
            }
        }
        int need = (1. - rate) * numFace/2;
        while(need--){
            pair<double, Edge*> tmp;
            while(!heap.empty()){
                tmp = *heap.begin();
                heap.erase(tmp);
                if(tmp.first == tmp.second->E && tmp.second->used && tmp.second->rev)
                    break;
            }
            Edge* i = tmp.second;
            assert(i->rev!=0);
            Node* b = i->t;
            Node* a = i->rev->t;
            assert(a->fix == 0);
            assert(b->fix == 0);
            assert(b>a);

            b->Q = a->Q + b->Q;
            b->pts = solveV(b->Q, a->pts, b->pts);
            b->E = calcE(b->pts, b->Q);

            update(i, b);
            update(i->rev, b);

            deleteEdge(i);
            deleteEdge(i->rev);

            i->rev->rev = 0;
            i->rev = 0;
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
                //assert(faceEdges[i][j]->used || faceEdges[i][j]->rev->used);
                //assert(faceEdges[i][j]->rev->rev == faceEdges[i][j]);
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
    ObjObj a = ObjObj("objs/fixed.perfect.dragon.100K.0.07.obj");
    Simplify t(a.pts, a.faces, 0.1);
    t.output(a.pts, a.faces);
    a.output("tmp.obj");
    return 0;
}
