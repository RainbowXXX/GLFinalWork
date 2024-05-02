#pragma once

#include <Qt3DExtras/Qt3DWindow>

class My3DWindow  : public Qt3DExtras::Qt3DWindow
{
	Q_OBJECT

public:
    QPoint last_point;

	My3DWindow(QScreen* screen = nullptr);
	~My3DWindow() noexcept;

signals:
    void wheelScrolled(QPoint delta);
    void mouseReleased(QPoint delta);
    void mousePressed(QPoint delta);
    void mouseMoved(QPoint delta);
private:
    void wheelEvent(QWheelEvent* event) override
    {
        QPoint delta = event->angleDelta();
        emit wheelScrolled(delta);

        Qt3DExtras::Qt3DWindow::wheelEvent(event);
    }
    void mouseMoveEvent(QMouseEvent* event) override {
        emit mouseMoved(event->pos());
        Qt3DExtras::Qt3DWindow::mouseMoveEvent(event);
    }
    void mousePressEvent(QMouseEvent* event) override
    {
        if (event->button() == Qt::LeftButton) {
            emit mousePressed(event->pos());
        }
        Qt3DExtras::Qt3DWindow::mousePressEvent(event);
    }

    void mouseReleaseEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            emit mousePressed(event->pos());
        }
        Qt3DExtras::Qt3DWindow::mouseReleaseEvent(event);
    }
};
