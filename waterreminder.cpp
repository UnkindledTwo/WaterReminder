#include "waterreminder.h"
#include "ui_waterreminder.h"

WaterReminder::WaterReminder(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::WaterReminder)
{
    ui->setupUi(this);

    ui->statusbar->addWidget(ui->labelStarted);

    trayIcon = new QSystemTrayIcon(this);
    connect(trayIcon, &QSystemTrayIcon::activated, this, &WaterReminder::trayIconActivated);
    initTrayMenu();

    trayIcon->setToolTip("Water Reminder");
    trayIcon->setVisible(true);
    trayIcon->setIcon(QIcon(":/Resources/trayIcon.png"));

    notif = new QSound(":/Resources/notif.wav");

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &WaterReminder::timerTick);
    timer->setInterval(50);
    timer->start();
}

void WaterReminder::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::ActivationReason::DoubleClick) {
        this->setHidden(!this->isHidden());
    }
}

void WaterReminder::closeEvent(QCloseEvent *e)
{
    e->setAccepted(false);
    this->hide();
}

void WaterReminder::initTrayMenu()
{
    QMenu *trayMenu = new QMenu(this);

    QAction *actionExit = new QAction(this);
    actionExit->setText("Exit");
    connect(actionExit, &QAction::triggered, this, [&](bool) {qApp->exit(0);});

    QAction *actionMinimize = new QAction(this);
    actionMinimize->setText("Show/Hide App");
    connect(actionMinimize, &QAction::triggered, this, [&](bool) {
        this->setHidden(!this->isHidden());
    });

    QAction *actionDrankGlass = new QAction(this);
    actionDrankGlass->setText("Drank Glass");
    connect(actionDrankGlass, &QAction::triggered, this, [&](bool) {
        on_bDrankGlass_clicked();
    });

    trayMenu->addAction(actionDrankGlass);
    trayMenu->addAction(actionMinimize);
    trayMenu->addSeparator();
    trayMenu->addAction(actionExit);

    trayIcon->setContextMenu(trayMenu);
}

WaterReminder::~WaterReminder()
{
    delete trayIcon;
    delete ui;
}

void WaterReminder::timerTick() {
    if(glassTimes.empty()) return;
    if(started == false) return;

    QDateTime nextGlass = glassTimes.first();
    QDateTime currentDateTime = QDateTime::currentDateTime();

    int minutesToNextGlass = currentDateTime.secsTo(nextGlass) / 60;
    int secondsToNextGlass = currentDateTime.secsTo(nextGlass) % 60;

    ui->labelNextGlass->setText(QString("Next Glass in %1 Minutes %2 Seconds (%3)").arg(minutesToNextGlass).arg(secondsToNextGlass).arg(nextGlass.time().toString()));
    ui->labelGlassCount->setText(QString("You need to drink %1 glasses of water").arg(glassesToDrink));

    if(nextGlass.time().minute() == currentDateTime.time().minute() &&
            nextGlass.time().hour() == currentDateTime.time().hour() &&
            nextGlass.time().second() == currentDateTime.time().second()) {
        on_bDrankGlass_clicked();
        notif->play();
        system("notify-send '-t' 2000 --app-name='Water Reminder' '--icon=:/Resources/trayIcon.png' 'Water Reminder' 'Drink your water'");
    }

    if(glassesToDrink == 0) {
        QMessageBox::information(this, "Congrats", "You finished your daily target");
        started = false;
    }
}

void WaterReminder::on_bCalculate_clicked()
{
    if(ui->toDrink->value() <= ui->glassVolume->value()) {
        QMessageBox::warning(this, "Error", "Value of amount to drink must be higher than glass volume");
        return;
    }

    started = false;
    drankGlasses = 0;
    glassesToDrink = ui->toDrink->value() / ui->glassVolume->value();
    drinkInterval = (ui->awakeHours->value() * 60) / glassesToDrink;

    if(drinkInterval == 0) {
        QMessageBox::warning(this, "Error", "Drink interval is too low");
        return;
    }

    startDateTime = QDateTime::currentDateTime();

    ui->labelInterval->setText(QString("You need to drink a glass of water evey %1 minutes").arg(drinkInterval));

    ui->labelStarted->setText(QString("Started at %1").arg(startDateTime.toString()));
    refreshGlassTimes();

    started = true;

    qDebug() << "Starting" << startDateTime << "To Drink" << glassesToDrink << "Interval" << drinkInterval << "Hours awake" << ui->awakeHours->value() << "To drink" << ui->toDrink->value() << "Glass volume" << ui->glassVolume->value();
}


void WaterReminder::on_bDrankGlass_clicked()
{
    if(!started) return;
    qDebug("Drank Glass");

    glassesToDrink--;

    drankGlasses++;
    ui->labelDrankGlasses->setText(QString("Drank %1 Glasses").arg(drankGlasses));

    refreshGlassTimes();
}

void WaterReminder::refreshGlassTimes()
{
    glassTimes.clear();
    glassTimes.append(QDateTime::currentDateTime().addSecs(drinkInterval * 60));
    for(int i = 0; i < glassesToDrink; i++) {
        glassTimes.append(glassTimes[i].addSecs(drinkInterval * 60));
    }
}


