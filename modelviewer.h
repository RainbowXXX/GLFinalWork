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
#include <qlineedit.h>
#include <qpushbutton.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

#include "scene.h"

namespace Ui {
class ModelViewer;
}

class ModelViewer : public QMainWindow
{
    Q_OBJECT

public:
    QString old_iterations, old_lambda, old_max_thread;

    explicit ModelViewer(QWidget *parent = nullptr);
    ~ModelViewer();

    void SetLayout();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

signals:
    void KeyPressed();

private slots:
    void check_text_int(const QString& new_val);
    void check_text_double(const QString& new_val);
    void on_actionOpen_File_triggered();

private:
    Ui::ModelViewer *ui;

    QSlider* createSlider();
    QPushButton* createButton();
    QLineEdit *createEdit(QString defaultVal);
    Scene *scene;

    QLineEdit* iterations, * lambda, * max_thread;

    QString currentFile = "";
};
