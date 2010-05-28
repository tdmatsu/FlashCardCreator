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
    QString m_filePath;
    QString m_collectionName;

    FlashCards();
public:
    FlashCards(QStringList entries, QString filePath, QString collectionName, QObject*parent)
        : QObject(parent), m_entries(entries), m_filePath(filePath), m_collectionName(collectionName) {}
    ~FlashCards() {}

    QStringList entries() const{return m_entries;}
    QString filePath() const{return m_filePath;}
    QString collectionName() const{return m_collectionName;}
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
