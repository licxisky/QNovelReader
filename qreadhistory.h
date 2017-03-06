#ifndef QREADHISTORY_H
#define QREADHISTORY_H

#include <QObject>
#include <QUrl>

class QReadHistory
{
public:
    QReadHistory();
    QReadHistory(QString readTime, QString novelName, QString novelAuthor, QUrl novelUrl, int chapterId, QString chapterName);
    QString readTime;  //format: yyyy-MM-dd hh:mm:ss ddd
    QString novelName;
    QString novelAuthor;
    QUrl novelUrl;
    int chapterId;
    QString chapterName;
};

#endif // QREADHISTORY_H
