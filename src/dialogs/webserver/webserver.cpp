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

#include "webserver.h"
#include "ui_webserver.h"

#include <QFileDialog>
#include <QObject>

WebServer::WebServer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WebServer)
{
    m_process = NULL ;
    m_serverFolder.clear() ;
    m_command.clear() ;
    ui->setupUi(this);
}

WebServer::~WebServer()
{
    on_stop_pushButton_clicked() ;
    delete ui;
}

void WebServer::processOutput()
{
    if (m_process) {
        ui->webserverOutput_plainTextEdit->moveCursor (QTextCursor::End);
        ui->webserverOutput_plainTextEdit->insertPlainText (m_process->readAllStandardOutput());
        ui->webserverOutput_plainTextEdit->moveCursor (QTextCursor::End);
    }
}

void WebServer::processTerminated(int exitCode)
{
    disconnect(m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(processOutput())) ;
    disconnect(m_process, SIGNAL(finished(int)), this, SLOT(processTerminated(int))) ;
    ui->stop_pushButton->setEnabled(false) ;
    ui->start_pushButton->setEnabled(true) ;
    ui->webserverOutput_plainTextEdit->appendPlainText( QString("Server Stopped (exit code")
                                                 + QString::number(exitCode) + QString(")\n"));
    delete m_process ;
    m_process = NULL ;
}


void WebServer::setServerFolder(QString folder)
{
    on_stop_pushButton_clicked() ;
    m_serverFolder = folder ;
    ui->webserverFolder_label->setText(m_serverFolder) ;
}

QString WebServer::serverFolder()
{
    return m_serverFolder ;
}

void WebServer::setCommand(QString command)
{
    on_stop_pushButton_clicked() ;
    m_command = command ;
    ui->webserverCommand_lineEdit->setText(m_command) ;
}

QString WebServer::command()
{
    return m_command ;
}

void WebServer::on_setFolder_pushButton_clicked()
{
    on_stop_pushButton_clicked() ;
    m_serverFolder = QFileDialog::getExistingDirectory(this, tr("Web Server Folder"),
                                m_serverFolder,
                                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->webserverFolder_label->setText(m_serverFolder) ;
}

void WebServer::on_start_pushButton_clicked()
{
    if (m_process) return ;
    ui->webserverOutput_plainTextEdit->clear() ;
    QString cmdline = m_command.replace("%FOLDER%", m_serverFolder) ;
    ui->webserverOutput_plainTextEdit->appendPlainText(QString("Server Launched: ") + cmdline + QString(" in ") + m_serverFolder + QString("\n"));
    QStringList cmdargs = cmdline.split(" ") ;
    QString cmds = cmdargs[0] ;
    cmdargs.removeAt(0) ;

    m_process = new QProcess ;
    if (m_process) {

        m_process->setProcessChannelMode(QProcess::MergedChannels);
        connect(m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(processOutput())) ;
        connect(m_process, SIGNAL(finished(int)), this, SLOT(processTerminated(int))) ;

        m_process->setWorkingDirectory(m_serverFolder);
        m_process->start(cmds, cmdargs) ;
        if (m_process->pid()>=0) {
            ui->stop_pushButton->setEnabled(true) ;
            ui->start_pushButton->setEnabled(false) ;
        }
    }
}

void WebServer::on_stop_pushButton_clicked()
{
    if (m_process) m_process->terminate() ;
    if (m_process) m_process->waitForFinished(2000) ;
    if (m_process) m_process->kill() ;
    if (m_process) processTerminated(-1);
}

void WebServer::on_close_pushButton_clicked()
{
    hide() ;
}

void WebServer::on_webserverCommand_lineEdit_editingFinished()
{
    setCommand(ui->webserverCommand_lineEdit->text()) ;
}
