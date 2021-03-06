#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

class FileLoader;


namespace Ui {
    class Dialog;
}

class Dialog : public QDialog {
    Q_OBJECT
public:
    Dialog(QWidget *parent = 0);
    ~Dialog();

protected:
    void changeEvent(QEvent *e);
    void dragEnterEvent ( QDragEnterEvent * event );
    void dropEvent ( QDropEvent * event );
    void dragMoveEvent ( QDragMoveEvent * event );

private:
    Ui::Dialog *ui;

    void createFlashCard();
    void writeFlashCard();

    QString m_strDstPath;
    QStringList m_dragFileNames;

public slots:
    void showFiles(FileLoader* fileLoader);
    void handleFileLoaderError(QString);

private slots:
    void on_btnRun_clicked();
    void on_btnBrowseTSVFiles_clicked();

signals:
    void filesSelected(QStringList fileNames);
    void runclicked(QString dstPath);
};

#endif // DIALOG_H
