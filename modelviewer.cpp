#include "stdafx.h"
#include "modelviewer.h"
#include "ui_modelviewer.h"
#include "scene.h"

ModelViewer::ModelViewer(QWidget *parent) :
    QMainWindow (parent),
    ui(new Ui::ModelViewer)
{
    ui->setupUi(this);
    setFocusPolicy(Qt::TabFocus);

    //central widget
    QWidget *central = new QWidget();
    setCentralWidget(central);

    //scene
    Qt3DExtras::Qt3DWindow *view = new Qt3DExtras::Qt3DWindow();
    QWidget *sceneWidget = QWidget::createWindowContainer(view);
    scene = new Scene(view);
    view-> installEventFilter(this);

    //slider
    QSlider *xSlider = createSlider();
    QSlider *ySlider = createSlider();
    QSlider *zSlider = createSlider();
    zSlider->setValue(-1);

    connect(xSlider,&QSlider::valueChanged,scene,&Scene::LightXChanged);
    connect(ySlider,&QSlider::valueChanged,scene,&Scene::LightYChanged);
    connect(zSlider,&QSlider::valueChanged,scene,&Scene::LightZChanged);

    //layout
    QLabel *xLabel = new QLabel();
    QLabel *yLabel = new QLabel();
    QLabel *zLabel = new QLabel();
    xLabel->setText("X");
    yLabel->setText("Y");
    zLabel->setText("Z");
    QVBoxLayout *xLayout = new QVBoxLayout;
    QVBoxLayout *yLayout = new QVBoxLayout;
    QVBoxLayout *zLayout = new QVBoxLayout;
    xLayout->addWidget(xSlider);
    xLayout->addWidget(xLabel);
    yLayout->addWidget(ySlider);
    yLayout->addWidget(yLabel);
    zLayout->addWidget(zSlider);
    zLayout->addWidget(zLabel);
    QWidget *xWidget = new QWidget();
    QWidget *yWidget = new QWidget();
    QWidget *zWidget = new QWidget();
    xWidget->setLayout(xLayout);
    yWidget->setLayout(yLayout);
    zWidget->setLayout(zLayout);
    xWidget->setFixedWidth(50);
    yWidget->setFixedWidth(50);
    zWidget->setFixedWidth(50);

    QHBoxLayout *sceneLayout = new QHBoxLayout;
    sceneLayout->addWidget(sceneWidget);
    sceneLayout->addWidget(xWidget);
    sceneLayout->addWidget(yWidget);
    sceneLayout->addWidget(zWidget);
    QWidget *mainWidget = new QWidget();
    mainWidget->setLayout(sceneLayout);

    QPushButton* actionButton = createButton();
    connect(actionButton, &QPushButton::clicked, scene, &Scene::startTransform);

    QLabel *controlDescription = new QLabel();
    controlDescription->setStyleSheet("font: 12pt;");
    controlDescription->setText("File->Open File to open a 3D model file. \n"
                                "Use arrows to move model. \n"
                                "Use shift + arrows to zoom model. \n"
                                "Use ctrl + arrows to rotate model. \n"
                                "Use sliders to change lighting direction.");
    QVBoxLayout *topLayout = new QVBoxLayout;
    topLayout->addWidget(mainWidget);
    topLayout->addWidget(controlDescription);
    topLayout->addWidget(actionButton);
    central->setLayout(topLayout);
}

QSlider *ModelViewer::createSlider()
{
    QSlider *slider = new QSlider(Qt::Vertical);
    slider->setRange(-180, 180);
    slider->setSingleStep(1);
    slider->setPageStep(15);
    slider->setTickInterval(15);
    slider->setTickPosition(QSlider::TicksRight);
    return slider;
}

QPushButton* ModelViewer::createButton()
{
    QPushButton* button = new QPushButton();
    button->setText("平滑");
    return button;
}

ModelViewer::~ModelViewer()
{
    delete ui;
}

bool ModelViewer::eventFilter(QObject *obj, QEvent *event)
{
    switch (event->type())
    {
    case QEvent::KeyPress:
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        scene->KeyControls(keyEvent);
        break;
    }
    case QEvent::Enter:
    case QEvent::Leave:
    {
        scene->MouseControls(nullptr, event->type());
        break;
    }
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove:
    {
        QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
        if (mouseEvent == nullptr) {
            qDebug() << "Problems during event type conversion." << event->type() << '\n';
            break;
        }
        scene->MouseControls(mouseEvent, event->type());
        break;
    }
    default:
        qDebug() << "Unsupported events:" << event->type() << '\n';
        break;
    }

    return QObject::eventFilter(obj, event);
}

void ModelViewer::on_actionOpen_File_triggered()
{
    QString filename= QFileDialog::getOpenFileName(this,"Open file");
    QFile file(filename);
    currentFile = filename;
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this,"Warning", "Cannot open "+file.errorString());
    }
    setWindowTitle(filename);
    Qt3DRender::QMesh *mesh = new Qt3DRender::QMesh();
    QUrl data =QUrl::fromLocalFile(filename);
    mesh->setSource(data);

    scene->cur_source = filename.toStdString();
    scene->NewScene(mesh);
}
