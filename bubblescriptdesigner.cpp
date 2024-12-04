#include "bubblescriptdesigner.h"
#include "ui_bubblescriptdesigner.h"

BubbleScriptDesigner::BubbleScriptDesigner(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::BubbleScriptDesigner)
{
    ui->setupUi(this);
}

BubbleScriptDesigner::~BubbleScriptDesigner()
{
    delete ui;
}
