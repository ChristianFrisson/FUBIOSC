/*
   Copyright (C) 2011-2012:
         Daniel Roggen, droggen@gmail.com

   All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDERS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE FREEBSD PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "helpdialog.h"
#include "ui_helpdialog.h"


HelpDialog::HelpDialog(QString message,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpDialog)
{
   ui->setupUi(this);

   QFrame *mainFrame = new QFrame();
   ui->scrollArea->setWidget(mainFrame);
   QVBoxLayout *signalVerticalLayout = new QVBoxLayout();
   mainFrame->setLayout(signalVerticalLayout);



   label = new QLabel();
   label->setWordWrap(true);
   label->setOpenExternalLinks(true);


   signalVerticalLayout->addWidget(label);
   label->setFixedWidth(550);

   label->setText(message);
}

HelpDialog::~HelpDialog()
{
    delete ui;
}

void HelpDialog::on_buttonBox_clicked(QAbstractButton *button)
{
   close();
}
