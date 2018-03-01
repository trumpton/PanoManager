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

#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QDialog>

namespace Ui {
class ProgressDialog;
}

class ProgressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProgressDialog(QWidget *parent = 0);
    ~ProgressDialog();

    void show() ;
    void setMaximum(int max) ;
    void setValue(int value) ;
    int value() ;
    void setTitle(QString title) ;
    void setText1(QString text) ;
    void setText2(QString text) ;
    bool isCancelled() ;

private slots:
    void on_build_buttonBox_rejected();

private:
    bool m_isCancelled ;
    Ui::ProgressDialog *ui;
};

#endif // PROGRESSDIALOG_H
