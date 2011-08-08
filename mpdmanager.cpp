#include "mpdmanager.h"

#include <iostream>
#include <QTimer>


MpdManager::MpdManager()
{
    mpd_socket = NULL;
}


MpdManager::~MpdManager()
{
}

void MpdManager::mpdConnect(QString host, int port){
    mpd_socket = new QTcpSocket(this);
    connect(mpd_socket, SIGNAL(readyRead()), this, SLOT(readServerResponse()));
    mpd_socket->connectToHost(host, port);

    // TODO: geht das noch hübscher?
    QTimer *timer = new QTimer(this);
    timer->singleShot(2000, this, SLOT(getCurrentVolume()));
}

void MpdManager::mpdDisconnect(){
    if (mpd_socket != NULL)
        mpd_socket->close();
}

void MpdManager::toggelPause(){
    send_command("pause");
}

void MpdManager::start(){
    send_command("play");
}

void MpdManager::stop(){
    send_command("stop");
}

void MpdManager::next(){
    send_command("next");
}

void MpdManager::previous(){
    send_command("previous");
}

void MpdManager::setVolume(int volume){
    send_command("setvol " + QString::number(volume));
}

void MpdManager::getCurrentSong(){
    send_command("currentsong");
}

void MpdManager::getPlaylist(){
    send_command("playlistinfo");
}

void MpdManager::getCurrentVolume(){
    send_command("status");
}

void MpdManager::send_command(QString cmd){
    if (mpd_socket->state() == QAbstractSocket::ConnectedState){
        QTextStream os(mpd_socket);
        os << tr("command_list_ok_begin") << "\n";
        os << cmd << "\n";
        os << tr("command_list_end") << "\n";
    }
}

void MpdManager::readServerResponse(){
    QTextStream in(mpd_socket);
    in.setCodec("UTF-8");

    QString line;
    QString name = "";
    QString title = "";
    QString artist = "";

    // TODO: song klasse oder sowas mit title, id/songpos
    QStringList files;

    do {
        line = in.readLine();

        if (line.startsWith("file:")){
            files << line; // .replace("file: ", "");
        }

        if (line.startsWith("volume:")){
            int volume = line.replace("volume: ", "").toInt();
            emit volChanged(volume);
        }

        if (line.contains("Name: "))
            name = line.replace("Name: ", "");
        if (line.contains("Title: "))
            title = line.replace("Title: ", "");
        if (line.contains("Artist: "))
            artist = line.replace("Artist: ", "");

    } while(!line.isNull());

    if ( !name.isEmpty() || !title.isEmpty() || !artist.isEmpty() )
        emit songUpdate(artist + " - " + title + " (" + name + ")");


    if ( files.length() > 1 )
        emit playlistUpdate(files);
}




