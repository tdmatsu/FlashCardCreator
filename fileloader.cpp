#include "fileloader.h"

#include <QDebug>
#include <QFile>
#include <QDir>

const QString OUTPUT_FOLDER_NAME = "Output";
const QString OUTPUT_WIDGET_FOLDER_NAME = "SimpleFlashCard";

const QString OUTPUT_WIDGET_STYLE_FOLDER_NAME = "style";
const QString OUTPUT_WIDGET_SCRIPT_FOLDER_NAME = "script";
const QString OUTPUT_WIDGET_DATA_FOLDER_NAME = "data";

const QString FILENAME_FILELIST_JS = "filelist.js";
const QString FILENAME_RENAME_BAT = "rename.bat";


FileLoader::FileLoader(QObject * parent)
    : QAbstractTableModel(parent)
{

}

void FileLoader::loadFiles(QStringList fileNames)
{
    qDebug() << "FileLoader::loadFiles start";

    Q_ASSERT(!fileNames.isEmpty());

    for (int i = 0; i < fileNames.size(); i++){
        qDebug () << fileNames[i];


        // read the CSV data
        QFile file(fileNames[i]);

        // open the file
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
            qDebug() << "failed to open input file";
        } else {

            qDebug() << "reading file... " << fileNames[i];

            QStringList lstEntries;

            while (!file.atEnd()){
                QString line = QString::fromUtf8(file.readLine().constData());
                lstEntries.append(line);
            }

            qDebug() << lstEntries.size() << " entries";

            QString fileXMLName = fileNames[i];
            fileXMLName = fileXMLName.mid(fileXMLName.lastIndexOf("/") + 1);
            fileXMLName = fileXMLName.left(fileXMLName.size() - 4) + ".xml";
            FlashCards* flashCards = new FlashCards(lstEntries, fileNames[i], fileXMLName, this);

            // add the entries
            m_lstFlashCards.append(flashCards);

            // close the file
            file.close();
        }
    }

    emit fileLoaded(this);

    qDebug() << "FileLoader::loadFiles end";
}

int FileLoader::columnCount(const QModelIndex & parent) const
{
    return 2;
}

int FileLoader::rowCount(const QModelIndex &parent) const
{
    return m_lstFlashCards.size();
}

QVariant FileLoader::data(const QModelIndex &index, int role) const
{
//    qDebug() << "data request: [" << index.row() << "," << index.column() << "] role:" << role;

    QVariant ret;
/*
    data request: [ 1 , 0 ] role: 6
    data request: [ 1 , 0 ] role: 7
    data request: [ 1 , 0 ] role: 9
    data request: [ 1 , 0 ] role: 10
    data request: [ 1 , 0 ] role: 1
    data request: [ 1 , 0 ] role: 0
    data request: [ 1 , 0 ] role: 8
*/
    switch (role)
    {
    case Qt::DisplayRole:
        switch (index.column()){
        case enumFileName:
            ret = m_lstFlashCards[index.row()]->filePath();
            break;
        case enumNumOfEntries:
            ret = m_lstFlashCards[index.row()]->entries().size();
            break;
        default:
            break;
        }
        break;
    default:
        ret = QVariant(QVariant::Invalid);
        break;
    }
    return ret;
}

#include <QSize>

QVariant FileLoader::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch (section)
        {
        case enumFileName:
            return "File name";
            break;
        case enumNumOfEntries:
            return "# of entries";
            break;
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

/*

  dstPath = path specified by the user
*/
void FileLoader::createFlashCards(QString dstPath)
{
    if(!QDir(dstPath).exists()){
        emit fileLoaderError("Output directory does not exist!");
        return;
    }

    qDebug() << "FileLoader::createFlashCards() Destination = " << dstPath;

    QDir dirOutput;

    // Creating output folder
    // In case of name conflicts, it will append "(n)" to the folder name, n being an incremental number
    // The path to the created folder is stored to dirOutput
    {
        QString folderName = OUTPUT_FOLDER_NAME;

        for (int i = 1; ;i++){
            // coin the output folder path
            dirOutput.setPath(dstPath + "/" + folderName);
            // does it already exist?
            if (dirOutput.exists()){
                folderName = OUTPUT_FOLDER_NAME + "(" + QString::number(i) + ")";
                if (i > 1000){
                    // Folders "Output(1...1000)" are already in the same directory, which is not unlikely to happen
                    emit fileLoaderError("Output directory coudn't be created!");
                    return;
                }
            } else {
                // output folder path determined (stored to dirOutput)
                break;
            }
        }
    }

    // Create output folders
    qDebug() << "creating output folders...";
    qDebug() << dirOutput.mkpath(OUTPUT_WIDGET_FOLDER_NAME + "/" + OUTPUT_WIDGET_STYLE_FOLDER_NAME);
    qDebug() << dirOutput.mkpath(OUTPUT_WIDGET_FOLDER_NAME + "/" + OUTPUT_WIDGET_SCRIPT_FOLDER_NAME);
    qDebug() << dirOutput.mkpath(OUTPUT_WIDGET_FOLDER_NAME + "/" + OUTPUT_WIDGET_DATA_FOLDER_NAME);

    // Write rename.bat
    {
        QFile file(dirOutput.path() + "/" + FILENAME_RENAME_BAT);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
            emit fileLoaderError("Couldn't open output file: " + file.fileName());
            return;
        }else{
            file.write(QString("del " + OUTPUT_WIDGET_FOLDER_NAME + ".wgz\n").toUtf8());
            file.write(QString("rename " + OUTPUT_WIDGET_FOLDER_NAME + ".zip " + OUTPUT_WIDGET_FOLDER_NAME + ".wgz\n").toUtf8());
            file.close();
        }
    }

    // Move to the widget directory (shouldn't fail)
    Q_ASSERT(dirOutput.cd(OUTPUT_WIDGET_FOLDER_NAME));

    // Copy files from the resource
    qDebug() << "copying files...";
    qDebug() << QFile::copy(":/SimpleFlashCard/info_plist", dirOutput.path() + "/info.plist");
    qDebug() << QFile::copy(":/SimpleFlashCard/icon_png", dirOutput.path() + "/icon.png");
    qDebug() << QFile::copy(":/SimpleFlashCard/index_html", dirOutput.path() + "/index.html");
    qDebug() << QFile::copy(":/SimpleFlashCard/style_css", dirOutput.path() + "/" + OUTPUT_WIDGET_STYLE_FOLDER_NAME + "/simpleflashcard.css");
    qDebug() << QFile::copy(":/SimpleFlashCard/flashcards_js", dirOutput.path() + "/" + OUTPUT_WIDGET_SCRIPT_FOLDER_NAME + "/flashcards.js");
    qDebug() << QFile::copy(":/SimpleFlashCard/ajax_js", dirOutput.path() + "/" + OUTPUT_WIDGET_SCRIPT_FOLDER_NAME + "/Ajax.js");
    qDebug() << QFile::copy(":/SimpleFlashCard/main_js", dirOutput.path() + "/" + OUTPUT_WIDGET_SCRIPT_FOLDER_NAME + "/main.js");

    // Move to data folder (shouldn't fail)
    Q_ASSERT(dirOutput.cd(OUTPUT_WIDGET_DATA_FOLDER_NAME));

    // Write XML files
    for (int i = 0; i < m_lstFlashCards.size(); i++){
        QFile file(dirOutput.path() + "/" + m_lstFlashCards[i]->fileXMLName());
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
            emit fileLoaderError("Couldn't open output file: " + file.fileName());
            return;
        }else{
            file.write(QString("<flashcards>\n").toUtf8());
            for (int j = 0; j < m_lstFlashCards[i]->entries().size(); j++){
                QString line = m_lstFlashCards[i]->entries()[j];
                line = line.trimmed();
                if (line.indexOf("\t") != -1){
                    QStringList wrk = line.split("\t");
                    QString writeline = "<entry><lang1>" + wrk[0] + "</lang1><lang2>" + wrk[1] + "</lang2></entry>\n";
                    file.write(writeline.toUtf8());
                }
            }
            file.write(QString("</flashcards>\n").toUtf8());
            file.close();
        }
    }

    // Write filelist.js
    {
        QFile file(dirOutput.path() + "/" + FILENAME_FILELIST_JS);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
            emit fileLoaderError("Couldn't open output file: " + file.fileName());
            return;
        }else{
            file.write(QString("var xmlFileNames = [\n").toUtf8());
            for (int i = 0; i < m_lstFlashCards.size(); i++){
                QString writeline;
                if (i == m_lstFlashCards.size() - 1){
                    writeline = ",";
                }
                writeline += "\"" + m_lstFlashCards[i]->fileXMLName() + "\"\n";
                file.write(writeline.toUtf8());
            }
            file.write(QString("];\n").toUtf8());
            file.close();
        }
    }

}


