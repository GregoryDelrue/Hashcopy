#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCryptographicHash>
#include <QDirIterator>
#include <QFileInfo>
#include <QString>
#include "stdio.h"
#include <QStringListModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QShortcut>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QHash<QByteArray, QString> *HashSourceFiles(const QStringList &files);
    QHash<QByteArray, QString> *HashTargetDir(const QString &dir, bool recursive = false);    //Datastructure-Hash


private slots:
    void on_pb_Browse_clicked();

    void on_pb_Browse_Source_clicked();

    void on_pb_Merge_clicked();

    void on_cb_Rehash_stateChanged(int arg1);

    void del_pressed();

    void on_cb_subdirs_toggled(bool checked);

signals:




private:
    bool recursive;
    Ui::MainWindow *ui;
    QStringList *filesToBeDeleted;
    QStringListModel *strmod;

    QByteArray *HashFile(const QString &fileName);
    void MergeHashLists(const QHash<QByteArray, QString> &sourceHash, const QHash<QByteArray, QString> &targetHash);
    void Rename(QFile &file); //takes a file, puts it in /doubles/filename
    bool Chck_doubles(const QString &filePath); //takes a path, checks if a file with it exists aready.Is a switch for Rename().

    QShortcut *shortcut_del;
    QStringList *viewModelStringList;
    QStringList *hashes;//||
    QStringList *files; //|| one of theses two is displayed on the listView. or the empty list. This is to avoid hashing the hashes


};

#endif // MAINWINDOW_H
