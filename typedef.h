#include <QtCore>
#include <QTypeInfo>
#include <QtGui>
#ifndef TYPEDEF_H
#define TYPEDEF_H
typedef QHash<QString,QString> record;
struct tResponse
{
    QString type; //expense or report
    QVector<record> data;
};
typedef QVector<QString> nrecord;
struct tnResponse
{
    QString type; //expense or report
    QHash<QString,nrecord> data;
};
struct tReport
{
    QString id;
    QString name;
    bool isdefault;
};
typedef QVector<tReport> tRrepots;
#endif // TYPEDEF_H
