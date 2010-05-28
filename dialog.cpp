#include "dialog.h"
#include "ui_dialog.h"

#include <QFileDialog>
#include <QTextStream>
#include <QDebug>

#include "fileloader.h"


//
// Constructor
//
Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    FileLoader* fileLoader = new FileLoader(this);

    ui->setupUi(this);

    QObject::connect(this, SIGNAL(filesSelected(QStringList)), fileLoader, SLOT(loadFiles(QStringList)));
    QObject::connect(fileLoader, SIGNAL(fileLoaded(FileLoader*)), this, SLOT(showFiles(FileLoader*)));
    QObject::connect(this, SIGNAL(runclicked(QString)), fileLoader, SLOT(createFlashCards(QString)));
    QObject::connect(fileLoader, SIGNAL(fileLoaderError(QString)), this, SLOT(handleFileLoaderError(QString)));

    setWindowIcon(QIcon(":/SimpleFlashCard/icon_png"));
    setWindowTitle("SimpleFlashCard creator");
}



//
// Destructor
//
Dialog::~Dialog()
{
    delete ui;
}

//
// changeEvent
//
void Dialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

//
// Add... button clicked
//
void Dialog::on_btnBrowseTSVFiles_clicked()
{
    qDebug() << "Dialog::on_btnBrowseTSVFiles_clicked() start";

    QStringList fileNames = QFileDialog::getOpenFileNames(this, "Open TSV files", "", "TSV file (*.tsv)");

    if (fileNames.size())
    {
        m_strDstPath = fileNames.at(0);
        m_strDstPath = m_strDstPath.left(m_strDstPath.lastIndexOf("/"));

        qDebug() << "Dialog::on_btnBrowseTSVFiles_clicked() files selected";

        qDebug () << "m_strDstPath == " << m_strDstPath;

        emit filesSelected(fileNames);
    }

    qDebug() << "Dialog::on_btnBrowseTSVFiles_clicked() end";
}

//
// Create the flash cards
//
void Dialog::createFlashCard()
{
    writeFlashCard();
}


//
// Write the flash cards
//
void Dialog::writeFlashCard()
{

}

#include <QResource>
#include <QTextStream>



//
// debug
//
void Dialog::on_pushButton_clicked()
{
    QFile file(":/SimpleFlashCard/info_plist");


    qDebug() << "file.size = " << file.size();

}


void Dialog::showFiles(FileLoader* fileLoader)
{
    qDebug() << "Dialog::showFiles start";

    QTableView* table = ui->tableView;  // just for readability

    table->setModel(fileLoader);
    table->horizontalHeader()->hide();
    table->verticalHeader()->hide();

    table->setColumnWidth(0, table->width());
    table->horizontalHeader()->setResizeMode(QHeaderView::Fixed);

    for (int i = 0; i < table->model()->rowCount(); i++){
        table->setRowHeight(i, 20);
    }

    qDebug() << "Dialog::showFiles end";
}


#include <QMessageBox>

void Dialog::handleFileLoaderError(QString err)
{
    QMessageBox msgBox;
    msgBox.setText(err);
    msgBox.exec();
}



void Dialog::on_btnRun_clicked()
{
    QString dst = QFileDialog::getExistingDirectory(this, "Select Destination Folder", m_strDstPath);

    if (dst.size()){
        emit runclicked(dst);
    }
}
