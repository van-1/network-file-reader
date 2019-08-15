// Qt
#include <QScrollBar>
#include <QTextCursor>
#include <QStringList>
#include <QRegExp>
#include <QTextDocument>

// Local
#include "PartedPlainTextEdit.h"

PartedPlainTextEdit::PartedPlainTextEdit(QWidget* parent)
  :QPlainTextEdit(parent)
{
  QScrollBar* scrollBar = this->verticalScrollBar();
  //connect(scrollBar, SIGNAL(sliderReleased()), this, SLOT(handleScrollAction()));
  connect(scrollBar, SIGNAL(actionTriggered(int)), this, SLOT(handleScrollAction()));
}


PartedPlainTextEdit::PartedPlainTextEdit(const QString& text, QWidget* parent)
  :QPlainTextEdit(text, parent)
{
  QScrollBar* scrollBar = this->verticalScrollBar();
  //connect(scrollBar, SIGNAL(sliderReleased()), this, SLOT(handleScrollAction()));
  connect(scrollBar, SIGNAL(actionTriggered(int)), this, SLOT(handleScrollAction()));
}


PartedPlainTextEdit::~PartedPlainTextEdit()
{}


void PartedPlainTextEdit::handleScrollAction()
{
  checkScrollPosition();
}

void PartedPlainTextEdit::checkScrollPosition()
{
  QScrollBar* scrollBar = this->verticalScrollBar();
  const bool atBottom = (scrollBar->sliderPosition() == scrollBar->maximum());
  const bool atTop = (scrollBar->sliderPosition() == scrollBar->minimum());

  if (atBottom)
  {
    emit scrollAtEnd(1);
    return;
  }

  if (atTop)
  {
    emit scrollAtEnd(-1);
    return;
  }
}


void PartedPlainTextEdit::wheelEvent(QWheelEvent* event)
{
  QPlainTextEdit::wheelEvent(event);
  checkScrollPosition();
}


void PartedPlainTextEdit::keyPressEvent(QKeyEvent* event)
{
  QPlainTextEdit::keyPressEvent(event);

  if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down || event->key() == Qt::Key_PageUp || event->key() == Qt::Key_PageDown)
  {
    checkScrollPosition();
  }
}

