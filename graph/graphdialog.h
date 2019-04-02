#ifndef GRAPHDIALOG_H
#define GRAPHDIALOG_H

#include <QDialog>
#include "../viewer/frames/frames.h"

namespace Ui {
class GraphDialog;
}

class GraphDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GraphDialog(Frames* frames, QWidget *parent = nullptr);
    ~GraphDialog();

    quint16 getCurrentClusterLenght();
    QString getCurrentX();
    QString getCurrentY();
    QString getCurrentWindowGraph();

    void    selectLastWindow();

    //очистить windowGraph
    void    clearWindow();

    //добавить к windowGraph строку
    void appendWindow(QString value);

    QString NEW_WINDOW = "New Window";

private:
    Ui::GraphDialog *ui;

private slots:
    void slotSelectDataX(QString value);

signals:
    void signalDataXChanged(QString);
};

#endif // GRAPHDIALOG_H
