#pragma once

#include <QObject>
#include <QWidget>

#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QPointLight>
#include <Qt3DRender/QDirectionalLight>
#include <Qt3DCore/QTransform>
#include <Qt3DWindow>

#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QTorusMesh>

#include <QKeyEvent>
#include <QTransform>
#include <QMesh>

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <Eigen/Core>
#include <Eigen/Sparse>

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Tools/Smoother/LaplaceSmootherT.hh>
#include <OpenMesh/Tools/Smoother/JacobiLaplaceSmootherT.hh>

class Scene : public QObject
{
    Q_OBJECT
public:
    explicit Scene(Qt3DExtras::Qt3DWindow *, QObject *parent=nullptr);

    std::string cur_source;
    void NewScene(Qt3DRender::QMesh *);
    void KeyControls(QKeyEvent* event);
    void MouseControls(QMouseEvent* event, QEvent::Type eventType);


public slots:
    void LightXChanged(int angle);
    void LightYChanged(int angle);
    void LightZChanged(int angle);
    void startTransform(bool checked);

private:
    // 鼠标上一次的位置
    QPoint last_pos;
    // 上一次更新的时候鼠标是否在边界内
    bool is_in_sence;
    // 上一次更新的时候鼠标左键是否被按下
    bool is_in_press;
    Qt3DRender::QMesh* curMesh;

    Qt3DCore::QEntity *rootEntity;
    Qt3DCore::QEntity *model;
    Qt3DCore::QTransform *transform;
    Qt3DRender::QDirectionalLight *light;

    void StartScene();
    void Camera(Qt3DExtras::Qt3DWindow *view);
    void SetupMesh(Qt3DRender::QMesh *mesh);
    void SetupTransform();
    void SetupMaterial();
    void SetupLighting();
};
