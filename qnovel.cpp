#include "qnovel.h"

QNovel::QNovel() : name(""), author(""), host(""), url("")
{

}

QNovel::QNovel(QString name, QString author, QString host, QString url)
{
    this->name = name;
    this->author = author;
    this->host = host;
    this->url = url;
}
