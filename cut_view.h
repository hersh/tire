/**************************************************************************
 * copyright 2014 SRI International.                                      *
 **************************************************************************/
/* Author: Dave Hershberger */

#ifndef CUT_VIEW_H
#define CUT_VIEW_H

#include <QGraphicsView>

class QGraphicsPathItem;
class QGraphicsScene;

class CutView: public QGraphicsView
{
Q_OBJECT
public:
  CutView();
  void setSourcePath(QPainterPath path);

private:
  QGraphicsScene* scene_;
  QGraphicsPathItem* path_item_;
};

#endif // CUT_VIEW_H
