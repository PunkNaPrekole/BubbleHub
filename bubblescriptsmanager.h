#ifndef BUBBLESCRIPTSMANAGER_H
#define BUBBLESCRIPTSMANAGER_H

#include <QDialog>
#include <QString>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

class BubbleScriptsManager : public QDialog
{
    Q_OBJECT

public:
    explicit BubbleScriptsManager(const QString &deviceName, QWidget *parent = nullptr);

private:
    QString deviceName;
};

#endif // BUBBLESCRIPTSMANAGER_H
