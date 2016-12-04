#ifndef INCLUDEOBJREADER
#define INCLUDEOBJREADER
//copyed from http://jackraken.github.io/2014/07/24/5-opengl-loadOBJ/

#include<vector>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include "basic.cpp"
using namespace std;

class ObjLoader {
    public:
        ObjLoader();
        bool OLoadObjFromFileVertexAndNormal(const char* path,std::vector<Point>* vertices
                ,std::vector<Point>* normals) {
            char lineHeader[1000];
            std::vector<float> temp_vertices;
            std::vector<float> temp_normals;
            std::vector<unsigned int> indices;
            float vertex[3], normal[3];
            unsigned int index[3][3],f=0;
            FILE* objfile = fopen(path, "r");
            if (objfile == NULL) {
                printf("Impossible to open the file !\n");
                return false;
            }
            while (fscanf(objfile, "%s", lineHeader) != EOF) {
                if (strcmp(lineHeader, "v") == 0) {
                    fscanf(objfile, "%f %f %f\n", &vertex[0], &vertex[1], &vertex[2]);
                    temp_vertices.push_back(vertex[0]);
                    temp_vertices.push_back(vertex[1]);
                    temp_vertices.push_back(vertex[2]);
                }
                else if (strcmp(lineHeader, "vn") == 0) {
                    fscanf(objfile, "%f %f %f\n", &normal[0], &normal[1], &normal[2]);
                    temp_normals.push_back(normal[0]);
                    temp_normals.push_back(normal[1]);
                    temp_normals.push_back(normal[2]);
                }
                else if (strcmp(lineHeader, "f") == 0) {
                    int matches = fscanf(objfile, "%u//%u %u//%u %u//%u",
                            &index[0][0], &index[2][0], &index[0][1],&index[2][1], &index[0][2], &index[2][2]);
                    printf("matches=%d,f=%u\n",matches,f++);
                    if (matches != 6) {
                        printf("File can't be read by our simple parser ( Try exporting with other options\n");
                        return false;
                    }
                    indices.push_back(index[0][0]);
                    indices.push_back(index[2][0]);
                    indices.push_back(index[0][1]);
                    indices.push_back(index[2][1]);
                    indices.push_back(index[0][2]);
                    indices.push_back(index[2][2]);
                }
            }
            for (unsigned int i = 0; i < indices.size(); i+=2) {
                vertices->push_back( Point(
                            temp_vertices[(indices[i] - 1) * 3], 
                            temp_vertices[(indices[i] - 1) * 3 + 1], 
                            temp_vertices[(indices[i] - 1) * 3 + 2]));

                normals->push_back( Point(
                            temp_normals[(indices[i + 1] - 1) * 3], 
                            temp_normals[(indices[i + 1] - 1) * 3 + 1], 
                            temp_normals[(indices[i + 1] - 1) * 3 + 2]));
            }
        }
};
#endif
