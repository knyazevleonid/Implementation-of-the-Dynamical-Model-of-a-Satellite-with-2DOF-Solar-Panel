#include "mainwindow.h"

#include <QApplication>
#include <QMessageBox>
#include <QMouseEvent>

MainWindow::MainWindow(QWidget *parent) :
    QOpenGLWidget(parent),
    m_texture(nullptr),
    m_vertexBuffer(QOpenGLBuffer::VertexBuffer),
    m_indexBuffer(QOpenGLBuffer::IndexBuffer)
{

}

MainWindow::~MainWindow()
{

}

void MainWindow::initializeGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    initShaders();
    initCube(1.0f, 0.05);
}

void MainWindow::resizeGL(int w, int h)
{
    float aspect = w / qreal(h ? h : 1);

    m_projectionMatrix.setToIdentity();
    m_projectionMatrix.perspective(45, aspect, 0.1f, 10.0f);
}

void MainWindow::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 modelViewMatrix;
    modelViewMatrix.setToIdentity();
    modelViewMatrix.translate(0.0f, 0.0f, -5.0f);
    modelViewMatrix.rotate(m_rotation);

    m_texture->bind(0);

    QMatrix4x4 modelMatrix;
    modelMatrix.setToIdentity();

    m_shaderProgramm.bind();//
    m_shaderProgramm.setUniformValue("u_projectionMatrix", m_projectionMatrix);
    m_shaderProgramm.setUniformValue("u_viewMatrix", modelViewMatrix);
    m_shaderProgramm.setUniformValue("u_modelMatrix", modelMatrix);
    m_shaderProgramm.setUniformValue("u_texture", 0);
    m_shaderProgramm.setUniformValue("u_lightPosition", QVector4D(0.0, 1000.0, 0.0, 1.0));
    m_shaderProgramm.setUniformValue("u_lightPower", 5.0f);

    // Bind buffers
    m_vertexBuffer.bind();
    m_indexBuffer.bind();

    int offset = 0;

    int location = m_shaderProgramm.attributeLocation("a_position");
    m_shaderProgramm.enableAttributeArray(location);
    m_shaderProgramm.setAttributeBuffer(location, GL_FLOAT, offset, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);

    location = m_shaderProgramm.attributeLocation("a_textcoord0");
    m_shaderProgramm.enableAttributeArray(location);
    m_shaderProgramm.setAttributeBuffer(location, GL_FLOAT, offset, 2, sizeof(VertexData));

    offset += sizeof(QVector2D);

    location = m_shaderProgramm.attributeLocation("a_normal");
    m_shaderProgramm.enableAttributeArray(location);
    m_shaderProgramm.setAttributeBuffer(location, GL_FLOAT, offset, 3, sizeof(VertexData));





    glDrawElements(GL_TRIANGLES, m_indexBuffer.size(), GL_UNSIGNED_INT, 0);

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug() << "In close event\n";

    QMessageBox::StandardButton ret;
    ret = QMessageBox::question( this,  QApplication::applicationName(), tr(" Do you want to close programm ? "),
                                 QMessageBox::Yes | QMessageBox::No , QMessageBox::No );
    if (ret == QMessageBox::No)
        event->ignore();
    else
        close();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton) {
        m_mousePosition = QVector2D(event->localPos());
    }

    event->accept();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{


    if (event->buttons() != Qt::LeftButton) return;

    QVector2D diff = QVector2D(event->localPos()) - m_mousePosition;
    m_mousePosition = QVector2D(event->localPos());

    float angle = diff.length() / 2.0f;

    QVector3D axis = QVector3D(diff.y(), diff.x(), 0.0f);

    m_rotation = QQuaternion::fromAxisAndAngle(axis, angle) * m_rotation;

    update();
}

void MainWindow::initShaders()
{
    if (!m_shaderProgramm.addShaderFromSourceFile(QOpenGLShader::Vertex, "://vshader.vsh"))
        close();

    if (!m_shaderProgramm.addShaderFromSourceFile(QOpenGLShader::Fragment, "://fshader.fsh"))
        close();

    if (!m_shaderProgramm.link())
        close();

    if (!m_shaderProgramm.bind())
        close();

}

void MainWindow::initCube(float width, float higth)
{
    float width_div_2 = width / 2;
    float h = higth / 2;
    QVector<VertexData> vertexes;


    //front, right, top, back, left, down

    // Top side
    vertexes.append(VertexData(QVector3D(width_div_2, h, width_div_2), QVector2D(0.0f, 1.0f), QVector3D(0.0f, 1.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, h, -width_div_2), QVector2D(0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, h, width_div_2), QVector2D(1.0f, 1.0f), QVector3D(0.0f, 1.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, h, -width_div_2), QVector2D(1.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f)));


    // Down side
    vertexes.append(VertexData(QVector3D(-width_div_2, -h, width_div_2), QVector2D(0.0f, 1.0f), QVector3D(0.0f, -1.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, -h, -width_div_2), QVector2D(0.0f, 0.0f), QVector3D(0.0f, -1.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, -h, width_div_2), QVector2D(1.0f, 1.0f), QVector3D(0.0f, -1.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, -h, -width_div_2), QVector2D(1.0f, 0.0f), QVector3D(0.0f, -1.0f, 0.0f)));

    // Left side
    /*
    vertexes.append(VertexData(QVector3D(-width_div_2, width_div_2, -width_div_2), QVector2D(0.0f, 1.0f), QVector3D(-1.0f, 0.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, -width_div_2, -width_div_2), QVector2D(0.0f, 0.0f), QVector3D(-1.0f, 0.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, width_div_2, width_div_2), QVector2D(1.0f, 1.0f), QVector3D(-1.0f, 0.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, -width_div_2, width_div_2), QVector2D(1.0f, 0.0f), QVector3D(-1.0f, 0.0f, 0.0f)));
*/
    vertexes.append(VertexData(QVector3D(-width_div_2, h, width_div_2), QVector2D(0.0f, 1.0f), QVector3D(-1.0f, 0.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, h, -width_div_2), QVector2D(0.0f, 0.0f), QVector3D(-1.0f, 0.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, -h, width_div_2), QVector2D(1.0f, 1.0f), QVector3D(-1.0f, 0.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, -h, -width_div_2), QVector2D(1.0f, 0.0f), QVector3D(-1.0f, 0.0f, 0.0f)));

    // Right side
    vertexes.append(VertexData(QVector3D(width_div_2, h, width_div_2), QVector2D(0.0f, 1.0f), QVector3D(1.0f, 0.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, -h, width_div_2), QVector2D(0.0f, 0.0f), QVector3D(1.0f, 0.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, h, -width_div_2), QVector2D(1.0f, 1.0f), QVector3D(1.0f, 0.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, -h, -width_div_2), QVector2D(1.0f, 0.0f), QVector3D(1.0f, 0.0f, 0.0f)));

    // Front side
    vertexes.append(VertexData(QVector3D(-width_div_2, h, width_div_2), QVector2D(0.0f, 1.0f), QVector3D(0.0f, 0.0f, 1.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, -h, width_div_2), QVector2D(0.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, h, width_div_2), QVector2D(1.0f, 1.0f), QVector3D(0.0f, 0.0f, 1.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, -h, width_div_2), QVector2D(1.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f)));

    // Back side
    vertexes.append(VertexData(QVector3D(width_div_2, h, -width_div_2), QVector2D(0.0f, 1.0f), QVector3D(0.0f, 0.0f, -1.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, -h, -width_div_2), QVector2D(0.0f, 0.0f), QVector3D(0.0f, 0.0f, -1.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, h, -width_div_2), QVector2D(1.0f, 1.0f), QVector3D(0.0f, 0.0f, -1.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, -h, -width_div_2), QVector2D(1.0f, 0.0f), QVector3D(0.0f, 0.0f, -1.0f)));


    QVector<GLuint> indexes;
    for (int i = 0; i < 24; i += 4){
        indexes.append(i + 0);
        indexes.append(i + 1);
        indexes.append(i + 2);
        indexes.append(i + 2);
        indexes.append(i + 1);
        indexes.append(i + 3);
    }

    // Create and fill vertex buffer and then -> release
    m_vertexBuffer.create();
    m_vertexBuffer.bind();
    m_vertexBuffer.allocate(vertexes.constData(), vertexes.size() * sizeof(VertexData));
    m_vertexBuffer.release();

    // Create and fill index buffer and then -> release
    m_indexBuffer.create();
    m_indexBuffer.bind();
    m_indexBuffer.allocate(indexes.constData(), indexes.size() * sizeof(GLuint));
    m_indexBuffer.release();

    m_texture = new QOpenGLTexture(QImage("://cube.png").mirrored());
    m_texture->setMinificationFilter(QOpenGLTexture::Nearest);
    m_texture->setMagnificationFilter(QOpenGLTexture::Linear);
    m_texture->setWrapMode(QOpenGLTexture::Repeat);
}


//Lyonya



mtl::dense2D<double> I2(3,3);

mtl::dense2D<double> I1(3,3);

mtl::dense_vector <double> a2_(3);

mtl::dense_vector <double> e1(3);

mtl::dense_vector<double> f3(double *y);

mtl::dense_vector <double> a1(3);

void ff(double x, double * y, double * f, int flag);

#define M 1
#define R 1
using namespace std;
/*
double fw1(double x, double * y);
double fw2(double x, double * y);
double fw3(double x, double * y);
double fq0(double x, double * y);
double fq1(double x, double * y);
double fq2(double x, double * y);
double fq3(double x, double * y);
double fpsi1(double x, double * y);
double fpsi2(double x, double * y);
*/
/*
double f(int i, double x, double * y){
    switch (i){
        case 0: return y[3];break;
        case 1: return y[4];break;
        case 2: return y[5];break;
        case 3: return (-M*y[0])/pow(pow(y[0],2)+pow(y[1],2)+pow(y[2],2),3);break;
        case 4: return (-M*y[1])/pow(pow(y[0],2)+pow(y[1],2)+pow(y[2],2),3);break;
        case 5: return (-M*y[2])/pow(pow(y[0],2)+pow(y[1],2)+pow(y[2],2),3);break;
        default : break;
    }
}
*/
/*

double ff(int i, double x, double * y){
        switch (i){
                case 0: return fw1(x, y);break;
                case 1: return fw2(x, y);break;
                case 2: return fw3(x, y);break;
        case 3: return fq0(x, y);break;
                case 4: return fq1(x, y);break;
                case 5: return fq2(x, y);break;
        case 6: return fq3(x, y);break;
                case 7: return y[9];break;
                case 8: return y[10];break;
                case 9: return fpsi1(x, y);break;
                case 10: return fpsi2(x, y);break;
                default : break;
        }
}
*/

/*************************************************************************
  Один шаг метода Рунге-Кутта четвертого порядка для решения
  системы дифферециальных уравнений.

  procedure SystemRungeKuttStep(
  const   x   :   Real;
  const   h   :   Real;
  const   n   :   Integer;
  var     y   :   array of Real);

  Алгоритм совершает один шаг метода для системы
  диффуров y[i]'=F(i,x,y) для i=1..n

  Начальная точка имеет кординаты (x,y[1], ..., y[n])

  После выполнения алгоритма в переменной y содержится состояние
  системы в точке x+h
 *************************************************************************/
void step(int n,double x,double h,double * y, int flag)
{
    int i=0;
    double * yt = new double[n];
    double * k1 = new double[n];
    double * k2 = new double[n];
    double * k3 = new double[n];
    double * k4 = new double[n];

    double * f  = new double[n];

    ff(x,y,f,flag);

    for(i = 0; i < n; i++)
    {
//		k1[i] = h*f(i, x, y);
        k1[i] = h*f[i];
    }
    for(i = 0; i < n; i++)
    {
        yt[i] = y[i]+0.5*k1[i];
    }

    ff(x+h*0.5, yt,f,flag);

    for(i = 0; i < n; i++)
    {
//		k2[i] = h*f(i, x+h*0.5, yt);
        k2[i] = h*f[i];
    }

    for(i = 0; i < n; i++)
    {
        yt[i] = y[i]+0.5*k2[i];
    }

    ff(x+h*0.5, yt,f,flag);

    for(i = 0; i < n; i++)
    {
//		k3[i] = h*f(i, x+h*0.5, yt);
        k3[i] = h*f[i];
    }
    for(i = 0; i < n; i++)
    {
        yt[i] = y[i]+k3[i];
    }

    ff(x+h, yt,f,flag);

    for(i = 0; i < n; i++)
    {
//		k4[i] = h*f(i, x+h, yt);
        k4[i] = h*f[i];
    }
    for(i = 0; i < n; i++)
    {
        y[i] = y[i]+(k1[i]+2.0*k2[i]+2.0*k3[i]+k4[i])/6;
    }
    if (flag == 1)
    {
        double modul = sqrt(y[3]*y[3]+y[4]*y[4]+y[5]*y[5]+y[6]*y[6]);
        if (modul != 0)
        {
            for(i = 3; i < 7; i++)
            {
                y[i] = y[i]/modul;
            }
        }
    }

    delete[] yt;
    delete[] k1;
    delete[] k2;
    delete[] k3;
    delete[] k4;
    delete[] f;

}
/*************************************************************************
  Алгоритм решает систему диффуров y[i]'=F(i,x,y) для i=1..n
  методом Рунге-Кутта 4 порядка.

  Начальная точка имеет кординаты (x,y[1], ..., y[n])

  До конечной точки мы добираемся через n промежуточных
  с постоянным шагом h=(x1-x)/m

  Результат помещается в переменную result[4]
 *************************************************************************/
void solvesystemrungekutta(int n,double x,double x1,int steps,double * result, int flag){

    for(int i = 0; i < steps; i++)
    {
        step(n, x+i*(x1-x)/steps, (x1-x)/steps, result, flag);
    }
}

double dist_pl(double sunvec[3], double result[6]){
    return sqrt(pow((sunvec[1]*result[0]-sunvec[0]*result[1]),2)+pow((sunvec[2]*result[1]-sunvec[1]*result[2]),2)+pow((sunvec[0]*result[2]-sunvec[2]*result[0]),2))/sqrt(sunvec[2]*sunvec[2]+sunvec[1]*sunvec[1]+sunvec[0]*sunvec[0]);
}

mtl::dense2D<double> K(mtl::dense_vector <double> a, mtl::dense_vector <double> b)
{
     using namespace mtl;
     dense2D<double> A(3, 3);
     A(0,0)= a[1]*b[1]+a[2]*b[2];
     A(0,1)= - a[1]*b[0];
     A(0,2)= - a[2]*b[0];

     A(1,0)= - a[0]*b[1];
     A(1,1)= a[0]*b[0]+a[2]*b[2];
     A(1,2)= - a[2]*b[1];

     A(2,0)= - a[0]*b[2];
     A(2,1)= - a[1]*b[2];
     A(2,2)= a[0]*b[0]+a[1]*b[1];

     return A;
}

mtl::dense2D<double> OMEGA(double *y)
{
    using namespace mtl;
    dense2D<double> A(4, 4);
    A(0,0)= 0;
    A(0,1)= y[2];
    A(0,2)= -y[1];
    A(0,3)= y[0];

    A(1,0)= -y[2];
    A(1,1)= 0;
    A(1,2)= y[0];
    A(1,3)= y[1];

    A(2,0)= y[1];
    A(2,1)= -y[0];
    A(2,2)= 0;
    A(2,3)= y[2];

    A(3,0)= -y[0];
    A(3,1)= -y[1];
    A(3,2)= -y[2];
    A(3,3)= 0;
    return A;
}

mtl::dense_vector<double> q(double *y)
{
    using namespace mtl;
    dense_vector <double> v(4);

    v=0.0;

    int i;
    for (i=0;i<4;i++)
    {
        v(i)=y[i+3];
    }

    return v;
}

mtl::dense_vector<double> dq(double *y)
{
    using namespace mtl;
    dense_vector <double> v(4);

    v=0.0;

    v=0.5*OMEGA(y)*q(y);

    return v;
}

mtl::dense2D<double> B3(double *y)
{
    using namespace mtl;
    dense2D<double> A(3, 3);

    A(0,0)= 1.0;
    A(0,1)= 0.0;
    A(0,2)= 0.0;

    A(1,0)= 0.0;
    A(1,1)= cos(y[8]);
    A(1,2)= sin(y[8]);

    A(2,0)= 0.0;
    A(2,1)= -sin(y[8]);
    A(2,2)= cos(y[8]);

    return A;
}

mtl::dense2D<double> B1(double *y)
{
    using namespace mtl;
    dense2D<double> A(3, 3);

    A(0,0)= cos(y[7]);
    A(0,1)= 0.0;
    A(0,2)= -sin(y[7]);

    A(1,0)= 0.0;
    A(1,1)= 1.0;
    A(1,2)= 0.0;

    A(2,0)= sin(y[7]);
    A(2,1)= 0.0;
    A(2,2)= cos(y[7]);

    return A;
}

mtl::dense2D<double> J2(double *y)
{
    using namespace mtl;
    dense2D<double> A(3, 3);

    A=0.0;

    A=trans(B1(y))*trans(B3(y)) * I2 * B3(y) * B1(y);

    return A;
}

mtl::dense_vector<double> alpha2(double *y)
{
    using namespace mtl;
    dense_vector <double> v(3);

    v=0.0;

    v=dense_vector <double>(trans(B1(y)) * dense_vector <double>(trans(B3(y))*a2_));

    return v;
}

mtl::dense_vector<double> omega1(double *y)
{
    using namespace mtl;
    dense_vector <double> v(3);

    v=0.0;

    int i;
    for (i=0;i<3;i++)
    {
        v(i)=y[i];
    }

    return v;
}

mtl::dense_vector<double> f1(double *y)
{
    using namespace mtl;
    dense_vector <double> v(3);

    v=0.0;

    dense_vector <double> v1(3);
    v1= I1*omega1(y);

    v=cross(omega1(y), v1);

    return v;
}

mtl::dense_vector<double> e3(double *y)
{
    using namespace mtl;
    dense_vector <double> v(3);

    v=0.0;

    dense_vector <double> v1(3);

    v1=0.0;
    v1(0)=1.0;

    v=trans(B3(y))*v1;

    return v;
}

mtl::dense_vector<double> w2(double *y)
{
    using namespace mtl;
    dense_vector <double> v(3);

    v=0.0;
    v=omega1(y) + e1 * y[9] + dense_vector <double>(trans(B1(y)) * dense_vector <double>(e3(y) * y[10]));

    return v;
}

mtl::dense_vector<double> f2(double *y)
{
    using namespace mtl;
    dense_vector <double> v(3);

    v=0.0;

    dense_vector <double> v1(3);
    v1= J2(y)*w2(y);

    v=cross(w2(y), v1) + J2(y)*f3(y);

    return v;
}

mtl::dense_vector<double> f3(double *y)
{
    using namespace mtl;
    dense_vector <double> v(3);

    v=0.0;

    dense_vector <double> v1(3);

    v1=0.0;
    v1=e1*y[9];

    dense_vector <double> v2(3);

    v2=0.0;
    v2=omega1(y)+e1*y[9];

    dense_vector <double> v3(3);

    v3=0.0;
    v3=dense_vector <double>(trans(B1(y)) * dense_vector <double>(e3(y)*y[10]));

    v=cross(omega1(y),v1) + cross(v2, v3);

    return v;
}

mtl::dense_vector<double> f4(double *y)
{
    using namespace mtl;
    dense_vector <double> v(3);

    v=0.0;

    dense_vector <double> v1(3);

    v1=0.0;
    v1=cross(omega1(y), a1);

    dense_vector <double> v2(3);

    v2=0.0;
    v2=cross(w2(y), alpha2(y));

    v=cross(f3(y), alpha2(y))+ cross(omega1(y),v1) + cross(w2(y), v2);

    return v;
}

mtl::dense_vector <double> v(double *y)
{
    using namespace mtl;
    dense_vector <double> v(5);

    v=0.0;

    dense_vector <double> v1(3);

    v1=0.0;
    v1=-f1(y)-f2(y)-cross(a1, f4(y))-cross(alpha2(y), f4(y));

    double v2=-dot(f2(y), e1)-dot(cross(alpha2(y), f4(y)), e1);

    double v3=-dot(f2(y), e3(y))-dot(cross(alpha2(y), f4(y)), dense_vector <double> (trans(B1(y))*e3(y)));

    int i;
    for(i=0;i<3;i++)
    {
        v(i)=v1(i);
    }

    v(3)=v2;
    v(4)=v3;

    return v;
}

mtl::dense2D<double> S(double *y)
{
    using namespace mtl;
    dense2D<double> A(5, 5);

    A=0.0;

    dense2D<double> B(3, 3);
    B=0.0;

    B=I1+J2(y)+K(dense_vector<double>(a1+alpha2(y)),dense_vector<double>(a1+alpha2(y)));

    dense_vector<double> v1(3);
    v1=(dense2D <double>(J2(y)+K(dense_vector<double>(a1+alpha2(y)),alpha2(y))))*e1;

    dense_vector<double> v2(3);
    v2=(dense2D <double>(J2(y)+K(dense_vector<double>(a1+alpha2(y)),alpha2(y))))*dense_vector <double>(trans(B1(y))*e3(y));

    dense_vector<double> v3(3);
    v3=trans((dense2D <double>(J2(y)+K(alpha2(y), dense_vector<double>(a1+alpha2(y))))))*e1;

    dense_vector<double> v4(3);
    v4=trans((dense2D <double>(J2(y)+K(alpha2(y), dense_vector<double>(a1+alpha2(y))))))*dense_vector <double>(trans(B1(y))*e3(y));

    double v5;

    v5=dot(e1, dense_vector <double>(J2(y)*e1)) + pow(two_norm(cross(e1, alpha2(y))), 2);

    double v6;

    v6=dot(e1, dense_vector <double>(J2(y)*dense_vector <double>(trans(B1(y))*e3(y)))) - dot(e1, alpha2(y)) * dot (dense_vector <double>(trans(B1(y))*e3(y)), alpha2(y));

    double v7;

    v7=dot(dense_vector <double>(J2(y)*e1), dense_vector <double>(trans(B1(y))*e3(y))) - dot(e1, alpha2(y)) * dot (dense_vector <double>(trans(B1(y))*e3(y)), alpha2(y));

    double v8;

    v8=dot(dense_vector <double>(trans(B1(y))*e3(y)), dense_vector <double>(J2(y)*dense_vector <double>(trans(B1(y))*e3(y)))) + pow(two_norm(cross(dense_vector <double>(trans(B1(y))*e3(y)), alpha2(y))), 2);

    int i, j;

    for (i=0;i<3;i++)
    {
        for (j=0;j<3;j++)
            A(i,j)=B(i,j);
    }

    for (i=0;i<3;i++)
    {
        A(3,i)=v3(i);
        A(4,i)=v4(i);
        A(i,3)=v1(i);
        A(i,4)=v2(i);
    }

    A(3,3) = v5;
    A(3,4) = v6;
    A(4,3) = v7;
    A(4,4) = v8;

    return A;
}


void ff(double x, double * y, double * f, int flag)
{
    if (flag == 1)
    {
        mtl::dense_vector <double> omegapsi(5);
        omegapsi = mtl::mat::inv(S(y))*v(y);

        int i;
        for (i=0;i<3;i++)
        {
            f[i]=omegapsi(i);
        }

        for (i=3;i<7;i++)
        {
            f[i]=dq(y)[i-3];
        }

        f[7] = y[9];
        f[8] = y[10];

        f[9] = omegapsi(3);
        f[10] = omegapsi(4);
        return;
    }

    int i;
    for (i=0;i<3;i++)
    {
        f[i] = y[i+3];
        f[i+3] = (-M*y[i])/pow(pow(y[0],2)+pow(y[1],2)+pow(y[2],2),3);
    }
    return;
}

mtl::dense2D<double> Qmatrix(mtl::dense_vector <double> q)
{
    mtl::dense2D<double> B(3, 3);

    B(0,0)=1.0 - 2.0 * q[2]*q[2] - 2.0 * q[3] * q[3];
    B(0,1)=2.0 * (q[1]*q[2]+q[0]*q[3]);
    B(0,2)=2.0 * (q[1]*q[3]-q[0]*q[2]);

    B(1,0)=2.0 * (q[1]*q[2]-q[0]*q[3]);
    B(1,1)=1.0 - 2.0 * q[1]*q[1] - 2.0 * q[3] * q[3];
    B(1,2)=2.0 * (q[2]*q[3]+q[0]*q[1]);

    B(2,0)=2.0 * (q[1]*q[3]+q[0]*q[2]);
    B(2,1)=2.0 * (q[2]*q[3]-q[0]*q[1]);
    B(2,2)=1.0 - 2.0 * q[1]*q[1] - 2.0 * q[2] * q[2];

    return B;
}

int main(int, char**)
{
    using namespace mtl;

//    dense2D<double> I2(3,3);

//    dense2D<double> I1(3,3);

    I1 = 0.0;
    I1(0,0)= 38.57;
    I1(1,1)= 29.05;
    I1(2,2)= 33.96;

    I2 = 0.0;
    I2(0,0)= 5.549;
    I2(1,1)= 1.757;
    I2(2,2)= 7.304;

    a2_=0.0;
    a2_(1)=1.175;

    e1 = 0.0;
    e1(1) = 1;

/*
    a1(0)=0.0094;
    a1(1)=-0.4489;
    a1(2)=-0.1268;
*/

    a1(0)=0.0;
    a1(1)=0.5;
    a1(2)=0.0;

/**************************************************************************/
/*
    dense_vector <double> a(3);
    dense_vector <double> b(3);
    a(0)=1.0;
    a(1)=0.0;
    a(2)=0.0;
    b(0)=0.0;
    b(1)=1.0;
    b(2)=0.0;

    dense2D <double> A(2,2);
    A = 0.0;
    A(0,0)=1.0;
    A(1,1)=1.0;

    std::cout << "B is \n" << mat::inv(A) << "\n";
*/

    double y[11];
    double result[6];
    double sunvec[3];

    //Задаём направление на Солнце
    sunvec[0] = 0;
    sunvec[1] = -1;
    sunvec[2] = 0;

    result[0] = 0;
    result[1] = 1;
    result[2] = 0;
    result[3] = 1;
    result[4] = 0;
    result[5] = 0;

    solvesystemrungekutta(6,0,3.1415,10000,result, 0);
    for(int i = 0; i < 6; i++){
        cout<<result[i]<<endl;
    }

    if((dist_pl(sunvec, result) < R)&&((sunvec[0]*result[0]+sunvec[1]*result[1]+sunvec[2]*result[2]) > 0))
        cout<<"SHADOW"<<endl;
    else
        cout<<"BRIGHT SIDE"<<endl;

    y[0]=0.001;
    y[1]=0.001;
    y[2]=0.001;
    y[3]=1.0;
    y[4]=0.0;
    y[5]=0.0;
    y[6]=0.0;

    y[7]=0.0;
    y[8]=0.0;

    y[9]=0.001;
    y[10]=0.001;

    int j;

    for (j=0;j<10;j++)
    {
        solvesystemrungekutta(11,0,100,100,y, 1);
        for(int i = 0; i < 11; i++){
            cout<<y[i]<<endl;
        }
        cout<<endl;
        cout<<Qmatrix(q(y))<<endl;
    }
    return 0;
}
































