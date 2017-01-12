#ifndef INCLUDEBASIC
#define INCLUDEBASIC 
#include<iostream>
#include<vector>
#include<assert.h>
#include<cmath>
typedef double ld;
using namespace std;

const double INF = 1e10;
const ld eps = 1e-6;

ld erand(){
    return rand()*1./RAND_MAX;
}

int sign(double a){
    return a>eps?1:(a>-eps?0:-1);
}

typedef vector<vector<double> > Matrix;

void outMatrix(Matrix A){
    for(int i = 0;i<A.size(); ++i){
        for(int j =0 ;j<A[i].size();++j)
            cout<<A[i][j]<<" ";
        cout<<endl;
    }
}

Matrix buildMatrix(int n, int m){
    return vector< vector<double> >(n, vector<double>(m, 0.));
}

Matrix operator + (const Matrix& a, const Matrix& b){
    Matrix c = buildMatrix(a.size(), a[0].size());
    for(int i = 0;i<a.size();++i)
        for(int j = 0;j<b[0].size();++j)
            c[i][j] = a[i][j] + b[i][j];
    return c;
}

Matrix operator * (const Matrix& a, const Matrix& b){
    Matrix c = buildMatrix(a.size(), b[0].size());
    assert(c.size() == a.size());
    assert(c[0].size() == b[0].size());
    assert(a[0].size() == b.size());
    for(int i = 0;i<a.size();++i)
        for(int j = 0;j<b[0].size();++j)
            for(int k=0;k<b.size();++k)
                c[i][j] += a[i][k] * b[k][j];
    return c;
}

Matrix Gaussian(Matrix A){
    int n = A.size();
    int m = A[0].size();
    for(int i=0;i<n;++i){
        for(int j=i;j<n;++j)
            if(A[j][i]!=0){
                for(int k=0;k<m;++k)
                    swap(A[j][k], A[i][k]);
                break;
            }
        if(A[i][i]==0)
            continue;
        ld tmp = A[i][i];
        for(int j=0;j<m;++j)
            A[i][j]/=tmp;

        for(int j=0;j<n;++j)
            if(A[j][i]!=0 && i!=j){
                ld tmp = A[j][i]/A[i][i];
                for(int k=0;k<m;++k)
                    A[j][k] -= A[i][k]*tmp;
            }
    }
    return A;
}

Matrix Inv(Matrix A){
    int n = A.size();
    Matrix C = A;
    for(int i=0;i<n;++i){
        for(int j=0;j<n;++j)
            A[i].push_back(double(i==j));
    }
    A = Gaussian(A);
    Matrix B;
    for(int i=0;i<n;++i){
        vector<ld> tmp;
        for(int j=0;j<n;++j)
            tmp.push_back(A[i][j+n]);
        B.push_back(tmp);
    }
    return B;
}

Matrix transpose(const Matrix& a){
    Matrix c = buildMatrix(a[0].size(), a.size());
    for(int i = 0;i<c.size();++i)
        for(int j = 0;j<c[i].size();++j)
            c[i][j] = a[j][i];
    return c;
}

class Point{
    public:
        ld x, y, z;
        Point(ld a = 0, ld b = 0, ld c = 0):x(a), y(b), z(c){}

        Point(Matrix a){
            if(a.size() == 0){
                x = a[0][0];
                y = a[0][1];
                z = a[0][2];
            }
            else{
                x = a[0][0];
                y = a[1][0];
                z = a[2][0];
            }
        }

        Matrix getMatrix(int n = 1) const{
            Matrix c = buildMatrix(1, 4);
            c[0][0] = x;
            c[0][1] = y;
            c[0][2] = z;
            c[0][3] = 1;
            if(n==3){
                return transpose(c);
            }
            return c;
        }

        Point& operator += (const Point& b){
            x += b.x;
            y += b.y;
            z += b.z;
            return *this;
        }
        Point& operator += (const double& b){
            x += b;
            y += b;
            z += b;
            return *this;
        }
        Point& operator -= (const Point& b){
            x -= b.x;
            y -= b.y;
            z -= b.z;
            return *this;
        }

        Point& operator -= (const double& b){
            x -= b;
            y -= b;
            z -= b;
            return *this;
        }
        void clamp(const double& a){
            x = min(x, a);
            y = min(y, a);
            z = min(z, a);
        }
        Point& operator *= (const Point& b){
            x *= b.x;
            y *= b.y;
            z *= b.z;
            return *this;
        }
        Point& operator *= (const double& b){
            x *= b;
            y *= b;
            z *= b;
            return *this;
        }
        Point& operator /= (const double& b){
            x /= b;
            y /= b;
            z /= b;
            return *this;
        }
        Point& operator /= (const Point& b){
            x /= b.x;
            y /= b.y;
            z /= b.z;
            return *this;
        }
        ld sum() const{
            return x + y + z;
        }
        ld norm() const{
            return sqrt(x * x + y * y + z * z);
        }
        Point& normalize(){
            ld a = norm();
            x/=a;
            y/=a;
            z/=a;
            return *this;
        }
        ld norm2() const{
            return x * x + y * y + z * z;
        }
};

Point operator + (const Point& a, const Point& b){
    return Point(a.x + b.x, a.y + b.y, a.z + b.z);
}

Point operator + (const Point& a, const double& b){
    return Point(a.x + b, a.y + b, a.z + b);
}

Point operator + (const double& b, const Point& a){
    return Point(a.x + b, a.y + b, a.z + b);
}

Point operator - (const Point& a, const Point& b){
    return Point(a.x - b.x, a.y - b.y, a.z - b.z);
}

Point operator - (const Point& a, const double& b){
    return Point(a.x - b, a.y - b, a.z - b);
}

Point operator - (const Point& a){
    return Point(-a.x, -a.y, -a.z);
}

Point operator - (const double& b, const Point& a){
    return Point(a.x - b, a.y - b, a.z - b);
}

Point operator * (const Point& a, const Point& b){
    return Point(a.x * b.x, a.y * b.y, a.z * b.z);
}

Point operator * (const Point& a, const double& b){
    return Point(a.x * b, a.y * b, a.z * b);
}

Point operator * (const double& b, const Point& a){
    return Point(a.x * b, a.y * b, a.z * b);
}

Point operator / (const Point& a, const Point& b){
    return Point(a.x / b.x, a.y / b.y, a.z / b.z);
}

Point operator / (const Point& a, const double& b){
    return Point(a.x / b, a.y / b, a.z / b);
}

ostream& operator << (ostream& a, const Point& b){
    a<<"("<<b.x<<","<<b.y<<","<<b.z<<")";
    return a;
}


Point chaji(const Point& a, const Point& b){
    return Point(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

ld dianji(const Point& a, const Point& b){
    return (a*b).sum();
}

class Plane{
public:
    Point src;
    Point dst;

    Plane(const Point& s, const Point& t):src(s), dst(t){}

    Plane(const Point& a, const Point& b, const Point& c){
        src = a;
        dst = chaji(b-a, c-a);
    }
    ld projectDelta(const Point& b) const{
        return dianji(b-src, dst);
    }

    Point project(const Point& b) const{
        return b-projectDelta(b) * dst/dst.norm2();
    }
};

typedef pair<Point, Point> Line;

ld dist(const Point& a, const Point& b){
    return (a-b).norm();
}

int intersectionPlane(const Plane&a, const Line& b, Point& interp){
    ld ds = a.projectDelta(b.first), dt = a.projectDelta(b.second);
    if(abs(dt - ds)< eps){
        return 0;
    }
    if(ds/(ds-dt)<eps)
        return 0;
    interp = (b.second - b.first) * (ds/(ds-dt))  + b.first;
    return 1;
}

int intersectionBall(const Line& ray, const Point& O, const double& Radius, double& a, double& b){
    Point c= ray.second - ray.first;
    ld length = c.norm();
    c = c/length;

    ld mid = dianji( O - ray.first, c );
    Point project = c * mid + ray.first;
    double d = dist(O, project); 
    if(d>Radius){
        return 0;
    }
    double l = sqrt(Radius * Radius - d*d);
    a = (mid - l)/length;
    b = (mid + l)/length;
    return 1;
}

ostream& operator << (ostream& a, const Line& b){
    a<<"Line("<<b.first<<","<<b.second<<")";
    return a;
}

#endif
