#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QFile>
#include "httprequest.h"
#include "typedef.h"

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QFile;
class QLabel;
class QLineEdit;
class QPushButton;
class QTextEdit;
class QTableWidget;
class QHBoxLayout;
class QVBoxLayout;
class QTabWidget;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    httprequest Req;
    tResponse response;
    tnResponse nresponse;
    tRrepots reports;
    tReport report;
    QString Reply;
    QString urltext;
    QString type;
    QString prefix;
    QWidget *widget;
    QLabel *statusLabel;
    QLabel *urlLabel;
    QLineEdit *urlLineEdit;
    QPushButton *getExpensesButton;
    QPushButton *getReportsButton;
    QPushButton *quitButton;
    QPushButton *exportButton;
    QPushButton *deleteButton;
    QDialogButtonBox *buttonBox;
    QTextEdit *textedit;
    QTableWidget *tableWidget;
    QHBoxLayout *centralLayout;
    QVBoxLayout *mainLayout;
    QHBoxLayout *topLayout;
    QTabWidget *tabWidget;
    int ctab;
    int iii;
    void displayExpenses(QTableWidget *tableWidget);
    void displayExpense();
    void displayReports();
    void parseXmlReply();
    void GetAllExpenses();


public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void enablegetExpensesButton();
    void slotGetExpenses();
    void slotGetReports();
    void exportcurrent();
    void currenttab(int);
    void deletemarked();
public slots:
    void setLabel(QString);
    void setButtonStatus(bool);
    void getReply(QString);

signals:
    void signalStart(QString,QString,QString);
};

#endif // MAINWINDOW_H
