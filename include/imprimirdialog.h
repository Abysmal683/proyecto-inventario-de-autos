#ifndef IMPRIMIRDIALOG_H
#define IMPRIMIRDIALOG_H

#include <QDialog>
#include <QGraphicsScene>
#include "infodialog.h"

namespace Ui {
class ImprimirDialog;
}

class ImprimirDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImprimirDialog(QWidget *parent = nullptr);
    ~ImprimirDialog();

    void setContent(const VehiclePrintData &data);

private slots:
    void on_pushButtonCancelar_clicked();
    void on_pushButtonImprimir_clicked();

private:
    Ui::ImprimirDialog *ui;

    QGraphicsScene *scene;
    QImage previewImage;

    VehiclePrintData printData;

    QImage renderPreview();
    void setPreviewContent(const QImage &img);
};

#endif // IMPRIMIRDIALOG_H
