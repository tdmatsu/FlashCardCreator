#ifndef FILELOADER_H
#define FILELOADER_H

#include <QObject>
#include <QStringList>

#include <QAbstractTableModel>

class FlashCards : public QObject
{
    Q_OBJECT

private:
    QStringList m_entries;
    QString m_fileName;

    FlashCards();
public:
    FlashCards(QStringList entries, QString fileName, QObject*parent) : QObject(parent), m_entries(entries), m_fileName(fileName) {}
    ~FlashCards() {}

    QStringList entries() const{return m_entries;}
    QString fileName() const{return m_fileName;}
};


class FileLoader : public QAbstractTableModel
{
    Q_OBJECT

private:
    enum ColumnID {
        enumFileName = 0,
        enumNumOfEntries = 1
    };

public:
    FileLoader(QObject * parent = 0);

    // inherited from QAbstractTableModel
    virtual int columnCount ( const QModelIndex & parent = QModelIndex()) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

public slots:
    void loadFiles(QStringList fileNames);
    void createFlashCards(QString dstPath);

signals:
    void fileLoaded(FileLoader*fileLoader);
    void fileLoaderError(QString);

private:
    QList<FlashCards*> m_lstFlashCards;
};

#endif // FILELOADER_H
