#include "qsearchhistory.h"

QSearchHistory::QSearchHistory()
{

}

QSearchHistory::QSearchHistory(QString searchTime, QString keyWard, QUrl host)
{
    this->searchTime = searchTime;
    this->keyWard = keyWard;
    this->host = host;
}
