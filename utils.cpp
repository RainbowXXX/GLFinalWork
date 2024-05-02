#include "stdafx.h"
#include "utils.h"

Eigen::Matrix3d calcTransMatrix(const Eigen::Vector3d& vector1, const Eigen::Vector3d& vector2) {
    using namespace Eigen;
    Vector3d n1 = vector1.normalized();
    Vector3d n2 = vector2.normalized();

    Vector3d axis = n1.cross(n2);
    double angle = std::acos(n1.dot(n2));

    Matrix3d K;
    K << 0, -axis[2], axis[1],
        axis[2], 0, -axis[0],
        -axis[1], axis[0], 0;
    Matrix3d rotationMatrix = Matrix3d::Identity() + std::sin(angle) * K + (1 - std::cos(angle)) * K * K;

    return rotationMatrix;
}

QVector3D calaTransform(const QVector3D& normal_vector, const QVector3D& new_normal_vector, const QVector3D& vector) {
    // 保证法向量和被转换的向量垂直(保证向量在法向量所在的平面内)
    assert(QVector3D::dotProduct(normal_vector, vector) == 0);

    Eigen::Vector3d vector0{ vector[0], vector[1], vector[2] };
    Eigen::Vector3d vector1{ normal_vector[0], normal_vector[1], normal_vector[2] };
    Eigen::Vector3d vector2{ new_normal_vector[0], new_normal_vector[1], new_normal_vector[2] };
    Eigen::Matrix3d matrix = calcTransMatrix(vector1, vector2);

    Eigen::Vector3d ret_vector = matrix * vector0;
    return QVector3D{ (float)ret_vector[0], (float)ret_vector[1], (float)ret_vector[2] };
}