#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //注册插拔事件
    HDEVNOTIFY hDevNotify;
    DEV_BROADCAST_DEVICEINTERFACE NotifacationFiler;
    ZeroMemory(&NotifacationFiler,sizeof(DEV_BROADCAST_DEVICEINTERFACE));
    NotifacationFiler.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotifacationFiler.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    for(int i=0;i<sizeof(GUID_DEVINTERFACE_LIST)/sizeof(GUID);i++)
    {
        NotifacationFiler.dbcc_classguid = GUID_DEVINTERFACE_LIST[i];//GetCurrentUSBGUID();
        hDevNotify = RegisterDeviceNotification(HANDLE(this->winId()),&NotifacationFiler,DEVICE_NOTIFY_WINDOW_HANDLE);
        if(!hDevNotify)
        {
            GetLastError();
            qDebug() << "注册失败" <<endl;
        }
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *keyValue)
{
    if(keyValue->key() == Qt::Key_Return)
    {
      Str1 = ui->lineEdit->text();
      ui->lineEdit->setText(Str1);
      qDebug() << Str1;
      Str1.clear();
    }else{
        Str2 += keyValue->text();
    }
}

bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    MSG* msg = reinterpret_cast<MSG*>(message);//第一层解算
       UINT msgType = msg->message;
       if(msgType==WM_DEVICECHANGE)
       {
           PDEV_BROADCAST_HDR lpdb = PDEV_BROADCAST_HDR(msg->lParam);//第二层解算
           switch (msg->wParam) {
           case DBT_DEVICEARRIVAL:
               if(lpdb->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
               {
                   PDEV_BROADCAST_DEVICEINTERFACE pDevInf  = PDEV_BROADCAST_DEVICEINTERFACE(lpdb);
                   QString device_name = "插入设备(name)："+QString::fromWCharArray(pDevInf->dbcc_name,int(pDevInf->dbcc_size)).toUtf8();
                    ui->label->setText("已插入设备");
                   //qDebug()<< device_name;
               }
               break;
           case DBT_DEVICEREMOVECOMPLETE:
               if(lpdb->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
               {
                   PDEV_BROADCAST_DEVICEINTERFACE pDevInf  = PDEV_BROADCAST_DEVICEINTERFACE(lpdb);
                   QString device_name = "移除设备(name)："+QString::fromWCharArray(pDevInf->dbcc_name,int(pDevInf->dbcc_size)).toUtf8();
                   ui->label->setText("设备已拔出");
                   //qDebug()<< device_name;
               }
               break;
           }
       }
       return false;
}

