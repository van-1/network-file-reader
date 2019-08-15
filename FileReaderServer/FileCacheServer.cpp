// Local
#include "FileCacheServer.h"

// Qt
#include <QTcpServer>
#include <QTcpSocket>
#include <QFileInfo>
#include <QFile>
#include <QtEndian>


FileCacheServer::FileCacheServer(const QString& filePath, int maxConnections, QObject *parent)
  : QObject(parent)
  , m_server(new QTcpServer(this))
  , m_filePath(filePath)
{
  m_server->setMaxPendingConnections(maxConnections);
  connect(m_server, SIGNAL(newConnection()), this, SLOT(newConnection()));
}


FileCacheServer::~FileCacheServer()
{}


void FileCacheServer::startServer()
{
  QFileInfo sharedFile(m_filePath);
  if (!sharedFile.exists())
  {
    emit errMsg("cannot find file " + m_filePath);
    return;
  }

  if (!m_server->listen(QHostAddress::Any, 8080))
  {
    emit errMsg(m_server->errorString());
    return;
  }
  emit msg("server started at port 8080");
}


void FileCacheServer::newConnection()
{
  QTcpSocket* clientSocket = m_server->nextPendingConnection();

  connect(clientSocket, SIGNAL(disconnected()), clientSocket, SLOT(deleteLater()));
  connect(clientSocket, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
  connect(clientSocket, SIGNAL(readyRead()), this, SLOT(readClient()));
  m_clients[clientSocket->peerAddress().toString() + ":" + QString::number(clientSocket->peerPort())] = 0;
}


void FileCacheServer::readClient()
{
  QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
  const QString& clientKey = clientSocket->peerAddress().toString() + ":" + QString::number(clientSocket->peerPort());

  qint32 direction = 0;
  qint32 chunkSize = 0;

  if (clientSocket->bytesAvailable() >= (sizeof(direction) + sizeof(chunkSize)))
  {
    clientSocket->read(reinterpret_cast<char*>(&direction), sizeof(direction));
    clientSocket->read(reinterpret_cast<char*>(&chunkSize), sizeof(chunkSize));

    // 1 - dwon, -1 - top
    direction = qFromBigEndian(direction);
    chunkSize = qFromBigEndian(chunkSize);
    emit msg("from client " + clientKey + " get direction = " + QString::number(direction) +
             " and chunk_size = " + QString::number(chunkSize));

    qint64 clientFileOffset = m_clients[clientKey];
    QFile sharedFile(m_filePath);
    sharedFile.open(QFile::ReadOnly);
    QByteArray fileData;

    if (direction == 1)
    {
      sharedFile.seek(clientFileOffset);
    }
    else if (direction == -1)
    {
      clientFileOffset += (2 * direction * chunkSize);
      if (clientFileOffset < 0)
        clientFileOffset = 0;
      sharedFile.seek(clientFileOffset);
    }

    emit msg("offset for client " + clientKey + " = " + QString::number(clientFileOffset));

    fileData = sharedFile.read(chunkSize);
    m_clients[clientKey] = sharedFile.pos();
    qint32 readSize = fileData.size();
    emit msg("to client " + clientKey + " will be sended " + QString::number(readSize) + " bytes");
    if (readSize > 0)
    {
      // Магическое слово, наличие которого говорит о том, что посылку получили полностью
      fileData.append("!AEAENAE!");
      sendChunkToClient(clientSocket, fileData);
      clientSocket->flush();
    }
    sharedFile.close();
  }
}


void FileCacheServer::clientDisconnected()
{
  QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
  m_clients.remove(clientSocket->peerAddress().toString() + ":" + QString::number(clientSocket->peerPort()));
  emit msg("client " + clientSocket->peerAddress().toString() + ":" + QString::number(clientSocket->peerPort()) + " disconnected");
}


void FileCacheServer::sendChunkToClient(QTcpSocket* socket, const QByteArray& data)
{
  socket->write(data.data(), data.size());
}
