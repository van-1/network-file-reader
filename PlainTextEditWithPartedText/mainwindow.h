#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Qt
#include <QMainWindow>

// Local
#include "PartedPlainTextEdit.h"

namespace Ui
{
  class MainWindow;
}

class QLineEdit;
class FileCache;
class QPlainTextEdit;


class MainWindow : public QMainWindow
{
  Q_OBJECT

  public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

  private slots:
    // 1 - dwon, -1 - top
    void scrollTextToEnd(int pos);
    void showStatusText(const QString& msg);
    void connected();
    void getNewData(const QByteArray& msg);

  private:
    Ui::MainWindow* ui;
    PartedPlainTextEdit* m_partedPlainTextEdit;
    FileCache* m_fileCache;
    QPlainTextEdit* m_msgLog;
};

#endif // MAINWINDOW_H
