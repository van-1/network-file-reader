#ifndef FILECACHE_H
#define FILECACHE_H

// Qt
#include <QObject>
#include <QAbstractSocket>
#include <QByteArray>
#include <QTime>
class QTcpSocket;


class FileCache : public QObject
{
  Q_OBJECT
  public:
    explicit FileCache(const QString& hostName, int port, QObject* parent = nullptr);
    ~FileCache();

  void connectToHost(int attempsCnt = 100, int timeout = 3000);
  void requestFileChunk(qint32 direction, qint32 chunkSizeBytes = 1000000);

  signals:
    void connected();
    void errorMsg(const QString& errMsg);
    void msg(const QString& msg);
    void finished(const QByteArray& chunk);

  private slots:
    void readyRead();
    void error(QAbstractSocket::SocketError);
    void delay(int milliseconds);

  private:
    QTcpSocket* m_tcpSocket;
    QByteArray m_recvBuffer;
    QString m_host;
    int m_port;
    qint32 m_chunkSize;
};

#endif // FILECACHE_H
