#include "mpdplaylistentry.h"

#include <iostream>


MpdPlaylistEntry::MpdPlaylistEntry()
{

}

MpdPlaylistEntry::MpdPlaylistEntry(
    QString title, int songid, int pos)
{
    this->title = title;
    this->songid = songid;
    this->pos = pos;
}

MpdPlaylistEntry::~MpdPlaylistEntry()
{
}
