// Qt
#include <QTcpSocket>
#include <QApplication>
#include <QtEndian>
#include <QTimer>
#include <QEventLoop>

// Local
#include "FileCache.h"


FileCache::FileCache(const QString& hostName, int port, QObject* parent)
  : QObject(parent)
  , m_tcpSocket(new QTcpSocket(this))
  , m_host(hostName)
  , m_port(port)
  , m_chunkSize(1000000)
{
  connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
  connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
          this, SLOT(error(QAbstractSocket::SocketError)));
  connect(m_tcpSocket, SIGNAL(connected()), this, SIGNAL(connected()));
}

FileCache::~FileCache()
{}


void FileCache::connectToHost(int attempsCnt, int timeout)
{
  int currAttemps = 0;
  while ((++currAttemps) < attempsCnt)
  {
    m_tcpSocket->connectToHost(m_host, quint16(m_port));
    if (m_tcpSocket->waitForConnected(100))
      break;
    emit msg("trying to connect attemp " + QString::number(currAttemps));
    delay(timeout);
  };
}


void FileCache::requestFileChunk(qint32 direction, qint32 chunkSizeBytes)
{
  m_chunkSize = chunkSizeBytes;
  qint32 directionNetwork = qToBigEndian(direction);
  qint32 chunkSizeBytesNetwork = qToBigEndian(chunkSizeBytes);
  m_tcpSocket->write(reinterpret_cast<char*>(&directionNetwork), sizeof(directionNetwork));
  m_tcpSocket->write(reinterpret_cast<char*>(&chunkSizeBytesNetwork), sizeof(chunkSizeBytesNetwork));
  m_tcpSocket->flush();
}


void FileCache::readyRead()
{
  if (m_tcpSocket->bytesAvailable() > 0)
  {
    // Магическое слово, наличие которого говорит о том, что посылку получили полностью
    const QByteArray& endOfPacketWord = "!AEAENAE!";
    bool hasWord = true;
    m_recvBuffer.append(m_tcpSocket->readAll());

    if (m_recvBuffer.size() > endOfPacketWord.size())
    {
      for (int i = m_recvBuffer.size() - 1, j = endOfPacketWord.size() - 1; j >= 0; --i, --j)
      {
        if (m_recvBuffer[i] != endOfPacketWord[j])
          hasWord = false;
      }
    }

    if (hasWord)
    {
      int magickWordPos = m_recvBuffer.indexOf(endOfPacketWord);
      m_recvBuffer.remove(magickWordPos, endOfPacketWord.length());
      emit finished(m_recvBuffer);
      emit msg("received data. sizeof(data) =" + QString::number(m_recvBuffer.size()));
      m_recvBuffer.clear();
    }
  }
}


void FileCache::error(QAbstractSocket::SocketError error)
{
  QString strError = "Error: " + (error == QAbstractSocket::HostNotFoundError ?
                     "The host was not found" :
                     error == QAbstractSocket::RemoteHostClosedError ?
                     "The remote host is closed" :
                     error == QAbstractSocket::ConnectionRefusedError ?
                     "The connection was refused" :
                     QString(m_tcpSocket->errorString()));

  emit errorMsg(strError);
}


void FileCache::delay(int milliseconds)
{
  QEventLoop loop;
  QTimer t;
  t.connect(&t, &QTimer::timeout, &loop, &QEventLoop::quit);
  t.start(milliseconds);
  loop.exec();
}

