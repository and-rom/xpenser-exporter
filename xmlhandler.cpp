#include <QtGui>
#include "xmlhandler.h"

XbelHandler::XbelHandler()
{

    //response.data.clear();
    //item = 0;
    i=0;
}

bool XbelHandler::startElement(const QString & /* namespaceURI */,
                               const QString & /* localName */,
                               const QString &qName,
                               const QXmlAttributes &attributes)
{
    if (qName=="resource") {
            response.data.resize(response.data.size()+1);
    }
    currentText.clear();
    return true;
}

bool XbelHandler::endElement(const QString & /* namespaceURI */,
                             const QString & /* localName */,
                             const QString &qName)
{
    if (qName=="date") {
        QStringList tmp=currentText.split(" ");
        currentText=tmp.first();
    }
    if (qName!="response") {
        if (qName!="resource") {
            if (qName!="modified" &
                    qName!="status" &
                    qName!="tags" &
                    qName!="notes" &
                    qName!="receipt" &
                    qName!="report_id" &
                    qName!="rawamount" &
                    qName!="created" &
                    qName!="image") {
                response.data[i].insert(qName,currentText);
                nresponse.data[qName].resize(i+1);
                nresponse.data[qName][i]=currentText;
            }
        } else {
            i++;
        }
    }
    /*if (qName!="response" & qName!="resource") {
        if (nresponse.data.contains(qName)) {
            nresponse.data[qName].resize(nresponse.data[qName].size()+1);
            nresponse.data[qName].push_back(currentText);
        } else {
            nresponse.data[qName].resize(1);
            nresponse.data[qName].push_back(currentText);

        }
    }

    if (qName == "date") {
        // удалить время в     currentText.split(" ");
        QTableWidgetItem* item = new QTableWidgetItem;
        item->setText(currentText);
        tableWidget->setItem(i, 0, item);
    } else if (qName == "type") {
        QTableWidgetItem* item = new QTableWidgetItem;
        item->setText(currentText);
        tableWidget->setItem(i, 1, item);
    } else if (qName == "amount") {
        QTableWidgetItem* item = new QTableWidgetItem;
        item->setText(currentText);
        tableWidget->setItem(i, 2, item);
    } else if (qName == "resource") {
        i++;
    }
    */
    return true;
}

bool XbelHandler::characters(const QString &str)
{
    currentText += str;
    return true;
}

bool XbelHandler::fatalError(const QXmlParseException &exception)
{
    QMessageBox::information(0, QObject::tr("SAX Bookmarks"),
                             QObject::tr("Parse error at line %1, column %2:\n"
                                         "%3")
                             .arg(exception.lineNumber())
                             .arg(exception.columnNumber())
                             .arg(exception.message()));
    return false;
}

QString XbelHandler::errorString() const
{
    return errorStr;
}
tResponse XbelHandler::getResponse()
{
    return response;
}
tnResponse XbelHandler::getnResponse()
{
    return nresponse;
}
