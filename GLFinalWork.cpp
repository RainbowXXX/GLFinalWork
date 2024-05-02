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

    leftWindow = new Qt3DExtras::Qt3DWindow; rightWindow = new Qt3DExtras::Qt3DWindow;

    containerLeft = createWindowContainer(leftWindow, leftFrame);
    containerRight = createWindowContainer(rightWindow, rightFrame);

    layout->addWidget(containerLeft);
    layout->addWidget(containerRight);

    // Left window setup
    Qt3DCore::QEntity* rootEntityLeft = new Qt3DCore::QEntity;
    leftWindow->setRootEntity(rootEntityLeft);

    Qt3DRender::QCamera* cameraLeft = leftWindow->camera();
    cameraLeft->lens()->setPerspectiveProjection(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
    cameraLeft->setPosition(QVector3D(0, 0, 10));
    cameraLeft->setViewCenter(QVector3D(0, 0, 0));

    Qt3DCore::QEntity* modelEntityLeft = new Qt3DCore::QEntity(rootEntityLeft);
    Qt3DRender::QMesh* meshLeft = new Qt3DRender::QMesh;
    meshLeft->setSource(QUrl::fromLocalFile("D:\\Documents\\source\\repos\\GLFinalWork\\Noisy.obj"));
    modelEntityLeft->addComponent(meshLeft);
    Qt3DExtras::QPhongMaterial* materialLeft = new Qt3DExtras::QPhongMaterial;
    modelEntityLeft->addComponent(materialLeft);

    // Right window setup (similar to left window)
    Qt3DCore::QEntity* rootEntityRight = new Qt3DCore::QEntity;
    rightWindow->setRootEntity(rootEntityRight);

    Qt3DRender::QCamera* cameraRight = rightWindow->camera();
    cameraRight->lens()->setPerspectiveProjection(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
    cameraRight->setPosition(QVector3D(0, 0, 10));
    cameraRight->setViewCenter(QVector3D(0, 0, 0));

    Qt3DCore::QEntity* modelEntityRight = new Qt3DCore::QEntity(rootEntityRight);
    //Qt3DRender::QMesh* meshRight = new Qt3DRender::QMesh;
    //meshRight->setSource(QUrl::fromLocalFile("path/to/your/mesh.obj"));
    Qt3DExtras::QCuboidMesh* meshRight = new Qt3DExtras::QCuboidMesh;
    modelEntityRight->addComponent(meshRight);
    Qt3DExtras::QPhongMaterial* materialRight = new Qt3DExtras::QPhongMaterial;
    modelEntityRight->addComponent(materialRight);
}

GLFinalWork::~GLFinalWork()
{
    delete this->centralWidget;
}
