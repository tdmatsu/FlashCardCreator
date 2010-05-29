#include "fileloader.h"

#include <QDebug>
#include <QFile>
#include <QDir>
#include <QProcess>

const QString OUTPUT_FOLDER_NAME = "Output";
const QString OUTPUT_WIDGET_FOLDER_NAME = "SimpleFlashCard";

const QString OUTPUT_WIDGET_STYLE_FOLDER_NAME = "style";
const QString OUTPUT_WIDGET_SCRIPT_FOLDER_NAME = "script";
const QString OUTPUT_WIDGET_DATA_FOLDER_NAME = "data";

const QString FILENAME_DATA_JS = "data.js";


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

            QString collectionName = fileNames[i];
            collectionName = collectionName.mid(collectionName.lastIndexOf("/") + 1);
            collectionName = collectionName.left(collectionName.size() - 4);
            FlashCards* flashCards = new FlashCards(lstEntries, fileNames[i], collectionName, this);

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
    return 1;
}

int FileLoader::rowCount(const QModelIndex &parent) const
{
    return m_lstFlashCards.size();
}

QVariant FileLoader::data(const QModelIndex &index, int role) const
{
    QVariant ret;

    switch (role)
    {
    case Qt::DisplayRole:
        ret = m_lstFlashCards[index.row()]->filePath() + " (" + QString::number(m_lstFlashCards[index.row()]->entries().size()) + " entries)";
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
    dirOutput.mkpath(OUTPUT_WIDGET_FOLDER_NAME + "/" + OUTPUT_WIDGET_STYLE_FOLDER_NAME);
    dirOutput.mkpath(OUTPUT_WIDGET_FOLDER_NAME + "/" + OUTPUT_WIDGET_SCRIPT_FOLDER_NAME);
    dirOutput.mkpath(OUTPUT_WIDGET_FOLDER_NAME + "/" + OUTPUT_WIDGET_DATA_FOLDER_NAME);

    // Move to the widget directory
    dirOutput.cd(OUTPUT_WIDGET_FOLDER_NAME);

    // Copy files from the resource
    qDebug() << "copying files...";
    QFile::copy(":/SimpleFlashCard/info_plist", dirOutput.path() + "/info.plist");
    QFile::copy(":/SimpleFlashCard/icon_png", dirOutput.path() + "/icon.png");
    QFile::copy(":/SimpleFlashCard/index_html", dirOutput.path() + "/index.html");
    QFile::copy(":/SimpleFlashCard/style_css", dirOutput.path() + "/" + OUTPUT_WIDGET_STYLE_FOLDER_NAME + "/simpleflashcard.css");
    QFile::copy(":/SimpleFlashCard/flashcards_js", dirOutput.path() + "/" + OUTPUT_WIDGET_SCRIPT_FOLDER_NAME + "/flashcards.js");
    QFile::copy(":/SimpleFlashCard/ajax_js", dirOutput.path() + "/" + OUTPUT_WIDGET_SCRIPT_FOLDER_NAME + "/Ajax.js");
    QFile::copy(":/SimpleFlashCard/main_js", dirOutput.path() + "/" + OUTPUT_WIDGET_SCRIPT_FOLDER_NAME + "/main.js");

    // Write data.js
    QFile file(dirOutput.path() + "/" + OUTPUT_WIDGET_DATA_FOLDER_NAME + "/" + FILENAME_DATA_JS);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        emit fileLoaderError("Couldn't open output file: " + file.fileName());
        return;
    }else{
        file.write(QString("var FlashCardCollection = new Array();\n").toUtf8());
        file.write(QString("var FlashCardCollectionNames = new Array();\n\n").toUtf8());

        for (int i = 0; i < m_lstFlashCards.size(); i++){   // collection loop
            QString strwrk;
            strwrk = "FlashCardCollectionNames[" + QString::number(i) + "] = '" + m_lstFlashCards[i]->collectionName() + " (" + QString::number(m_lstFlashCards[i]->entries().size()) + ")';\n";
            file.write(strwrk.toUtf8());
            strwrk = "FlashCardCollection[" + QString::number(i) + "] = [\n";
            file.write(strwrk.toUtf8());

            for (int j = 0; j < m_lstFlashCards[i]->entries().size(); j++){     // entry loop
                QString line = m_lstFlashCards[i]->entries()[j];
                line = line.trimmed();
                if (line.indexOf("\t") != -1){
                    line.replace("\"","\"\"");
                    QStringList wrk = line.split("\t");
                    QString writeline = ",\"" + wrk[0] + "\\t" + wrk[1] + "\"" + "\n";
                    if(j == 0){
                        writeline = writeline.mid(1);   // remove the preceeding comma
                    }
                    file.write(writeline.toUtf8());
                }
            }
            file.write(QString("];\n\n").toUtf8());
        }
        file.close();
    }

    // zip it up
    if (QFile("C:/Program Files/7-Zip/7z.exe").exists())
    {
        qDebug() << "zipping...";
        QString zip_program = "C:/Program Files/7-Zip/7z.exe";
        QStringList zip_arguments;
        zip_arguments << "a" << (dirOutput.path() + ".zip") << (dirOutput.path() + "\\");

        QProcess *zip_process = new QProcess(this);
        zip_process->start(zip_program, zip_arguments);
        zip_process->waitForFinished(5000);

        QFile zip_file(dirOutput.path() + ".zip");
        if(zip_file.exists())
        {
            qDebug() << "zip file already exists! renaming...";
            zip_file.rename(zip_file.fileName().left(zip_file.fileName().size()-4) + ".wgz");
        }
    }

    QString strExplorerPath1 = "C:/WINNT/explorer.exe";
    QString strExplorerPath2 = "C:/WINDOWS/explorer.exe";
    QString strExplorerPath;

    if (QFile(strExplorerPath1).exists()) {
        strExplorerPath = strExplorerPath1;
    } else if (QFile(strExplorerPath2).exists()) {
        strExplorerPath = strExplorerPath2;
    }

    // open the output folder
    if (strExplorerPath.size())
    {
        qDebug() << "opening output folder...";

        QString program = strExplorerPath;
        QStringList arguments;
        QDir dirwrk = dirOutput;
        dirwrk.cdUp();
        arguments << dirwrk.path().replace("/", "\\");

        QProcess *process = new QProcess(this);
        process->start(program, arguments);
    }
}

