#ifndef DEVICECONTROLBLOCK_H
#define DEVICECONTROLBLOCK_H

#include <QWidget>

class DeviceControlBlock : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceControlBlock(QWidget *parent = nullptr) : QWidget(parent) {}
    virtual ~DeviceControlBlock() {}
};

#endif // DEVICECONTROLBLOCK_H
