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
    mpd_socket->connectToHost(host, port);

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
//    std::cout << "pl: " << qPrintable(this->getPlaylist()) << std::endl;
}

QString MpdManager::getCurrentSong(){
    send_command("currentsong");
    QTextStream in(mpd_socket);
    in.setCodec("UTF-8");

    QString name = "";
    QString title = "";
    QString artist = "";

    QString line;
    do {
        line = in.readLine();
        if (line.contains("Name: "))
            name = line.replace("Name: ", "");
        if (line.contains("Title: "))
            title = line.replace("Title: ", "");
        if (line.contains("Artist: "))
            artist = line.replace("Artist: ", "");
    } while (!line.isNull());

    return artist + " - " + title;
}

void MpdManager::getCurrentVolume(){
    send_command("status");
    QTextStream in(mpd_socket);
    in.setCodec("UTF-8");

    int vol = 0;

    QString line;
    do {
        line = in.readLine();
        if (line.contains("volume: "))
            vol = line.replace("volume: ", "").toInt();
    } while (!line.isNull());

    emit volChanged(vol);
}

void MpdManager::send_command(QString cmd){
    QTextStream os(mpd_socket);
    os.flush();
    os << tr("command_list_ok_begin") << "\n";
    os << tr("status") << "\n";
    os << cmd << "\n";
    os << tr("command_list_end") << "\n";
    os.flush();
}




