/**************************************************************************
 * copyright 2015 Dave Hershberger
 **************************************************************************/

#include <QGraphicsPathItem>
#include <cut_view.h>
#include <math.h>
#include <path_view.h>
#include <stdio.h>

CutView::CutView()
{
  scale(10, 10);
  scene_ = new QGraphicsScene;
  scene_->setSceneRect(-5, -25, 50, 25);
  setScene(scene_);

  path_item_ = new QGraphicsPathItem;
  scene_->addItem(path_item_);
}

void CutView::setSourcePath(QPainterPath path)
{
  QPainterPath resampled_path = PathView::resample(path);

  QPainterPath cut_path;
  cut_path.moveTo(0, 0);

  float rim_radius = 15;
  int num_tire_segments = 16;
  float tire_segment_angle = 2 * M_PI / num_tire_segments;
  float path_length = resampled_path.length();

  int num_segments = 1000;
  for(int i = 0; i <= num_segments; i++)
  {
    float x = path_length * i / float(num_segments);
    QPointF point = resampled_path.pointAtPercent(i / float(num_segments));
//    printf("point: %lf, %lf\n", point.x(), point.y());
    float y = 0.5 * tire_segment_angle * (rim_radius - point.y());
    cut_path.lineTo(x, -y);
  }
  path_item_->setPath(cut_path);
}

