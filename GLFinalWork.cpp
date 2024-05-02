#include "stdafx.h"
#include "GLFinalWork.h"

GLFinalWork::GLFinalWork(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    this->centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    layout = new QHBoxLayout(centralWidget);
    leftFrame = new QFrame; rightFrame = new QFrame;

    leftWindow = new My3DWindow(); rightWindow = new My3DWindow();

    containerLeft = createWindowContainer(leftWindow, leftFrame);
    containerRight = createWindowContainer(rightWindow, rightFrame);

    layout->addWidget(containerLeft);
    layout->addWidget(containerRight);

    // Left window setup
    rootEntityLeft = new Qt3DCore::QEntity;
    leftWindow->setRootEntity(rootEntityLeft);

    cameraLeft = leftWindow->camera();
    cameraLeft->lens()->setPerspectiveProjection(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
    cameraLeft->setPosition(QVector3D(0, 0, 2));
    cameraLeft->setViewCenter(QVector3D(0, 0, 0));

    modelEntityLeft = new Qt3DCore::QEntity(rootEntityLeft);
    meshLeft = new Qt3DRender::QMesh;
    meshLeft->setSource(QUrl::fromLocalFile("D:\\Documents\\source\\repos\\GLFinalWork\\Noisy.obj"));
    modelEntityLeft->addComponent(meshLeft);
    materialLeft = new Qt3DExtras::QPhongMaterial;
    modelEntityLeft->addComponent(materialLeft);

    // Right window setup (similar to left window)
    rootEntityRight = new Qt3DCore::QEntity;
    rightWindow->setRootEntity(rootEntityRight);

    cameraRight = rightWindow->camera();
    cameraRight->lens()->setPerspectiveProjection(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
    cameraRight->setPosition(QVector3D(0, 0, 2));
    cameraRight->setViewCenter(QVector3D(0, 0, 0));

    modelEntityRight = new Qt3DCore::QEntity(rootEntityRight);
    //meshRight = new Qt3DRender::QMesh;
    //meshRight->setSource(QUrl::fromLocalFile("path/to/your/mesh.obj"));
    Qt3DExtras::QCuboidMesh* meshRight = new Qt3DExtras::QCuboidMesh;
    modelEntityRight->addComponent(meshRight);
    materialRight = new Qt3DExtras::QPhongMaterial;
    modelEntityRight->addComponent(materialRight);
    
    QObject::connect(leftWindow, &My3DWindow::wheelScrolled, this, &GLFinalWork::onWheelScrolled);
    QObject::connect(rightWindow, &My3DWindow::wheelScrolled, this, &GLFinalWork::onWheelScrolled);

    QObject::connect(leftWindow, &My3DWindow::mouseMoved, this, &GLFinalWork::onMouseMoved);
    QObject::connect(rightWindow, &My3DWindow::mouseMoved, this, &GLFinalWork::onMouseMoved);

    QObject::connect(leftWindow, &My3DWindow::mousePressed, this, &GLFinalWork::onMousePressed);
    QObject::connect(rightWindow, &My3DWindow::mousePressed, this, &GLFinalWork::onMousePressed);

    QObject::connect(leftWindow, &My3DWindow::mouseReleased, this, &GLFinalWork::onMouseReleased);
    QObject::connect(rightWindow, &My3DWindow::mouseReleased, this, &GLFinalWork::onMouseReleased);
}

GLFinalWork::~GLFinalWork()
{
    delete this->centralWidget;
}

void GLFinalWork::onWheelScrolled(QPoint delta) {
    My3DWindow* window = dynamic_cast<My3DWindow*>(sender());
    if (window == nullptr) {
        qDebug() << "不支持的发送方" << '\n';
        return;
    }

    if (window == leftWindow) {
        auto position = cameraLeft->position();
        auto viewCenter = cameraLeft->viewCenter();
        if (delta.y() < 0) {
            auto ans = position + (position - viewCenter) / 10.0;
            cameraLeft->setPosition(ans);
        }
        else {
            auto ans = position - (position - viewCenter) / 10.0;
            cameraLeft->setPosition(ans);
        }
        return;
    }
    if (window == rightWindow) {
        auto position = cameraRight->position();
        auto viewCenter = cameraRight->viewCenter();
        if (delta.y() < 0) {
            auto ans = position + (position - viewCenter) / 10.0;
            cameraRight->setPosition(ans);
        }
        else {
            auto ans = position - (position - viewCenter) / 10.0;
            cameraRight->setPosition(ans);
        }
        return;
    }
}

void GLFinalWork::onMouseMoved(QPoint curpos) {
    My3DWindow* window = dynamic_cast<My3DWindow*>(sender());
    if (window == nullptr) {
        qDebug() << "不支持的发送方" << '\n';
        return;
    }

    auto position = cameraLeft->position();
    auto viewCenter = cameraLeft->viewCenter();

    QPoint delta;
    if (window == leftWindow) {
        delta = curpos - leftWindow->last_point;
        QVector3D res, tmp{ -(float)delta.x()/100, -(float)delta.y()/ 100, 0 };

        QVector3D normal_to = position - viewCenter;
        QVector3D normal_from{ 0,0,(position - viewCenter).length() };

        // 摄像机到视野中心的距离
        double dis = (position - viewCenter).length();
        // 鼠标移动的长度
        double delta_dis = tmp.length();
        // 视线变化的角度
        double ang = delta_dis / (pi * dis);
        
        // 这个是半成品
        tmp.normalize();
        tmp = position + calaTransform(normal_from, normal_to, tmp * dis * std::tan(ang));
        //tmp = position + calaTransform(normal_from, normal_to, tmp);

        res = viewCenter + (tmp - viewCenter) / std::cos(ang);

        cameraLeft->setPosition(res);
        
        //qDebug() << dis << ' ' << (res - viewCenter).length() << '\n';

        leftWindow->last_point = curpos;
    }
    else if (window == rightWindow) {
        delta = curpos - rightWindow->last_point;
    }
}

void GLFinalWork::onMousePressed(QPoint curpos) {

}

void GLFinalWork::onMouseReleased(QPoint curpos) {

}