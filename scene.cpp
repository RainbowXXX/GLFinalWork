#include "stdafx.h"
#include "scene.h"
#include "modelviewer.h"

Scene::Scene(Qt3DExtras::Qt3DWindow *view, QObject *parent) : QObject (parent)
{
    rootEntity = new Qt3DCore::QEntity();
    view->setRootEntity(rootEntity);
    view->defaultFrameGraph()->setClearColor(QColor(QRgb(0x42f4bc)));
    Camera(view);
    StartScene();

}

void Scene::Camera(Qt3DExtras::Qt3DWindow *view)
{
    Qt3DRender::QCamera *camera = view->camera();
    camera->lens()->setPerspectiveProjection(45.0f,16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0, 0, 40));
    camera->setViewCenter(QVector3D(0, 0, 0));
}

void Scene::StartScene()
{
    model = new Qt3DCore::QEntity(rootEntity);

    curMesh = nullptr;

    SetupTransform();
    SetupMaterial();
    SetupLighting();

}

void laplacianSmoothing(std::vector<MyMesh>& table, int iterations, double lambda, int max_thread = 100);

void Scene::NewScene()
{
    {
        MyMesh mesh;
        // 从文件中读取模型
        if (!OpenMesh::IO::read_mesh(mesh, cur_source)) {
            qDebug() << "无法读取模型文件" << '\n';
            return;
        }
        scale(mesh);
        // 将结果写入文件
        if (!OpenMesh::IO::write_mesh(mesh, cur_source + ".obj")) {
            qDebug() << "无法写入平滑后的模型文件" << '\n';
            return;
        }
    }

    delete model;
    model = new Qt3DCore::QEntity(rootEntity);

    Qt3DRender::QMesh* mesh = new Qt3DRender::QMesh();
    QUrl data = QUrl::fromLocalFile(QString::fromStdString(cur_source + ".obj"));
    mesh->setSource(data);

    SetupMesh(mesh);
    SetupTransform();
    SetupMaterial();
    SetupLighting();
}

void Scene::SetupMesh(Qt3DRender::QMesh *mesh)
{
    model->addComponent(mesh);
    curMesh = (Qt3DRender::QMesh*)mesh;
}

void Scene::SetupTransform()
{
    transform = new Qt3DCore::QTransform(model);
    transform->setScale3D(QVector3D(1, 1, 1));
    transform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 0));
    model->addComponent(transform);
}

void Scene::SetupMaterial()
{
    Qt3DRender::QMaterial *material = new Qt3DExtras::QPhongMaterial(model);
    model->addComponent(material);
}

void Scene::SetupLighting()
{
    Qt3DCore::QEntity *lightEntity = new Qt3DCore::QEntity(rootEntity);
    light = new Qt3DRender::QDirectionalLight(lightEntity);
    light->setColor("white");
    light->setIntensity(1);
    light->setWorldDirection(QVector3D(0,0,-1));

    lightEntity->addComponent(light);
}

void Scene::KeyControls(QKeyEvent *event){
    if (event->modifiers().testFlag(Qt::ControlModifier)){
        if(event->key()==Qt::Key_Up){
            transform->setRotationX(transform->rotationX()-6);
        }
        if(event->key()==Qt::Key_Down){
            transform->setRotationX(transform->rotationX()+6);
        }
        if(event->key()==Qt::Key_Left){
            transform->setRotationY(transform->rotationY()-6);
        }
        if(event->key()==Qt::Key_Right){
            transform->setRotationY(transform->rotationY()+6);
        }
    }else if (event->modifiers().testFlag(Qt::ShiftModifier)) {
        if(event->key()==Qt::Key_Up){
            transform->setTranslation(QVector3D(transform->translation().x(),transform->translation().y(),transform->translation().z()-1));
        }
        if(event->key()==Qt::Key_Down){
            transform->setTranslation(QVector3D(transform->translation().x(),transform->translation().y(),transform->translation().z()+1));
        }
    }else{
        if(event->key()==Qt::Key_Up){
            transform->setTranslation(QVector3D(transform->translation().x(),transform->translation().y()+1,transform->translation().z()));
        }
        if(event->key()==Qt::Key_Down){
            transform->setTranslation(QVector3D(transform->translation().x(),transform->translation().y()-1,transform->translation().z()));
        }
        if(event->key()==Qt::Key_Left){
            transform->setTranslation(QVector3D(transform->translation().x()-1,transform->translation().y(),transform->translation().z()));
        }
        if(event->key()==Qt::Key_Right){
            transform->setTranslation(QVector3D(transform->translation().x()+1,transform->translation().y(),transform->translation().z()));
        }
    }
}

void Scene::MouseControls(QMouseEvent* event, QEvent::Type eventType) {
    switch (eventType)
    {
    case QEvent::Enter:
        this->is_in_sence = true;
        return;
    case QEvent::Leave:
        this->is_in_sence = false;
        return;
    case QEvent::MouseButtonPress:
        this->is_in_press = true;
        return;
    case QEvent::MouseButtonRelease:
        this->is_in_press = false;
        return;
    case QEvent::MouseMove:
    {
        auto cur_pos = event->pos();
        if (this->is_in_sence and this->is_in_press) {
            auto [cur_x, cur_y] = cur_pos;

            double d_x = cur_x - last_pos.x();
            double d_y = cur_y - last_pos.y();

            d_x *= 6;
            d_y *= 6;

            qDebug() << d_x << ' ' << d_y << '\n';

            transform->setRotationX(transform->rotationX() + d_x);
            transform->setRotationY(transform->rotationY() + d_y);

        }
        this->last_pos = cur_pos;
    }
        return;
    default:
        break;
    }
}

void Scene::LightXChanged(int angle)
{
    light->setWorldDirection(QVector3D(angle,light->worldDirection().y(),light->worldDirection().z()));
}

void Scene::LightYChanged(int angle)
{
    light->setWorldDirection(QVector3D(light->worldDirection().x(),angle,light->worldDirection().z()));
}

void Scene::LightZChanged(int angle)
{
    light->setWorldDirection(QVector3D(light->worldDirection().x(),light->worldDirection().y(),angle));
}

// 正态分布的概率密度函数
double normalPDF(double x, double mean = 0.0, double stddev = 1.0) {
    double coefficient = 1.0 / (stddev * std::sqrt(2 * M_PI));
    double exponent = std::exp(-0.5 * std::pow((x - mean) / stddev, 2));
    return coefficient * exponent;
}

void work(int begin, int end, double lambda, MyMesh& mesh, MyMesh& smoothedMesh, std::mutex& mutex, std::vector<std::vector<int>>& E) {

    for (int i = begin; i < end; i++) {
        MyMesh::VertexHandle vh = mesh.vertex_handle(i);
        OpenMesh::DefaultTraits::Point origin_tmp = mesh.point(vh);
        Eigen::Vector3d sum(0, 0, 0), origin{ origin_tmp[0], origin_tmp[1], origin_tmp[2] };

        double totalWeight = lambda + E[vh.idx()].size();
        for (auto j : E[i]) {
            MyMesh::VertexHandle vh_to = mesh.vertex_handle(j);

            auto temp = mesh.point(vh_to);
            Eigen::Vector3d tmp{ temp[0], temp[1], temp[2] };

            sum += tmp;
        }

        auto temp = mesh.point(vh);
        sum += lambda * Eigen::Vector3d{ temp[0], temp[1], temp[2] };

        Eigen::Vector3d new_pos = sum / totalWeight;

        {
            //mutex.lock();
            MyMesh::VertexHandle vh_from = smoothedMesh.vertex_handle(vh.idx());
            smoothedMesh.set_point(vh_from, MyMesh::Point(new_pos[0], new_pos[1], new_pos[2]));
            //mutex.unlock();
        }
    }
}

void remesh(
    MyMesh& mesh,
    std::vector<std::vector<int>>& E,
    double eps = 1e-4) {

    size_t sub_face = 0;
    for (auto face : mesh.all_faces()) {
        if (mesh.calc_face_area(face) < eps) sub_face++;
    }
    if (sub_face < 0.1 * mesh.n_faces()) return;

    OpenMesh::Decimater::DecimaterT<MyMesh> decimater(mesh);
    OpenMesh::Decimater::ModQuadricT<MyMesh>::Handle hModQuadric;
    decimater.add(hModQuadric);

    decimater.initialize();

    decimater.decimate_to_faces(0, mesh.n_faces() - sub_face);

    // Remove the deleted faces and vertices
    mesh.garbage_collection();

    E.resize(mesh.n_vertices(), std::vector<int>{});
    for (auto v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it) {
        MyMesh::VertexHandle vh = *v_it;
        for (auto vv_it = mesh.vv_iter(vh); vv_it.is_valid(); ++vv_it) {
            MyMesh::VertexHandle vh_to = *vv_it;
            E[vh.idx()].emplace_back(vh_to.idx());
        }
    }

}

void Scene::scale(MyMesh& mesh) {
    // 2. 计算模型的中心位置
    MyMesh::Point center(0.0, 0.0, 0.0);
    for (const auto& v : mesh.vertices()) {
        center += mesh.point(v);
    }
    center /= mesh.n_vertices();

    // 3. 将模型平移到中心位置
    for (auto& v : mesh.vertices()) {
        mesh.set_point(v, mesh.point(v) - center);
    }

    // 4. 计算模型的包围盒大小（或其他尺寸指标），这里使用包围盒的对角线长度作为参考
    float diagonal_length = 0.0f;
    for (const auto& v : mesh.vertices()) {
        float distance = (mesh.point(v) - MyMesh::Point(0.0, 0.0, 0.0)).length();
        if (distance > diagonal_length) {
            diagonal_length = distance;
        }
    }

    // 5. 缩放模型，使其大小适合所需的距离
    float target_distance = 10.0f; // 目标距离
    float scale_factor = target_distance / diagonal_length;
    for (auto& v : mesh.vertices()) {
        mesh.set_point(v, mesh.point(v) * scale_factor);
    }
}

void laplacianSmoothing(MyMesh& mesh, int iterations, double lambda, int max_thread) {
    size_t n = mesh.n_vertices();
    std::vector<std::vector<int>> E(n);

    for (auto v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it) {
        MyMesh::VertexHandle vh = *v_it;
        for (auto vv_it = mesh.vv_iter(vh); vv_it.is_valid(); ++vv_it) {
            MyMesh::VertexHandle vh_to = *vv_it;
            E[vh.idx()].emplace_back(vh_to.idx());
        }
    }
    std::mutex mutex{};

    while (iterations--) {
        MyMesh smoothedMesh = mesh;
        int block_size = (n + max_thread - 1) / max_thread, block_num = (n + block_size - 1) / block_size;
        {
            std::vector<std::jthread> thread_pool;
            for (int i = 0; i < block_num; i++) {
                int begin = i * block_size;
                int end = (i + 1) * block_size;
                end = std::min(end, (int)n);
                thread_pool.emplace_back(work, begin, end, lambda, std::ref(mesh), std::ref(smoothedMesh), std::ref(mutex), std::ref(E));
            }
        }
        mesh = std::move(smoothedMesh);
        remesh(mesh, E, 1e-7);
        n = mesh.n_vertices();
    }
}

void Scene::setLambda(double new_val) {
    lambda = new_val;
}
void Scene::setIters(int new_val) {
    iters = new_val;
}
void Scene::setMaxThreads(int new_val) {
    max_threads = new_val;
}

void Scene::startTransform(bool checked)
{
    auto _thread = [&]() {
        MyMesh mesh;
        // 从文件中读取模型
        model->removeComponent(curMesh);
        if (!OpenMesh::IO::read_mesh(mesh, cur_source)) {
            qDebug() << "无法读取模型文件" << '\n';
            return;
        }
        laplacianSmoothing(mesh, iters, lambda, max_threads);
        scale(mesh);
        // 将结果写入文件
        if (!OpenMesh::IO::write_mesh(mesh, cur_source + ".obj")) {
            qDebug() << "无法写入平滑后的模型文件" << '\n';
            return;
        }

        curMesh->setSource(QUrl::fromLocalFile(QString::fromStdString(cur_source + ".obj1")));
        curMesh->setSource(QUrl::fromLocalFile(QString::fromStdString(cur_source + ".obj")));

        //cur_source = cur_source + ".obj";

        model->addComponent(curMesh);
        };
    std::thread{ _thread }.detach();
}
