/**************************************************************************
 * copyright 2015 Dave Hershberger
 **************************************************************************/

#include <QAction>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMenuBar>
#include <QMessageBox>
#include <QPainter>
#include <QPrintDialog>
#include <QPrinter>
#include <QSplitter>
#include <QTextEdit>
#include <cut_view.h>
#include <path_view.h>
#include <tire_editor.h>

TireEditor::TireEditor()
{
  QSplitter* splitter = new QSplitter;
  setCentralWidget(splitter);
  text_edit_ = new QTextEdit;
  path_view_ = new PathView;
  cut_view_ = new CutView;
  splitter->addWidget(text_edit_);
  splitter->addWidget(path_view_);
  splitter->addWidget(cut_view_);
  connect(text_edit_, SIGNAL(textChanged()), this, SLOT(onTextChanged()));

  QAction* new_act = new QAction(tr("&New"), this);
  new_act->setShortcuts(QKeySequence::New);
  new_act->setStatusTip(tr("Create a new file"));
  connect(new_act, SIGNAL(triggered()), this, SLOT(newFile()));
     
  QAction* open_act = new QAction(tr("&Open..."), this);
  open_act->setShortcuts(QKeySequence::Open);
  open_act->setStatusTip(tr("Open an existing file"));
  connect(open_act, SIGNAL(triggered()), this, SLOT(open()));

  QAction* save_act = new QAction(tr("&Save"), this);
  save_act->setShortcuts(QKeySequence::Save);
  save_act->setStatusTip(tr("Save the document to disk"));
  connect(save_act, SIGNAL(triggered()), this, SLOT(save()));

  QAction* save_as_act = new QAction(tr("Save &As"), this);
  save_as_act->setShortcuts(QKeySequence::SaveAs);
  save_as_act->setStatusTip(tr("Save the document with a new filename"));
  connect(save_as_act, SIGNAL(triggered()), this, SLOT(saveAs()));

  QAction* print_act = new QAction(tr("&Print"), this);
  print_act->setShortcuts(QKeySequence::Print);
  print_act->setStatusTip(tr("Print the cutout patterns"));
  connect(print_act, SIGNAL(triggered()), this, SLOT(print()));

  QMenu* file_menu = menuBar()->addMenu(tr("&File"));
  file_menu->addAction(new_act);
  file_menu->addAction(open_act);
  file_menu->addAction(save_act);
  file_menu->addAction(save_as_act);
  file_menu->addAction(print_act);
  menuBar()->addMenu(file_menu);
}

void TireEditor::onTextChanged()
{
  path_view_->setPathString(text_edit_->toPlainText());
  cut_view_->setSideSourcePath(path_view_->getSidePath());
  cut_view_->setTreadSourcePath(path_view_->getTreadPath());
}

void TireEditor::newFile()
{
  text_edit_->setText("");
}

void TireEditor::open()
{
  load(QFileDialog::getOpenFileName(this, tr("Open File"),
                                    QDir::currentPath(),
                                    tr("Tire Files (*.tire)")));
}

void TireEditor::load(QString filename)
{
  filename = QFileInfo(filename).absoluteFilePath();
  QFile file(filename);
  if(file.open(QIODevice::ReadOnly))
  {
    text_edit_->setText(file.readAll());
    current_file_name_ = filename;
    file.close();
  }
  else
  {
    QMessageBox::critical(this, "Open failed.", file.errorString());
  }
}

void TireEditor::save()
{
  if(current_file_name_ == "")
  {
    saveAs();
  }
  else
  {
    QFile file(current_file_name_);
    if(file.open(QIODevice::WriteOnly))
    {
      file.write(text_edit_->toPlainText().toAscii());
      file.close();
    }
    else
    {
      QMessageBox::critical(this, "Save failed.", file.errorString());
    }
  }
}

void TireEditor::saveAs()
{
  current_file_name_ =
    QFileDialog::getSaveFileName(this, tr("Save File"),
                                 QDir::currentPath() + "/untitled.tire",
                                 tr("Tire Files (*.tire)"));
  save();
}

void TireEditor::print()
{
  QPrinter printer;

  QPrintDialog *dialog = new QPrintDialog(&printer, this);
  dialog->setWindowTitle(tr("Print Cutout Patterns"));
  if(dialog->exec() == QDialog::Accepted)
  {
    QPainter painter;
    painter.begin(&printer);
    printPath(cut_view_->sidePath(), &painter, &printer);
    printer.newPage();
    printPath(cut_view_->treadPath(), &painter, &printer);
    painter.end();
  }
}

void TireEditor::printPath(const QPainterPath& path,
                           QPainter* painter,
                           QPrinter* printer)
{
  // As measured on large-format printer at work.
  float real_print_over_desired = (17.0 + 13.0/16.0) / 18.0;

  QRectF path_bounds = path.boundingRect();
  QRectF page_inches = printer->pageRect(QPrinter::Inch);
  QRectF page_pixels = printer->pageRect(QPrinter::DevicePixel);

  float dpi_x = page_pixels.width() / page_inches.width();
  float dpi_y = page_pixels.height() / page_inches.height();

  painter->resetTransform();
  painter->translate(page_pixels.width() / 2 - path_bounds.center().y() * dpi_x,
                     page_pixels.height() / 2 - path_bounds.center().x() * dpi_y);
  painter->rotate(90);
  painter->scale(dpi_x / real_print_over_desired, dpi_y / real_print_over_desired);
  QPen fat_pen;
  fat_pen.setWidthF(.01);
  painter->setPen(fat_pen);
  painter->drawPath(path);
  painter->drawRect(QRectF(path_bounds.center().x()-.5, path_bounds.center().y()-.5, 1, 1));
}
