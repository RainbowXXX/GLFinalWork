#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_GLFinalWork.h"

#include <QtWidgets>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QMesh>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QPointLight>

class GLFinalWork : public QMainWindow
{
    Q_OBJECT

public:
    GLFinalWork(QWidget *parent = nullptr);
    ~GLFinalWork();

private:
    Ui::GLFinalWorkClass ui;

    QWidget* centralWidget;

    QHBoxLayout* layout;
    QFrame* leftFrame, *rightFrame;

    QWidget* containerLeft, * containerRight;
    Qt3DExtras::Qt3DWindow* leftWindow, *rightWindow;
};
