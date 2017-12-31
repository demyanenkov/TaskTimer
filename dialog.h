#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QMouseEvent>

class Dialog: public QDialog {

    Q_OBJECT

public:
    Dialog(){
        setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    }

private:
    double dx, dy;

private slots:
    void mouseMoveEvent( QMouseEvent* e ) {
        if( e->buttons() | Qt::LeftButton ) {
            setGeometry(
                pos().x() + ( e->x() - dx ),
                pos().y() + ( e->y() - dy ),
                width(),
                height()
            );
        }
    }
    void mousePressEvent( QMouseEvent* e ) {
        if( e->button() == Qt::LeftButton ) {
            dx = e->x();
            dy = e->y();
            setCursor( Qt::OpenHandCursor );
        }
    }
    void mouseReleaseEvent( QMouseEvent* e ) {
        if( e->button() == Qt::LeftButton ) {
            setCursor( Qt::ArrowCursor );
        }
    }
};

#endif // DIALOG_H

