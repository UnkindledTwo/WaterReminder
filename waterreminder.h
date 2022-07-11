#ifndef WATERREMINDER_H
#define WATERREMINDER_H

#include <QMainWindow>
#include <QDateTime>
#include <QTimer>
#include <QDebug>
#include <QVector>
#include <QSound>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class WaterReminder; }
QT_END_NAMESPACE

class WaterReminder : public QMainWindow
{
    Q_OBJECT

public:
    WaterReminder(QWidget *parent = nullptr);
    ~WaterReminder();

private slots:
    void on_bCalculate_clicked();

    void timerTick();

    void on_bDrankGlass_clicked();

    void refreshGlassTimes();

    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);

protected:
    void closeEvent(QCloseEvent *e);
private:
    Ui::WaterReminder *ui;

    QDateTime startDateTime;

    QTimer *timer;

    QVector<QDateTime> glassTimes;

    int drankGlasses = 0;
    int glassesToDrink = 0;
    int drinkInterval = 0;

    bool started = false;

    QSound* notif;

    QSystemTrayIcon *trayIcon;

    void initTrayMenu();
};
#endif // WATERREMINDER_H
