#ifndef BUBBLESCRIPTDESIGNER_H
#define BUBBLESCRIPTDESIGNER_H

#include <QDialog>

namespace Ui {
class BubbleScriptDesigner;
}

class BubbleScriptDesigner : public QDialog
{
    Q_OBJECT

public:
    explicit BubbleScriptDesigner(QWidget *parent = nullptr);
    ~BubbleScriptDesigner();

private:
    Ui::BubbleScriptDesigner *ui;
};

#endif // BUBBLESCRIPTDESIGNER_H
