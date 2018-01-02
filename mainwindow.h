#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include <QMap>

namespace Ui {
    class MainWindow;
}

class Dialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void onTimer(QString text);

private slots:
    void timerEvent(QTimerEvent *) override;
    void on_pbStart_clicked();
    void on_cbTaskName_currentTextChanged(const QString &);
    void on_pbMinimal_clicked();

private:
    int diffTime();
    void fileAppend();
    QString timeToString(int time, int color=0);
    void addRow(QDateTime dateTime, int dt, QString taskName);
    void minimalInit();

    Ui::MainWindow *ui;
    bool inProgress = false;
    QDateTime startTime;
    int timerId;

    QMap<QString,int> map;
    Dialog *dialog;
};

#endif // MAINWINDOW_H
