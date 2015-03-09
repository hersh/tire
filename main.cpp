/**************************************************************************
 * copyright 2015 Dave Hershberger
 **************************************************************************/

#include <QApplication>
#include <tire_editor.h>

int main(int argc, char** argv)
{
  QApplication app(argc, argv);
  TireEditor editor;
  if(argc > 1)
  {
    editor.load(argv[1]);
  }
  editor.show();
  return app.exec();
}

