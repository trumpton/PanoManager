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
// Web Server Dialog
//

#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <QString>
#include <QDialog>
#include <QProcess>

namespace Ui {
class WebServer;
}

class WebServer : public QDialog
{
    Q_OBJECT

public:
    explicit WebServer(QWidget *parent = 0);
    void setServerFolder(QString folder) ;
    QString serverFolder() ;
    void setCommand(QString command) ;
    QString command() ;
    ~WebServer();


private slots:
    void processOutput() ;
    void processTerminated(int exitCode) ;
    void on_setFolder_pushButton_clicked();
    void on_start_pushButton_clicked();
    void on_stop_pushButton_clicked();
    void on_close_pushButton_clicked();

    void on_webserverCommand_lineEdit_editingFinished();

private:
    QString m_serverFolder ;
    QProcess *m_process ;
    QString m_command ;
    Ui::WebServer *ui;
};

#endif // WEBSERVER_H
