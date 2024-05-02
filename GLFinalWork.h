#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_GLFinalWork.h"

#include <QtWidgets>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QMesh>
#include <Qt3DRender/QCamera>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DRender/QPointLight>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QOrbitCameraController>

#include "My3DWindow.h"
#include "utils.h"

class GLFinalWork : public QMainWindow
{
    Q_OBJECT

public:
    static inline double pi = acos(-1.0);
    GLFinalWork(QWidget *parent = nullptr);
    ~GLFinalWork();

private:
    Ui::GLFinalWorkClass ui;

    QHBoxLayout* layout;
    QWidget* centralWidget;
    QFrame* leftFrame, *rightFrame;
    QWidget* containerLeft, * containerRight;
    My3DWindow* leftWindow, *rightWindow;

    Qt3DRender::QMesh* meshLeft, *meshRight;
    Qt3DRender::QCamera* cameraLeft, *cameraRight;
    Qt3DCore::QEntity* rootEntityLeft, *rootEntityRight;
    Qt3DCore::QEntity* modelEntityLeft, *modelEntityRight;
    Qt3DExtras::QPhongMaterial* materialLeft, *materialRight;

public slots:
    void onWheelScrolled(QPoint delta);

    void onMouseMoved(QPoint curpos);

    void onMousePressed(QPoint curpos);

    void onMouseReleased(QPoint curpos);
};
