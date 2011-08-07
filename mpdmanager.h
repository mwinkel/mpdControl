#ifndef MPDMANAGER_H
#define MPDMANAGER_H

#include <QTcpSocket>
//#include <Qt/qtcpsocket.h>

class MpdManager : public QObject
{
    Q_OBJECT

    public:
        MpdManager();
        ~MpdManager();

        void mpdConnect(QString host, int port);
        void mpdDisconnect();

    public slots:
        void toggelPause();
        void start();
        void stop();
        void next();
        void previous();
        void setVolume(int volume);
        QString getCurrentSong();
        void getCurrentVolume();
        QString getPlaylist();

    private slots:
        void send_command(QString cmd);

    private:
        QTcpSocket *mpd_socket;

    signals:
        void volChanged(int);
};

#endif // MPDMANAGER_H
