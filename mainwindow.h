#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QStringList>
#include "mpdmanager.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    enum ScreenOrientation {
        ScreenOrientationLockPortrait,
        ScreenOrientationLockLandscape,
        ScreenOrientationAuto
    };

    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

    // Note that this will only have an effect on Symbian and Fremantle.
    void setOrientation(ScreenOrientation orientation);

    void showExpanded();

private slots:
    void updateTitle(QString songTitle);
    void updatePlaylist(QStringList playlist);
    void saveSettings();
    void on_options_connect_and_save_clicked();


    void on_tabWidget_currentChanged(int index);

private:
    Ui::MainWindow *ui;
    QMenu* m_pMenu;
    MpdManager *mpdmanager;

};

#endif // MAINWINDOW_H
