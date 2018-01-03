#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDateTime>
#include <QFile>
#include <QBoxLayout>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>

#include "dialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle(QString("TaskTimer version 1  Build: ") + __DATE__ + " " + __TIME__ );
    setWindowIcon(QIcon(":/timer.png"));
    timerEvent(0);

    // Чтение данных
    QFile file;
    file.setFileName("TaskTimer.txt");
    if(file.open(QIODevice::ReadOnly)){
        QString s;
        while(!(s = file.readLine()).isEmpty()){
            s = s.remove('\r').remove('\n');
            QStringList sl = s.split(' ');
            QString taskName = s.right(s.size()-s.indexOf(' ', s.indexOf(' ')+1)-1);

            if(sl.size()>1)
                addRow(QDateTime().fromMSecsSinceEpoch(sl[0].toLongLong()), sl[1].toInt(), taskName.toUtf8());
        }
        file.close();
    }

    minimalInit();

    tray.reset(new QSystemTrayIcon);
    tray->setIcon(QIcon(":/timer.png"));
    createMenu();
    tray->show();

    settings.reset(new QSettings("TaskTimer.ini", QSettings::IniFormat, this));
    if(settings->value("minimal").isValid())
        if(settings->value("minimal").toBool()) {
            setVisible(false);
            dialog->setVisible(true);
        }

    if(settings->value("minimal_geometry").isValid())
        dialog->setGeometry(settings->value("minimal_geometry").toRect());

    if(!dialog->isVisible()) show();
}

MainWindow::~MainWindow()
{
    settings->setValue("minimal", !isVisible());
    settings->setValue("minimal_geometry", dialog->geometry());

    if(inProgress) on_pbStart_clicked();
    delete ui;
}

void MainWindow::minimalInit()
{
    // Минималистический диалог
    dialog.reset(new Dialog());
    dialog->setGeometry(x(),0,0,0);
    auto layout = new QHBoxLayout;
    layout->setMargin(0);

    auto text = new QLabel(ui->labelTime->text(), dialog.data());
    text->setFont(QFont(ui->labelTime->font()));
    connect(this, MainWindow::onTimer, [=](QString t){ text->setText(t); });
    layout->addWidget(text);

    auto setIcon = [](bool f)->QIcon { return QIcon(f ? ":/stop.png" : ":/start.png"); };

    auto stop = new QPushButton(setIcon(inProgress), "", this);
    connect(stop, QPushButton::clicked, [=](){
        ui->pbStart->clicked();
        stop->setIcon(setIcon(inProgress));
    });
    stop->setMaximumWidth(30);
    layout->addWidget(stop);

    dialog->setWindowOpacity(0.7);
    dialog->setLayout(layout);
}

void MainWindow::createMenu()
{
    trayMenu.reset(new QMenu(this));

    auto actionShowHide = new QAction (tr("Скрыть\\Развернуть"), this);
    trayMenu->addAction(actionShowHide);
    connect(actionShowHide, SIGNAL(triggered(bool)), this, SLOT(on_pbMinimal_clicked()));

    auto actionExit = new QAction (tr("Выход"), this);
    trayMenu->addAction(actionExit);
    connect(actionExit, QAction::triggered, [&](){ QCoreApplication::quit(); });

    tray->setContextMenu(trayMenu.data());
}

void MainWindow::on_pbStart_clicked()
{
    inProgress = !inProgress;

    if(inProgress) {
        startTime = QDateTime::currentDateTime();
        timerId = startTimer(1000);
        timerEvent(0);
    }
    else{
        killTimer(timerId);
        addRow(startTime, diffTime(), ui->cbTaskName->currentText());
        timerEvent(0);
        fileAppend();
    }

    ui->pbStart->setText(inProgress ? "Stop" : "Start");
}

void MainWindow::addRow(QDateTime dateTime, int dt, QString taskName)
{
    int row=0, column = 0;
    ui->tableWidget->insertRow(row);

    auto value = map.find(taskName);
    if(value!=map.end()) *value+=dt;
    else map.insert(taskName,dt);

    if(ui->cbTaskName->findText(taskName)<0)
        ui->cbTaskName->addItem(taskName);
    if(ui->cbTaskName->currentText() == taskName)
        ui->labelFullTime->setText(timeToString(map[taskName]));

    for(auto& s: { dateTime.toString("hh:mm:ss dd.MM.yyyy"),  timeToString(dt),  taskName }){
        ui->tableWidget->setItem(row, column, new QTableWidgetItem(s));
        ui->tableWidget->item(row, column)->setTextAlignment(Qt::AlignCenter);
        column++;
    }
}

void MainWindow::fileAppend()
{
    QFile file;
    file.setFileName("TaskTimer.txt");
    if(file.open(QIODevice::Append)){
        QString s;
        s.sprintf("%lli %i %s\r\n", startTime.currentMSecsSinceEpoch(), diffTime(), ui->cbTaskName->currentText().toUtf8().data());
        file.write(s.toUtf8().data());
        file.close();
    }
}

int MainWindow::diffTime()
{
    return startTime.secsTo(QDateTime::currentDateTime());
}

QString MainWindow::timeToString(int time, int color)
{
    QString s = QString().sprintf("%02i:%02i:%02i", time/3600, (time/60)%60, time%60);
    return color ? QString().sprintf("<font color=#%06X>", color) + s + "</font>" : s;
}

void MainWindow::timerEvent(QTimerEvent *)
{
    QString text = timeToString(diffTime(), inProgress ? 0x229911 : 0x884411);
    emit onTimer(text);
    ui->labelTime->setText(text);
}

void MainWindow::on_cbTaskName_currentTextChanged(const QString &arg)
{
    if(map.find(arg)!=map.end()) ui->labelFullTime->setText(timeToString(map[arg]));
}

void MainWindow::on_pbMinimal_clicked()
{
    bool minimal = dialog->isVisible();
    dialog->setVisible(!minimal);
    setVisible(minimal);
}

