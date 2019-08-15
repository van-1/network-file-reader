// Local
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "FileCache.h"

// Qt
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPlainTextEdit>


MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , m_fileCache(new FileCache("127.0.0.1", 8080, this))
{
  ui->setupUi(this);
  m_partedPlainTextEdit = new PartedPlainTextEdit();
  m_partedPlainTextEdit->setReadOnly(true);

  m_msgLog = new QPlainTextEdit();
  setWindowTitle("File viewer");
  QVBoxLayout* vLayout = new QVBoxLayout();
  vLayout->addWidget(m_partedPlainTextEdit);
  vLayout->addWidget(m_msgLog);
  centralWidget()->setLayout(vLayout);

  connect(m_partedPlainTextEdit, SIGNAL(scrollAtEnd(int)), this, SLOT(scrollTextToEnd(int)));
  connect(m_fileCache, SIGNAL(msg(const QString&)), this, SLOT(showStatusText(const QString&)));
  connect(m_fileCache, SIGNAL(errorMsg(const QString&)), this, SLOT(showStatusText(const QString&)));
  connect(m_fileCache, SIGNAL(finished(const QByteArray&)), this, SLOT(getNewData(const QByteArray&)));
  connect(m_fileCache, SIGNAL(connected()), this, SLOT(connected()));
  m_fileCache->connectToHost();
}


MainWindow::~MainWindow()
{
  delete ui;
}


void MainWindow::scrollTextToEnd(int pos)
{
  m_msgLog->setPlainText(m_msgLog->toPlainText() + "requested new chunk of text. direction = " + QString::number(pos) + "\n");
  m_fileCache->requestFileChunk(pos);
}


void MainWindow::showStatusText(const QString &msg)
{
  m_msgLog->setPlainText(m_msgLog->toPlainText() + msg + "\n");
}


void MainWindow::connected()
{
  m_fileCache->requestFileChunk(1);
}


void MainWindow::getNewData(const QByteArray& msg)
{
  if (!msg.isEmpty())
  {
    m_partedPlainTextEdit->clear();
    m_partedPlainTextEdit->setPlainText(QString(msg));
  }
}

