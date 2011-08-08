#include "mainwindow.h"
#include "mpdmanager.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <QtCore/QCoreApplication>
#include <QTimer>
#include <QSettings>




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_pMenu = new QMenu(this);
//    m_pMenu->addAction("Example", this, SLOT(exampleSlot()));

    // Create Options CBA
    QAction *pOptions = new QAction("Options", this);
    // Set defined menu into Options button
    pOptions->setMenu(m_pMenu);
    pOptions->setSoftKeyRole(QAction::PositiveSoftKey);
    addAction(pOptions);

    // Create Exit CBA
    QAction *pExitButton = new QAction(QString("Exit"), this);
    pExitButton->setSoftKeyRole(QAction::NegativeSoftKey);
    // Exit button closes the application
    QObject::connect(pExitButton, SIGNAL(triggered()),
    QApplication::instance(), SLOT(quit()));
    addAction(pExitButton);


    QCoreApplication::setOrganizationName("-foo-");
//    QCoreApplication::setOrganizationDomain("www._noch_leer_.com");
    QCoreApplication::setApplicationName("mpdControl");
    QSettings settings;

    // letzte Verbindungsdaten laden ...
    int mpd_port = settings.value("port").toInt();
    QString mpd_host = settings.value("host").toString();

    // default values
    if (mpd_port == 0){ mpd_port = 6600; }
    if (mpd_host == NULL){ mpd_host = "localhost"; }

    this->ui->options_host->setText(mpd_host);
    this->ui->options_port->setText(QString::number(mpd_port));

    mpdmanager = new MpdManager();
    mpdmanager->mpdConnect(mpd_host, mpd_port);
    connect(mpdmanager, SIGNAL(songUpdate(QString,QString)), this, SLOT(updateTitle(QString,QString)));
    connect(mpdmanager, SIGNAL(playlistUpdate(QList<MpdPlaylistEntry*>)), this, SLOT(updatePlaylist(QList<MpdPlaylistEntry*>)));
    connect(mpdmanager, SIGNAL(volChanged(int)), this->ui->volumeSlider, SLOT(setValue(int)));
    connect(ui->btnConnect, SIGNAL(clicked()), mpdmanager, SLOT(toggelPause()));
    connect(ui->btnPlay, SIGNAL(clicked()), mpdmanager, SLOT(start()));
    connect(ui->btnStop, SIGNAL(clicked()), mpdmanager, SLOT(stop()));
    connect(ui->btnNext, SIGNAL(clicked()), mpdmanager, SLOT(next()));
    connect(ui->btnPrevious, SIGNAL(clicked()), mpdmanager, SLOT(previous()));
    connect(ui->volumeSlider, SIGNAL(valueChanged(int)), mpdmanager, SLOT(setVolume(int)));
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), mpdmanager, SLOT(getCurrentSong()));
    timer->start(1000);

    connect(this->ui->listWidget_playlist, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(playlistEntryClicked(QListWidgetItem*)));
}

MainWindow::~MainWindow()
{
    mpdmanager->mpdDisconnect();
    this->saveSettings();
    delete ui;
}

void MainWindow::setOrientation(ScreenOrientation orientation)
{
#if defined(Q_OS_SYMBIAN)
    // If the version of Qt on the device is < 4.7.2, that attribute won't work
    if (orientation != ScreenOrientationAuto) {
        const QStringList v = QString::fromAscii(qVersion()).split(QLatin1Char('.'));
        if (v.count() == 3 && (v.at(0).toInt() << 16 | v.at(1).toInt() << 8 | v.at(2).toInt()) < 0x040702) {
            qWarning("Screen orientation locking only supported with Qt 4.7.2 and above");
            return;
        }
    }
#endif // Q_OS_SYMBIAN

    Qt::WidgetAttribute attribute;
    switch (orientation) {
#if QT_VERSION < 0x040702
    // Qt < 4.7.2 does not yet have the Qt::WA_*Orientation attributes
    case ScreenOrientationLockPortrait:
        attribute = static_cast<Qt::WidgetAttribute>(128);
        break;
    case ScreenOrientationLockLandscape:
        attribute = static_cast<Qt::WidgetAttribute>(129);
        break;
    default:
    case ScreenOrientationAuto:
        attribute = static_cast<Qt::WidgetAttribute>(130);
        break;
#else // QT_VERSION < 0x040702
    case ScreenOrientationLockPortrait:
        attribute = Qt::WA_LockPortraitOrientation;
        break;
    case ScreenOrientationLockLandscape:
        attribute = Qt::WA_LockLandscapeOrientation;
        break;
    default:
    case ScreenOrientationAuto:
        attribute = Qt::WA_AutoOrientation;
        break;
#endif // QT_VERSION < 0x040702
    };
    setAttribute(attribute, true);
}

void MainWindow::showExpanded()
{
#ifdef Q_OS_SYMBIAN
    showFullScreen();
#elif defined(Q_WS_MAEMO_5)
    showMaximized();
#else
    show();
#endif
}

void MainWindow::updateTitle(QString state, QString songTitle){
    ui->label_state->setText(state);
    ui->label_currentsong->setText(songTitle);
}

void MainWindow::updatePlaylist(QList<MpdPlaylistEntry*> playlist){
    ui->listWidget_playlist->clear();

    for (int i = 0; i < playlist.size(); ++i){
        QListWidgetItem *item = new QListWidgetItem(ui->listWidget_playlist);
        item->setData(Qt::DisplayRole, playlist.at(i)->title);
        item->setData(Qt::UserRole + 1, playlist.at(i)->pos);

        ui->listWidget_playlist->addItem(item);
    }
}

void MainWindow::saveSettings(){
    QString mpd_host = this->ui->options_host->text();
    int mpd_port = this->ui->options_port->text().toInt();

    QSettings settings;
    settings.setValue("host", mpd_host);
    settings.setValue("port", mpd_port);
}

void MainWindow::on_options_connect_and_save_clicked()
{
    this->saveSettings();
    // TODO
    mpdmanager->mpdConnect(ui->options_host->text(),ui->options_port->text().toInt());
}
void MainWindow::on_tabWidget_currentChanged(int index)
{
    if (index==1)
        mpdmanager->getPlaylist();
}

void MainWindow::playlistEntryClicked(QListWidgetItem* item){
    if (item != NULL){
        int songpos = item->data(Qt::UserRole + 1).toInt();
        mpdmanager->play(songpos);
    }
}
