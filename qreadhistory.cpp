#include "qreadhistory.h"

QReadHistory::QReadHistory() : readTime(""), novelName(""), novelAuthor(""), chapterId(-1), chapterName("")
{

}

QReadHistory::QReadHistory(QString readTime, QString novelName, QString novelAuthor, QUrl novelUrl, int chapterId, QString chapterName)
{
    this->readTime = readTime;
    this->novelName = novelName;
    this->novelAuthor = novelAuthor;
    this->novelUrl = novelUrl;
    this->chapterId = chapterId;
    this->chapterName = chapterName;
}
