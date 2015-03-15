/**************************************************************************
 * copyright 2015 Dave Hershberger
 **************************************************************************/

#ifndef TIRE_EDITOR_H
#define TIRE_EDITOR_H

#include <QMainWindow>

class CutView;
class PathView;
class QTextEdit;

class TireEditor: public QMainWindow
{
Q_OBJECT
public:
  TireEditor();
  void load(QString filename);

public Q_SLOTS:
  void onTextChanged();
  void newFile();
  void open();
  void save();
  void saveAs();
  void print();

private:
  void printPath(const QPainterPath& path,
                 QPainter* painter,
                 QPrinter* printer);

  QTextEdit* text_edit_;
  PathView* path_view_;
  QString current_file_name_;
  CutView* cut_view_;
};

#endif // TIRE_EDITOR_H
