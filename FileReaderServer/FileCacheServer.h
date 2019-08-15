#ifndef FILECACHESERVER_H
#define FILECACHESERVER_H

// Qt
#include <QObject>
#include <QHash>
class QTcpServer;
class QTcpSocket;


class FileCacheServer : public QObject
{
  Q_OBJECT
  public:
    explicit FileCacheServer(const QString& filePath, int maxConnections = 10, QObject *parent = nullptr);
    ~FileCacheServer();
    void startServer();

  signals:
    void errMsg(const QString& errMsg);
    void msg(const QString& msg);

  private slots:
    void newConnection();
    void readClient();
    void clientDisconnected();
    void sendChunkToClient(QTcpSocket* socket, const QByteArray& data);

  private:
    QTcpServer* m_server;
    QHash<QString, qint64> m_clients;
    QString m_filePath;
};

#endif // FILECACHESERVER_H
