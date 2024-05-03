#pragma once

#include <QMainWindow>
#include <Qt3DCore/QEntity>
#include <Qt3DWindow>

#include <QKeyEvent>

#include <QFile>
#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include <QMesh>
#include <QUrl>

#include <QSlider>
#include <qpushbutton.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

#include <scene.h>

namespace Ui {
class ModelViewer;
}

class ModelViewer : public QMainWindow
{
    Q_OBJECT

public:
    explicit ModelViewer(QWidget *parent = nullptr);
    ~ModelViewer();

    void SetLayout();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

signals:
    void KeyPressed();

private slots:
    void on_actionOpen_File_triggered();

private:
    Ui::ModelViewer *ui;

    QSlider* createSlider();
    QPushButton *createButton();
    Scene *scene;

    QString currentFile = "";
};
