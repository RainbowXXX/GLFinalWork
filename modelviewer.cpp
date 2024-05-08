#include "stdafx.h"
#include "modelviewer.h"
#include "ui_modelviewer.h"
#include "scene.h"

ModelViewer::ModelViewer(QWidget* parent) :
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

    this->old_lambda = "0.5";
    this->old_iterations = "3";
    this->old_max_thread = "20";

    this->scene->setLambda(0.5);
    this->scene->setIters(3);
    this->scene->setMaxThreads(20);

    iterations = createEdit(old_iterations), lambda = createEdit(old_lambda), max_thread = createEdit(old_max_thread);

    connect(lambda, &QLineEdit::textChanged, this, &ModelViewer::check_text_double);
    connect(iterations, &QLineEdit::textChanged, this, &ModelViewer::check_text_int);
    connect(max_thread, &QLineEdit::textChanged, this, &ModelViewer::check_text_int);

    //QLabel *controlDescription = new QLabel();
    //controlDescription->setStyleSheet("font: 12pt;");
    //controlDescription->setText("File->Open File to open a 3D model file. \n"
    //                            "Use arrows to move model. \n"
    //                            "Use shift + arrows to zoom model. \n"
    //                            "Use ctrl + arrows to rotate model. \n"
    //                            "Use sliders to change lighting direction.");

    QLabel* iterations_label = new QLabel(), * lambda_label = new QLabel(), * max_thread_label = new QLabel();
    QHBoxLayout* iterations_box = new QHBoxLayout(), * lambda_box = new QHBoxLayout(), * max_thread_box = new QHBoxLayout();

    iterations_label->setText(  "迭 代 次 数");
    lambda_label->setText(      "lambda 值");
    max_thread_label->setText(  "最大线程数");

    iterations_box->addWidget(iterations_label);
    lambda_box->addWidget(lambda_label);
    max_thread_box->addWidget(max_thread_label);

    iterations_box->addWidget(iterations);
    lambda_box->addWidget(lambda);
    max_thread_box->addWidget(max_thread);

    QVBoxLayout *topLayout = new QVBoxLayout;
    topLayout->addWidget(mainWidget);
    //topLayout->addWidget(controlDescription);
    //topLayout->addWidget(iterations);
    //topLayout->addWidget(lambda);
    //topLayout->addWidget(max_thread);

    topLayout->addLayout(iterations_box);
    topLayout->addLayout(lambda_box);
    topLayout->addLayout(max_thread_box);

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

QLineEdit* ModelViewer::createEdit(QString defaultVal) {
    QLineEdit* editor = new QLineEdit();
    editor->setText(defaultVal);
    return editor;
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

void ModelViewer::check_text_int(const QString& new_val)
{
    QLineEdit* editor = dynamic_cast<QLineEdit*>(sender());
    if (editor == nullptr) {
        qDebug() << "不支持的发送者" << '\n';
        return;
    }

    QString old_val;
    bool is_ok = true;
    new_val.toInt(&is_ok);
    if (not is_ok) goto fail;
    else goto ok;

fail:
    if (editor == iterations) old_val = old_iterations;
    else if (editor == lambda) old_val = old_lambda;
    else if (editor == max_thread) old_val = old_max_thread;
    editor->setText(old_val);
    return;
ok:
    if (editor == iterations) old_iterations = new_val, this->scene->setIters(new_val.toInt());
    else if (editor == lambda) old_lambda = new_val, this->scene->setLambda(new_val.toDouble());
    else if (editor == max_thread) old_max_thread = new_val, this->scene->setMaxThreads(new_val.toInt());
    return;
}

void ModelViewer::check_text_double(const QString& new_val)
{
    QLineEdit* editor = dynamic_cast<QLineEdit*>(sender());
    if (editor == nullptr) {
        qDebug() << "不支持的发送者" << '\n';
        return;
    }

    bool is_ok;
    is_ok = true;
    QString old_val;
    new_val.toDouble(&is_ok);
    if (not is_ok) goto fail;
    else goto ok;

fail:
    if (editor == iterations) old_val = old_iterations;
    else if (editor == lambda) old_val = old_lambda;
    else if (editor == max_thread) old_val = old_max_thread;
    editor->setText(old_val);
    return;
ok:
    if (editor == iterations) old_iterations = new_val, this->scene->setIters(new_val.toInt());
    else if (editor == lambda) old_lambda = new_val, this->scene->setLambda(new_val.toDouble());
    else if (editor == max_thread) old_max_thread = new_val, this->scene->setMaxThreads(new_val.toInt());
    return;
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


    scene->cur_source = filename.toStdString();
    scene->NewScene();
}
