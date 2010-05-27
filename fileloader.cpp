#include "fileloader.h"

#include <QDebug>
#include <QFile>

const QString OUTPUT_FOLDER_NAME = "Output";

FileLoader::FileLoader(QObject * parent)
    : QAbstractTableModel(parent)
{

}

void FileLoader::loadFiles(QStringList fileNames)
{
    qDebug() << "FileLoader::loadFiles start";

    Q_ASSERT(!fileNames.isEmpty());

    for (int i = 0; i < fileNames.size(); i++){
        qDebug () << fileNames.at(i);


        // read the CSV data
        QFile file(fileNames.at(i));

        // open the file
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
            qDebug() << "failed to open input file";
        } else {

            qDebug() << "reading file... " << fileNames.at(i);

            QStringList lstEntries;

            while (!file.atEnd()){
                QString line = QString::fromUtf8(file.readLine().constData());
                lstEntries.append(line);
            }

            qDebug() << lstEntries.size() << " entries";

            FlashCards* flashCards = new FlashCards(lstEntries, fileNames.at(i),this);

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
            ret = m_lstFlashCards.at(index.row())->fileName();
            break;
        case enumNumOfEntries:
            ret = m_lstFlashCards.at(index.row())->entries().size();
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

#include <QDir>
#include <QFileDialog>

void FileLoader::createFlashCards(QString dstPath)
{
    if(!QDir(dstPath).exists()){
        emit fileLoaderError("Output directory does not exist!");
        return;
    }

    qDebug() << "FileLoader::createFlashCards() Destination = " << dstPath;

    // create the output folder
    QString folderName = OUTPUT_FOLDER_NAME;
    QString strDstFolder;
    int i = 1;
    while (1){
        strDstFolder = dstPath + "/" + folderName;
        if (QDir(strDstFolder).exists()){
            folderName = OUTPUT_FOLDER_NAME + "(" + QString::number(i) + ")";
            i++;
            if (i > 1000){
                emit fileLoaderError("Output directory coudn't be created!");
                return;
            }
        } else {
            folderName += "/SimpleFlashCard";
            strDstFolder += "/SimpleFlashCard";
            qDebug() << "Making output dir... ";
            qDebug() << (QDir(dstPath).mkpath(folderName) ? "success :)" : "failed :(");
            break;
        }
    }


    QString dirStyle = "style";
    QString dirScript = "script";
    QString dirData = "data";

    qDebug() << "creating folders...";
    qDebug() << QDir(strDstFolder).mkdir(dirStyle);
    qDebug() << QDir(strDstFolder).mkdir(dirScript);
    qDebug() << QDir(strDstFolder).mkdir(dirData);

    qDebug() << "copying files...";
    qDebug() << QFile::copy(":/SimpleFlashCard/info_plist", strDstFolder + "/info.plist");
    qDebug() << QFile::copy(":/SimpleFlashCard/icon_png", strDstFolder + "/icon.png");
    qDebug() << QFile::copy(":/SimpleFlashCard/index_html", strDstFolder + "/index.html");
    qDebug() << QFile::copy(":/SimpleFlashCard/style_css", strDstFolder + "/" + dirStyle + "/simpleflashcard.css");
    qDebug() << QFile::copy(":/SimpleFlashCard/flashcards_js", strDstFolder + "/" + dirScript + "/flashcards.js");
    qDebug() << QFile::copy(":/SimpleFlashCard/ajax_js", strDstFolder + "/" + dirScript + "/Ajax.js");
    qDebug() << QFile::copy(":/SimpleFlashCard/main_js", strDstFolder + "/" + dirScript + "/main.js");




    // export each set of flashcards
    for (int i = 0; i < m_lstFlashCards.size(); i++){
        QString strFileName = strDstFolder + "/" + dirData + "/" + m_lstFlashCards.at(i)->fileName().mid(m_lstFlashCards.at(i)->fileName().lastIndexOf("/") + 1);
        strFileName = strFileName.left(strFileName.size() - 4) + ".xml";
        QFile file(strFileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
            emit fileLoaderError("Output file couldn't be opened!\n" + strFileName);
            return;
        }else{
            file.write(QString("<flashcards>\n").toUtf8());
            for (int j = 0; j < m_lstFlashCards.at(i)->entries().size(); j++){
                QString line = m_lstFlashCards.at(i)->entries().at(j);
                line = line.trimmed();
                if (line.indexOf("\t") != -1){
                    QStringList wrk = line.split("\t");
                    QString writeline = "<entry><lang1>" + wrk.at(0) + "</lang1><lang2>" + wrk.at(1) + "</lang2></entry>\n";
                    file.write(writeline.toUtf8());
                }
            }
            file.write(QString("</flashcards>\n").toUtf8());
            file.close();
        }
    }

/*
    // show the number of entries
    showMessage(QString().sprintf("%d entries", m_lstEntries.size()));
    QString strFileName = m_strOutputDir;
    strFileName.append("/test2.html");
    QFile file(strFileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        showMessage("failed to open output file");
        showMessage(strFileName);
        return;
    }

    for (int i=0; i < m_lstEntries.size(); ++i)
    {
        file.write(m_lstEntries.at(i).toUtf8() + "\n");
    }

    file.close();
*/

}


