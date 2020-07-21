#include "GLTools.h"
#include "GLShaderManager.h"
#include "GLFrustum.h"
#include "GLBatch.h"
#include "GLMatrixStack.h"
#include "GLGeometryTransform.h"
#include "StopWatch.h"

#include <math.h>
#include <stdio.h>

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif


GLShaderManager        shaderManager;            // 着色器管理器
GLMatrixStack        modelViewMatrix;        // 模型视图矩阵堆栈
GLMatrixStack        projectionMatrix;        // 投影矩阵堆栈
GLFrustum            viewFrustum;            // 视景体
GLGeometryTransform    transformPipeline;        // 几何图形变换管道

GLTriangleBatch        torusBatch;             //大球
GLTriangleBatch     sphereBatch;            //小球
GLBatch             floorBatch;          //地板

//角色帧 照相机角色帧
GLFrame   cameraFrame;
GLFrame  objectFrame;

//**4、添加附加随机球
#define NUM_SPHERES 50
GLFrame spheres[NUM_SPHERES];

void SetupRC() {
    
}

void RenderScene() {
    
}

//屏幕更改大小或已初始化
void ChangeSize(int nWidth, int nHeight) {
    
}

void SpeacialKeys(int key, int x, int y) {
    
}

int main(int argc, char* argv[]) {
    gltSetWorkingDirectory(argv[0]);
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800,600);
    
    glutCreateWindow("OpenGL SphereWorld");
    
    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);
    glutSpecialFunc(SpeacialKeys);
    
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return 1;
    }
    
    
    SetupRC();
    glutMainLoop();
    
    return 0;
}
