#include "helpdialog.h"

HelpDialog::HelpDialog(QWidget *parent) : QDialog(parent)
{
    layout = new QGridLayout(this);
    text = new QTextBrowser();
    layout->addWidget(text);
    text->setSource(QUrl("qrc:/doc/help.html"));
    resize(640,400);
    setWindowFlags(Qt::Window);
    setWindowTitle(tr("帮助"));
}

HelpDialog::~HelpDialog()
{

}

