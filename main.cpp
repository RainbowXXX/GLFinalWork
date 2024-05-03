#include "stdafx.h"
#include "modelviewer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ModelViewer* modelViewer = new ModelViewer();
    modelViewer->show();

    return a.exec();
}
