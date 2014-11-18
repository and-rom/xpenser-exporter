#include <QtGui>
#include <QtNetwork>

#include "mainwindow.h"
#include "xmlhandler.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    widget= new QWidget(this);
    urlLineEdit = new QLineEdit("http://xpenser.com/api/v1.0/");
    urltext=urlLineEdit->text();

    urlLabel = new QLabel(tr("&URL:"));
    urlLabel->setBuddy(urlLineEdit);
    statusLabel = new QLabel(tr("Please enter the URL of a file you want to "
                                "download."));
    textedit=new QTextEdit;


    getExpensesButton = new QPushButton(tr("Expenses"));
    getReportsButton = new QPushButton(tr("Reports"));
    quitButton = new QPushButton(tr("Quit"));
    deleteButton = new QPushButton(tr("Delete"));
    exportButton = new QPushButton(tr("Export current"));
    exportButton->setEnabled(false);

    buttonBox = new QDialogButtonBox;
    buttonBox->addButton(getExpensesButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(getReportsButton, QDialogButtonBox::ApplyRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);


    //tableWidget->setRowCount(10);
    //tableWidget->setColumnCount(3);
    //tableWidget->setHorizontalHeaderLabels(labels);


    connect(urlLineEdit, SIGNAL(textChanged(QString)),this, SLOT(enablegetExpensesButton()));

    connect(getExpensesButton, SIGNAL(clicked()), this, SLOT(slotGetExpenses()));
    connect(getReportsButton, SIGNAL(clicked()), this, SLOT(slotGetReports()));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(exportButton, SIGNAL(clicked()), this, SLOT(exportcurrent()));
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(deletemarked()));

    connect(&Req, SIGNAL(setLabel(QString)), this, SLOT(setLabel(QString)));
    connect(&Req, SIGNAL(setButtonStatus(bool)), this, SLOT(setButtonStatus(bool)));

    connect(this, SIGNAL(signalStart(QString,QString,QString)), &Req, SLOT(download(QString,QString,QString)));
    connect(&Req,SIGNAL(sendReply(QString)),this,SLOT(getReply(QString)));

    topLayout = new QHBoxLayout;
    topLayout->addWidget(urlLabel);
    topLayout->addWidget(urlLineEdit);

    centralLayout =new QHBoxLayout;
    tabWidget=new QTabWidget;
    centralLayout->addWidget(tabWidget);

    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(currenttab(int)));

    mainLayout = new QVBoxLayout;
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(statusLabel);
    mainLayout->addLayout(centralLayout);
    //mainLayout->addWidget(textedit);
    mainLayout->addWidget(exportButton);
    mainLayout->addWidget(deleteButton);
    mainLayout->addWidget(buttonBox);
    widget->setLayout(mainLayout);
    setCentralWidget(widget);

    setWindowTitle(tr("Xpenser CSV Exporter"));
    urlLineEdit->setFocus();
    iii=0;



}

MainWindow::~MainWindow()
{

}
void MainWindow::enablegetExpensesButton()
{
    getExpensesButton->setEnabled(!urlLineEdit->text().isEmpty());
    urltext=urlLineEdit->text();
}
void MainWindow::setLabel(QString strLabel)
{
    statusLabel->setText(strLabel);
}
void MainWindow::setButtonStatus(bool status)
{
    if (type=="expense") {
        getExpensesButton->setEnabled(status);
    } else if (type=="reports") {
        getReportsButton->setEnabled(status);
    }

}
void MainWindow::slotGetExpenses()
{
    tabWidget->clear();
    type="expenses";
    emit signalStart(urltext+"expenses/?format=xml","get","");
    exportButton->setEnabled(true);
}
void MainWindow::GetAllExpenses()
{
    type="123";
    if (reports.size()>0){
        report=reports[0];
        reports.pop_front();
        QString tmp=urltext+"expenses/?report="+report.id+"&format=xml";
        emit signalStart(tmp,"get","");
    } else {type="";}

}
void MainWindow::slotGetReports()
{
    tabWidget->clear();
    type="reports";
    emit signalStart(urltext+"reports/?format=xml","get","");
    exportButton->setEnabled(true);
}
void MainWindow::getReply(QString replytext)
{
    Reply=replytext;
    textedit->setText(Reply);
    if (type=="delete") {
        deletemarked();
    } else {
        parseXmlReply();
    }

}
void MainWindow::parseXmlReply()
{
    XbelHandler handler;
    QXmlSimpleReader reader;
    reader.setContentHandler(&handler);
    reader.setErrorHandler(&handler);
    QXmlInputSource xmlInputSource;
    xmlInputSource.setData(Reply);
    if (!reader.parse(xmlInputSource))
        QMessageBox::information(this, tr("XML"),
                                 tr("Failed to load xml reply!"));
    response=handler.getResponse();
    response.type=type;
    nresponse=handler.getnResponse();
    nresponse.type=type;
    if (type=="expenses") {
        tableWidget = new QTableWidget;
        tabWidget->addTab(tableWidget,"Default");
        displayExpenses(tableWidget);
    } else if (type=="reports") {
        displayReports();
    } else if (type=="123") {
        tableWidget = new QTableWidget;
        tableWidget->setAccessibleName(report.id);
        if (report.isdefault) {
            tableWidget->setFocus();
            tabWidget->insertTab(0,tableWidget,report.name);
        }
        else {
            tabWidget->addTab(tableWidget,report.name);
        }
        displayExpenses(tableWidget);
      tabWidget->setCurrentIndex(0);
    }

}
void MainWindow::displayExpenses(QTableWidget *tableWidget)
{
    tableWidget->setColumnCount(nresponse.data.size());
    tableWidget->setRowCount(0);
    QString key;
    int i=0;
    foreach (key, nresponse.data.keys()) {
        tableWidget->setHorizontalHeaderItem(i, new QTableWidgetItem(key));
        if (tableWidget->rowCount()<nresponse.data[key].size()) {
            tableWidget->setRowCount(nresponse.data[key].size());
            }
        for (int j = 0; j < nresponse.data[key].size(); ++j) {
            QTableWidgetItem* item = new QTableWidgetItem(nresponse.data[key][j]);
            if (i==0) {
                item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
                item->setCheckState(Qt::Unchecked);
            }
            tableWidget->setItem(j, i, item);

        }
i++;
    }

    if (type=="123") { GetAllExpenses();}
}
void MainWindow::displayReports()
{
    for (int i = 0; i < nresponse.data["id"].size(); ++i) {
        reports.resize(reports.size()+1);
        if (i<nresponse.data["is_default"].size())
        {
            if (nresponse.data["is_default"][i]!="")
            {
                reports[i].isdefault=true;
            }
        } else reports[i].isdefault=false;
        reports[i].id=nresponse.data["id"][i];
        reports[i].name=nresponse.data["name"][i];
    }
    emit GetAllExpenses();
}
void MainWindow::currenttab(int tab)
{
    ctab=tab;
    QWidget *widget=tabWidget->widget(tab);
    tableWidget = qobject_cast<QTableWidget *>(widget);

}
void MainWindow::exportcurrent()
{
    QFile f("money.csv");

    if (f.open(QFile::WriteOnly))
    {
        QTextStream data(&f);
        QStringList strList;

        for( int r = 0; r < tableWidget->rowCount(); ++r )
        {
            strList.clear();
            for( int c = 0; c < tableWidget->columnCount(); ++c )
            {
                strList << "\""+tableWidget->item( r, c )->text()+"\"";
            }
            data << strList.join( "," )+"\n";
        }
        f.close();
    }
}
void MainWindow::deletemarked()
{
    type="delete";
    if (iii<tableWidget->rowCount()) {
        Qt::CheckState st=tableWidget->item(iii,0)->checkState();
        if (st) {
            //QMessageBox::information(this, tr("XML"),tableWidget->item(i,2)->text());
            emit signalStart(urltext+"expense/"+tableWidget->item(iii,2)->text()+"?action=delete","post","");
        }
        iii++;
    } else {
        type="";

    }
}
