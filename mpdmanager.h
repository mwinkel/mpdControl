#ifndef MPDMANAGER_H
#define MPDMANAGER_H

#include <QTcpSocket>
#include <QStringList>

#include "mpdplaylistentry.h"

enum
{
     UNKNOWN = 0,
     CONNECT = 1,
     DISCONNECT = 2,
     PARSE_PLAYLIST = 3,
     PARSE_STATUS = 4,
     PARSE_CURRENTSONG = 5
};

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
        void getCurrentSong();
        void getPlaylist();
        void getCurrentVolume();
        void readServerResponse();

    private slots:
        void send_command(QString cmd);

    private:
        QTcpSocket *mpd_socket;
        int mpd_state;

    signals:
        void volChanged(int);
        void songUpdate(QString);
        void playlistUpdate(QList<MpdPlaylistEntry*>);
};

#endif // MPDMANAGER_H
