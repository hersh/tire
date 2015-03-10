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
  void setSideSourcePath(QPainterPath path);
  void setTreadSourcePath(QPainterPath path);

private:
  QGraphicsScene* scene_;
  QGraphicsPathItem* side_path_item_;
  QGraphicsPathItem* tread_path_item_;
  float half_tread_edge_length_;
};

#endif // CUT_VIEW_H
