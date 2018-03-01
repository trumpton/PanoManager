//    PanoManager - Interactive panorama tour manager program
//    Copyright (C) 2018  Steve M Clarke
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

//
// Progress Dialog
//

#include "progressdialog.h"
#include "ui_progressdialog.h"

#include <QApplication>

ProgressDialog::ProgressDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProgressDialog)
{
    ui->setupUi(this);
    m_isCancelled=false ;
}

ProgressDialog::~ProgressDialog()
{
    delete ui;
}

void ProgressDialog::show()
{
    QDialog::show() ;
    qApp->processEvents() ;
}

void ProgressDialog::setMaximum(int max)
{
    ui->progressBar->setMinimum(0) ;
    ui->progressBar->setMaximum(max);
}

int ProgressDialog::value()
{
    return ui->progressBar->value() ;
}

void ProgressDialog::setValue(int value)
{
    ui->progressBar->setValue(value) ;
    qApp->processEvents() ;
}

void ProgressDialog::setText1(QString text)
{
    ui->text1->setText(text) ;
    ui->text2->setText("") ;
    qApp->processEvents() ;
}

void ProgressDialog::setText2(QString text)
{
    ui->text2->setText(text) ;
    qApp->processEvents() ;
}

void ProgressDialog::setTitle(QString title)
{
    setWindowTitle(title) ;
    qApp->processEvents() ;
}

bool ProgressDialog::isCancelled()
{
       return m_isCancelled ;
}

void ProgressDialog::on_build_buttonBox_rejected()
{
    m_isCancelled = true ;
}
