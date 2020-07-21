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
    
    //1.初始化
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    shaderManager.InitializeStockShaders();
    
    //2.开启深度测试（球旋转）
    glEnable(GL_DEPTH_TEST);
    
    //3.地板数据
    floorBatch.Begin(GL_LINES, 324);
    for (GLfloat x = -20.0f; x <= 20.0f; x += 0.5f) {
        floorBatch.Vertex3f(x, -0.55f, 20.0f);
        floorBatch.Vertex3f(x, -0.55f, -20.0f);
        
        floorBatch.Vertex3f(20.0f, -0.55f, x);
        floorBatch.Vertex3f(-20.0f, -0.55f, x);
    }
    floorBatch.End();
    
    //4.大球数据(基于gltools模型)
    gltMakeSphere(torusBatch, 0.4f, 40, 80);
    
    //5. 绘制小球
    gltMakeSphere(sphereBatch, 0.2f, 13, 26);
    
    //6. 随机位置放置小球球
    for (int i =0; i < NUM_SPHERES; i++) {
        
        //y轴不变，X,Z产生随机值
        GLfloat x = ((GLfloat)((rand() % 400) - 200 ) * 0.1f);
        GLfloat z = ((GLfloat)((rand() % 400) - 200 ) * 0.1f);
        
        //在y方向，将球体设置为0.0的位置，这使得它们看起来是飘浮在眼睛的高度
        //对spheres数组中的每一个顶点，设置顶点数据
        spheres[i].SetOrigin(x, 0.0f,z);
    }
    
}

void RenderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //1. 颜色(地板,大球颜色,小球颜色)
    static GLfloat vFloorColor[] = {0.0f,1.0f,0.0f,1.0f};
    static GLfloat vTorusColor[] = {1.0f,0.0f,0.0f,1.0f};
    static GLfloat vSpereColor[] = {0.0f,0.0f,1.0f,1.0f};
    static GLfloat vTranlateColor[] = {0.5f,0.5f,0.5f,1.0f};
    
    //2.动画
    static CStopWatch rotTimer;
    float yRot = rotTimer.GetElapsedSeconds() * 60.0f;
    
    modelViewMatrix.PushMatrix();
    
    M3DMatrix44f mCamera;
    cameraFrame.GetCameraMatrix(mCamera);
    modelViewMatrix.PushMatrix(mCamera);
    
    shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vFloorColor);
    floorBatch.Draw();
    
    //点光源的位置
    M3DVector4f vLightPos = {0,10,5,1};
    
    //大球往里平移3.0
    modelViewMatrix.Translate(0, 0, -3.0f);
    
    //大球
    modelViewMatrix.PushMatrix();
    modelViewMatrix.Rotate(yRot, 0, 1, 0);
    shaderManager.UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF, transformPipeline.GetModelViewMatrix(), transformPipeline.GetProjectionMatrix(), vLightPos, vTorusColor);
    torusBatch.Draw();
    modelViewMatrix.PopMatrix();
    
    //小球
    for (int i = 0; i < NUM_SPHERES; i++) {
        modelViewMatrix.PushMatrix();
        modelViewMatrix.MultMatrix(spheres[i]);
        shaderManager.UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF, transformPipeline.GetModelViewMatrix(), transformPipeline.GetProjectionMatrix(), vLightPos, vSpereColor);
        sphereBatch.Draw();
        modelViewMatrix.PopMatrix();
    }
    
    // 让一个小球围着大球公转;
    modelViewMatrix.Rotate(yRot * -2, 0, 1, 0);
    modelViewMatrix.Translate(0.8f, 0, 0);
    shaderManager.UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF, transformPipeline.GetModelViewMatrix(), transformPipeline.GetProjectionMatrix(), vLightPos, vTranlateColor);
    sphereBatch.Draw();
    
    modelViewMatrix.PopMatrix();
    modelViewMatrix.PopMatrix();
    
    glutSwapBuffers();
    glutPostRedisplay();
}

//屏幕更改大小或已初始化
void ChangeSize(int nWidth, int nHeight) {
    
    //1. 设置视口
    glViewport(0, 0, nWidth, nHeight);
    
    //2. 设置投影方式，创建投影矩阵，投影矩阵堆栈
    viewFrustum.SetPerspective(35.0f, float(nWidth)/float(nHeight), 1.0f, 500.0f);
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    
    //3. 变换管道设置2个矩阵堆栈(管理)
    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
}

void SpeacialKeys(int key, int x, int y) {
    
    float linear = 0.1f;
    float angular = float(m3dDegToRad(5.0f));
    
    //往里面移动
    if (key == GLUT_KEY_UP) {
        cameraFrame.MoveForward(linear);
    }
    
    //往外面移动
    if (key == GLUT_KEY_DOWN) {
        cameraFrame.MoveForward(-linear);
    }
    
    //逆向旋转
    if (key == GLUT_KEY_LEFT) {
        cameraFrame.RotateWorld(angular, 0, 1, 0);
    }
    //顺向旋转
    if (key == GLUT_KEY_RIGHT) {
        cameraFrame.RotateWorld(-angular, 0, 1, 0);
    }
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
