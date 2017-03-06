#ifndef QSEARCHHISTORY_H
#define QSEARCHHISTORY_H

#include <QObject>
#include <QUrl>

class QSearchHistory
{
public:
    QSearchHistory();
    QSearchHistory(QString searchTime, QString keyWard, QUrl host);
    QString searchTime;
    QString keyWard;
    QUrl host;

};

#endif // QSEARCHHISTORY_H
