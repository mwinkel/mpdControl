#ifndef MPDPLAYLISTENTRY_H
#define MPDPLAYLISTENTRY_H

#include <QObject>

class MpdPlaylistEntry : public QObject
{
    Q_OBJECT

    public:
        MpdPlaylistEntry();
        MpdPlaylistEntry(QString title, int songid, int pos);
        ~MpdPlaylistEntry();


        QString title;
        int songid;
        int pos;

    signals:

    public slots:

};

#endif // MPDPLAYLISTENTRY_H
