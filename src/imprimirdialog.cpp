#include "imprimirdialog.h"
#include "ui_imprimirdialog.h"

#include <QPrinter>
#include <QPrintDialog>
#include <QPdfWriter>
#include <QPainter>
#include <QFileDialog>
#include <QGraphicsPixmapItem>

ImprimirDialog::ImprimirDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ImprimirDialog)
{
    ui->setupUi(this);

    scene = new QGraphicsScene(this);
    ui->graphicsViewPrevisualizacion->setScene(scene);

    if (parent)
        move(parent->geometry().center() - rect().center());

    ui->radioButtonPDF->setChecked(true);
}

ImprimirDialog::~ImprimirDialog()
{
    delete ui;
}

// ======================================================
//     CARGAR CONTENIDO PARA PREVIEW
// ======================================================
void ImprimirDialog::setContent(const VehiclePrintData &data)
{
    printData = data;

    QImage preview = renderPreview();
    setPreviewContent(preview);
}

// ======================================================
//     PREVIEW EN EL GRAPHICS VIEW
// ======================================================
void ImprimirDialog::setPreviewContent(const QImage &img)
{
    previewImage = img;

    scene->clear();
    QPixmap pix = QPixmap::fromImage(img);

    auto *item = scene->addPixmap(pix);
    item->setTransformationMode(Qt::SmoothTransformation);

    // Tamaño real de la escena = tamaño real de la imagen
    scene->setSceneRect(QRect(QPoint(0,0), pix.size()));

    // RESET
    ui->graphicsViewPrevisualizacion->resetTransform();
    ui->graphicsViewPrevisualizacion->setAlignment(Qt::AlignCenter);

    // ---- FIX PRINCIPAL ----
    // Mostrar la imagen más grande (zoom inicial x1.8)
    double zoomFactor = 1;
    ui->graphicsViewPrevisualizacion->scale(zoomFactor, zoomFactor);

    // Permitir scroll (evita que Qt encoja la imagen)
    ui->graphicsViewPrevisualizacion->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->graphicsViewPrevisualizacion->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

// ======================================================
//          GENERAR VISTA PREVIA COMO IMAGEN
// ======================================================
QImage ImprimirDialog::renderPreview()
{
    int width = 800;
    int height = 1200;

    QImage img(width, height, QImage::Format_ARGB32);
    img.fill(Qt::white);

    QPainter p(&img);
    p.setRenderHint(QPainter::TextAntialiasing);

    int y = 60;

    auto draw = [&](const QString &label, const QString &value) {
        p.drawText(50, y, QString("%1: %2").arg(label, value));
        y += 40;
    };

    draw("VIN", printData.vin);
    draw("Placa", printData.placa);
    draw("Marca", printData.marca);
    draw("Modelo", printData.modelo);
    draw("Epoca", printData.epoca);
    draw("Color", printData.color);
    draw("Propietario", printData.propietario);

    draw("Motor", printData.motor);
    draw("Kilometraje", printData.kilometraje);
    draw("Puertas", printData.puertas);
    draw("Carroceria", printData.carroceria);
    draw("Detalles", printData.detalles);
    draw("Estado", printData.estado);

    y += 30;

    if (!printData.imagen.isNull()) {

        QSize target(600, 400);

        QImage scaled = printData.imagen.scaled(
            target,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            );

        p.drawImage(QRect(100, y, scaled.width(), scaled.height()), scaled);
    }

    p.end();
    return img;
}

// ======================================================
//                 BOTONES
// ======================================================
void ImprimirDialog::on_pushButtonCancelar_clicked()
{
    close();
}

void ImprimirDialog::on_pushButtonImprimir_clicked()
{
    if (previewImage.isNull())
        return;

    // ------------------------------------
    // Opción guardar como PDF
    // ------------------------------------
    if (ui->radioButtonPDF->isChecked()) {

        QString fileName = QFileDialog::getSaveFileName(
            this, "Guardar como PDF", "", "Archivos PDF (*.pdf)");

        if (fileName.isEmpty())
            return;

        QPdfWriter pdf(fileName);
        pdf.setPageSize(QPageSize(QPageSize::A4));
        pdf.setResolution(300);

        QPainter painter(&pdf);

        QImage toPrint = renderPreview();
        QSize scaled = toPrint.size();
        scaled.scale(painter.viewport().size(), Qt::KeepAspectRatio);

        painter.drawImage(QRect(QPoint(0,0), scaled), toPrint);
        painter.end();
    }

    // ------------------------------------
    // Opción imprimir por impresora
    // ------------------------------------
    else if (ui->radioButtonImpresora->isChecked()) {

        QPrinter printer(QPrinter::HighResolution);
        QPrintDialog dialog(&printer, this);

        if (dialog.exec() != QDialog::Accepted)
            return;

        QPainter painter(&printer);

        QRect target = painter.viewport();
        QSize scaled = previewImage.size();
        scaled.scale(target.size(), Qt::KeepAspectRatio);

        int x = (target.width() - scaled.width()) / 2;
        int y = (target.height() - scaled.height()) / 2;

        painter.drawImage(QRect(x, y, scaled.width(), scaled.height()), previewImage);
        painter.end();
    }

    accept();
}
