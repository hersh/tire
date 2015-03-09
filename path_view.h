/**************************************************************************
 * copyright 2015 Dave Hershberger
 **************************************************************************/

#ifndef PATH_VIEW_H
#define PATH_VIEW_H

#include <QWidget>

class QGraphicsPathItem;
class QGraphicsScene;
class QGraphicsView;
class QLabel;

class PathView: public QWidget
{
Q_OBJECT
public:
  PathView();

  void setPathString(QString path_string);
  QPainterPath getPath() const { return path_; }

  static QPainterPath resample(const QPainterPath& path);

private:
  void addArc(float radius, float arclength,
              float* x, float* y, float* angle);
  void finish(float* x, float* y, float* angle);
  void setPathInItems();

  QGraphicsScene* scene_;

  float start_x_;
  float start_y_;
  float start_angle_;

  QGraphicsView* graphics_view_;
  QGraphicsPathItem* left_item_;
  QGraphicsPathItem* right_item_;
  QGraphicsPathItem* tread_item_;
  QPainterPath path_;
  QLabel* status_;
  float tread_width_;
};

#endif // PATH_VIEW_H
