#ifndef QNOVEL_H
#define QNOVEL_H

#include <QObject>

#include "qchapter.h"

class QNovel
{
public:
    QNovel();
    QNovel(QString, QString, QString, QString);
    QString name;
    QString author;
    QString host;
    QString url;
    QList<QChapter> chapters;
};

#endif // QNOVEL_H
