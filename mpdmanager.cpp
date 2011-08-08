#include "mpdmanager.h"

#include <iostream>
#include <QTimer>


MpdManager::MpdManager()
{
    mpd_socket = NULL;
    mpd_state = UNKNOWN;
}


MpdManager::~MpdManager()
{
}

void MpdManager::mpdConnect(QString host, int port){
    mpd_socket = new QTcpSocket(this);
    connect(mpd_socket, SIGNAL(readyRead()), this, SLOT(readServerResponse()));
    mpd_socket->connectToHost(host, port);
    mpd_state = CONNECT;

    // TODO: geht das noch hübscher?
    QTimer *timer = new QTimer(this);
    timer->singleShot(1500, this, SLOT(getCurrentVolume()));
}

void MpdManager::mpdDisconnect(){
    if (mpd_socket != NULL)
        mpd_state = DISCONNECT;
        mpd_socket->close();
}

void MpdManager::toggelPause(){
    mpd_state = PARSE_CURRENTSONG;
    send_command("pause");
}

void MpdManager::start(){
    mpd_state = PARSE_CURRENTSONG;
    send_command("play");
}

void MpdManager::play(int songpos){
    mpd_state = PARSE_CURRENTSONG;
    send_command("play " + QString::number(songpos));
}

void MpdManager::stop(){
    mpd_state = PARSE_CURRENTSONG;
    send_command("stop");
}

void MpdManager::next(){
    mpd_state = PARSE_CURRENTSONG;
    send_command("next");
}

void MpdManager::previous(){
    mpd_state = PARSE_CURRENTSONG;
    send_command("previous");
}

void MpdManager::setVolume(int volume){
    send_command("setvol " + QString::number(volume));
}

void MpdManager::getCurrentSong(){
    mpd_state = PARSE_CURRENTSONG;
    send_command("status\ncurrentsong");
}

void MpdManager::getPlaylist(){
    mpd_state = PARSE_PLAYLIST;
    send_command("playlistinfo");
}

void MpdManager::getCurrentVolume(){
    mpd_state = PARSE_STATUS;
    send_command("status");
}

void MpdManager::send_command(QString cmd){
    if (mpd_socket->state() == QAbstractSocket::ConnectedState){
        QTextStream os(mpd_socket);
        os << tr("command_list_ok_begin") << "\n";
        os << cmd << "\n";
        os << tr("command_list_end") << "\n";
        os.flush();
    }
}

void MpdManager::readServerResponse(){
    QTextStream in(mpd_socket);
    in.setCodec("UTF-8");

    QString line;
    QString name = "";
    QString title = "";
    QString artist = "";
    QString state = "";

    QList<MpdPlaylistEntry*> playlist;
    QString playlist_title = "";
    int playlist_pos = -1;
    int playlist_song_id = -1;

    do {
        line = in.readLine();

        if (mpd_state == PARSE_STATUS){
            if (line.startsWith("volume:")){
                int volume = line.replace("volume: ", "").toInt();
                emit volChanged(volume);
            }
        }
        else
        if (mpd_state == PARSE_PLAYLIST){
            // (file, Pos, Id)

            if (line.startsWith("file:"))
                playlist_title = line.replace("file: ", "");
            if (line.startsWith("Pos:"))
                playlist_pos = line.replace("Pos: ", "").toInt();
            if (line.startsWith("Id:"))
                playlist_song_id = line.replace("Id: ", "").toInt();

            if (playlist_song_id != -1){
                MpdPlaylistEntry *entry = new MpdPlaylistEntry(playlist_title, playlist_song_id, playlist_pos);
                playlist.append(entry);

                playlist_song_id = -1;
            }
        }
        else
        if (mpd_state == PARSE_CURRENTSONG){
            if (line.contains("Name: "))
                name = line.replace("Name: ", "");
            if (line.contains("Title: "))
                title = line.replace("Title: ", "");
            if (line.contains("Artist: "))
                artist = line.replace("Artist: ", "");
            if (line.contains("state: "))
                state = line.replace("state: ", "");
        }

    } while(!line.isNull());

    if ( mpd_state == PARSE_CURRENTSONG &&
         (!artist.isEmpty() || !title.isEmpty() || !name.isEmpty()) ){
        // TODO
        emit songUpdate("[" + state + "]", artist + " - " + title + " (" + name + ")");
    }


    if ( mpd_state == PARSE_PLAYLIST && playlist.length() > 0){
        emit playlistUpdate(playlist);
    }

    mpd_state = PARSE_CURRENTSONG;
}
