#pragma once
#include <qvector3d.h>
#include <Eigen/Dense>

/// <summary>
/// 使用罗德里格斯公式计算旋转矩阵(从vector1到vector2的变换)
/// </summary>
/// <param name="vector1"></param>
/// <param name="vector2"></param>
/// <returns></returns>
Eigen::Matrix3d calcTransMatrix(const Eigen::Vector3d& vector1, const Eigen::Vector3d& vector2);

/// <summary>
/// 将一个平面的向量转换为三维中另一个平面的向量
/// </summary>
/// <param name="normal_vector">原本平面的法向量</param>
/// <param name="new_normal_vector">新平面的法向量</param>
/// <param name="vector">原本的向量</param>
/// <returns>转换过后的另一个平面的向量</returns>
QVector3D calaTransform(const QVector3D& normal_vector, const QVector3D& new_normal_vector, const QVector3D& vector);