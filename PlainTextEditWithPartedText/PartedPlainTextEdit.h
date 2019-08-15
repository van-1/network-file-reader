#ifndef PARTEDPLAINTEXTEDIT_H
#define PARTEDPLAINTEXTEDIT_H

// Qt
#include <QObject>
#include <QPlainTextEdit>
#include <QString>


class PartedPlainTextEdit: public QPlainTextEdit
{
  Q_OBJECT

  public:
    explicit PartedPlainTextEdit(QWidget* parent = nullptr);
    explicit PartedPlainTextEdit(const QString& text, QWidget* parent = nullptr);
    ~PartedPlainTextEdit();

  signals:
    // 1 - dwon, -1 - top
    void scrollAtEnd(int pos);

  private slots:
    void handleScrollAction();
    void checkScrollPosition();

  protected:
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
};

#endif // PARTEDPLAINTEXTEDIT_H
