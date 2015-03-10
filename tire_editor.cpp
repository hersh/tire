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

  QMenu* file_menu = menuBar()->addMenu(tr("&File"));
  file_menu->addAction(new_act);
  file_menu->addAction(open_act);
  file_menu->addAction(save_act);
  file_menu->addAction(save_as_act);
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

