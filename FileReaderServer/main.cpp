// Qt
#include <QCoreApplication>
#include <QString>
#include <QObject>

// Local
#include "FileCacheServer.h"

// STL
#include <iostream>

#define USAGE "usage: max_connections file_to_share"


int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);

  if (argc < 3)
  {
    std::cout << USAGE << std::endl;
    return 0;
  }

  QString filePath(argv[2]);
  FileCacheServer server(filePath, QString(argv[1]).toInt(), nullptr);
  QObject::connect(&server, &FileCacheServer::errMsg,  [=]( const QString &msg ) {std::cout << msg.toStdString() << std::endl;});
  QObject::connect(&server, &FileCacheServer::msg,  [=]( const QString &msg ) {std::cout << msg.toStdString() << std::endl;});

  server.startServer();

  return a.exec();
}
