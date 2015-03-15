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
  scene_->setSceneRect(-5, -15, 50, 30);
  setScene(scene_);

  side_path_item_ = new QGraphicsPathItem;
  scene_->addItem(side_path_item_);
  tread_path_item_ = new QGraphicsPathItem;
  scene_->addItem(tread_path_item_);
}

void CutView::setSideSourcePath(QPainterPath path)
{
  QPainterPath cut_path;
  cut_path.moveTo(0, 0);

  float rim_radius = 15;
  int num_tire_segments = 16;
  float tire_segment_angle = 2 * M_PI / num_tire_segments;
  float path_length = path.length();

  int num_segments = 1000;
  float x, y;
  // path along top edge
  for(int i = 0; i <= num_segments; i++)
  {
    x = path_length * i / float(num_segments);
    QPointF point = path.pointAtPercent(i / float(num_segments));
    y = 0.5 * tire_segment_angle * (rim_radius - point.y());
    cut_path.lineTo(x, -y);
  }
  // arc at seam with tread
  float r = rim_radius - path.pointAtPercent(1).y();
  if(y*y > r*r) return;
  float b = sqrt(r*r - y*y);
  float ax = x - b;
  float angle_rad = atan2(y, b);
  float angle_deg = angle_rad * 180 / M_PI;
  half_tread_edge_length_ = angle_rad * r;
  cut_path.arcTo(ax-r, -r, 2*r, 2*r, angle_deg, -2*angle_deg);
  // path along bottom edge
  for(int i = num_segments; i >= 0; i--)
  {
    x = path_length * i / float(num_segments);
    QPointF point = path.pointAtPercent(i / float(num_segments));
    y = 0.5 * tire_segment_angle * (rim_radius - point.y());
    cut_path.lineTo(x, y);
  }
  cut_path.lineTo(0, 0);

  side_path_item_->setPath(cut_path);
  tread_path_item_->setPos(ax + r, 0);
}

void CutView::setTreadSourcePath(QPainterPath path)
{
  QPainterPath resampled_path = PathView::resample(path);

  QPainterPath cut_path;
  cut_path.moveTo(0, 0);

  float rim_radius = 15;
  int num_tire_segments = 16;
  float tire_segment_angle = 2 * M_PI / num_tire_segments;
  float path_length = resampled_path.length();

  float first_y = 0.5 * tire_segment_angle *
    (rim_radius - resampled_path.pointAtPercent(0).y());
  float scale = half_tread_edge_length_ / first_y;
//  printf("scale = %lf\n", scale);

  int num_segments = 1000;
  for(int i = 0; i <= num_segments; i++)
  {
    float x = path_length * i / float(num_segments);
    QPointF point = resampled_path.pointAtPercent(i / float(num_segments));
    float y = scale * 0.5 * tire_segment_angle * (rim_radius - point.y());
    cut_path.lineTo(x, -y);
  }
  for(int i = num_segments; i >= 0; i--)
  {
    float x = path_length * i / float(num_segments);
    QPointF point = resampled_path.pointAtPercent(i / float(num_segments));
    float y = scale * 0.5 * tire_segment_angle * (rim_radius - point.y());
    cut_path.lineTo(x, y);
  }
  cut_path.lineTo(0, 0);
  tread_path_item_->setPath(cut_path);
}

