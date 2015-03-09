/**************************************************************************
 * copyright 2015 Dave Hershberger
 **************************************************************************/

#include <QGraphicsEllipseItem>
#include <QGraphicsPathItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLabel>
#include <QVBoxLayout>
#include <math.h>
#include <path_view.h>
#include <stdio.h>

PathView::PathView()
{
  QVBoxLayout* layout = new QVBoxLayout;
  graphics_view_ = new QGraphicsView;
  layout->addWidget(graphics_view_, 1);
  status_ = new QLabel;
  layout->addWidget(status_);
  setLayout(layout);

  graphics_view_->scale(10, 10);

  scene_ = new QGraphicsScene;
  scene_->setSceneRect(-15, -25, 30, 30);

  QGraphicsItem* rim = scene_->addEllipse(-.5, -.5, 1, 1);
  rim->setPos(-8, 0);
  rim = scene_->addEllipse(-.5, -.5, 1, 1);
  rim->setPos(8, 0);
  graphics_view_->setScene(scene_);

  start_x_ = -7.5;
  start_y_ = .5;
  start_angle_ = -90;

  tread_width_ = 18;

  left_item_ = new QGraphicsPathItem;
  scene_->addItem(left_item_);
  right_item_ = new QGraphicsPathItem;
  right_item_->setTransform(QTransform(-1, 0, 0, 1, 0, 0));
  scene_->addItem(right_item_);
  tread_item_ = new QGraphicsPathItem;
  QPen pen;
  pen.setWidthF(.3);
  tread_item_->setPen(pen);
  scene_->addItem(tread_item_);
}

void PathView::setPathString(QString path_string)
{
  path_ = QPainterPath();

  path_.moveTo(start_x_, start_y_);
  float x = start_x_;
  float y = start_y_;
  float angle = start_angle_;

  QStringList lines = path_string.split("\n");
  for(int i = 0; i < lines.size(); i++)
  {
    QStringList words = lines[i].split(" ");
    if(words.size() == 2)
    {
      bool ok = true;
      float radius = words[0].toFloat(&ok);
      if(ok)
      {
        float arclength = words[1].toFloat(&ok);
        if(ok)
        {
          addArc(radius, arclength, &x, &y, &angle);
        }
      }
    }
  }
  finish(&x, &y, &angle);
  setPathInItems();
  QRectF bounds = path_.boundingRect();
  status_->setText(QString("width: %1, height: %2").
                   arg(bounds.width() * 2).arg(bounds.height()));
}

void PathView::addArc(float radius, float arclength,
                      float* x, float* y, float* angle)
{
  float rad_angle = *angle * M_PI / 180;

  // radius == 0 means straight line, since we have no infinity key.
  if(radius == 0)
  {
    float end_x = *x + arclength * cos(rad_angle);
    float end_y = *y + arclength * sin(rad_angle);

    path_.lineTo(end_x, end_y);

    *x = end_x;
    *y = end_y;
  }
  else // else positive or negative arc
  {
    float a = rad_angle + M_PI/2;
    float cx = *x + radius * cos(a);
    float cy = *y + radius * sin(a);

    float sweep_angle = arclength / radius * 180 / M_PI;
    path_.arcTo(cx - radius, cy - radius, 2*radius, 2*radius,
                -*angle + 90, -sweep_angle);
    QPointF end = path_.currentPosition();
    *x = end.x();
    *y = end.y();
    *angle += sweep_angle;
  }
}

void PathView::finish(float* x, float* y, float* angle)
{
  float sweep_angle = -*angle;
  float c = cos((*angle + 90) * M_PI / 180);
  if(c != 0)
  {
    float radius = -*x / c;
    addArc(radius, sweep_angle * M_PI / 180 * radius, x, y, angle);
  }
}

QPainterPath PathView::resample(const QPainterPath& path)
{
  QPainterPath new_path;
  new_path.moveTo(path.pointAtPercent(0));
  int num_segments = 1000;
  for(int i = 1; i <= num_segments; i++)
  {
    new_path.lineTo(path.pointAtPercent(i / float(num_segments)));
  }
  return new_path;
}

void PathView::setPathInItems()
{
  QPainterPath path = resample(path_);
  float side_len = path.length() - tread_width_ / 2;
  float side_ratio = side_len / path.length();

  QPainterPath side_path;
  side_path.moveTo(path.pointAtPercent(0));
  int total_segments = 1000;
  int num_segments = int(side_ratio * total_segments);
  for(int i = 1; i <= num_segments; i++)
  {
    side_path.lineTo(path.pointAtPercent(i / float(total_segments)));
  }
  left_item_->setPath(side_path);
  right_item_->setPath(side_path);
  
  QPainterPath tread_path;
  tread_path.moveTo(path.pointAtPercent(num_segments / float(total_segments)));
  for(int i = num_segments; i <= total_segments; i++)
  {
    tread_path.lineTo(path.pointAtPercent(i / float(total_segments)));
  }
  for(int i = total_segments; i >= num_segments; i--)
  {
    QPointF point = path.pointAtPercent(i / float(total_segments));
    point.setX(-point.x());
    tread_path.lineTo(point);
  }
  tread_item_->setPath(tread_path);
}
