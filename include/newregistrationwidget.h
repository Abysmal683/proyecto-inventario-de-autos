#ifndef NEWREGISTRATIONWIDGET_H
#define NEWREGISTRATIONWIDGET_H

#include <QWidget>
#include <QCompleter>
#include <QByteArray>

namespace Ui {
class NewRegistrationWidget;
}

class NewRegistrationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NewRegistrationWidget(QWidget *parent = nullptr);
    ~NewRegistrationWidget();

signals:
    void goWelcomeWidgetRequested();
    void vehicleAdded(const QVariantMap &vehicle);

private slots:
    void onPushButtonConfirmarClicked();
    void onPushButtonSeleccionarImagenClicked();

private:
    Ui::NewRegistrationWidget *ui;
    QByteArray vehicleImage;
    QCompleter *vinCompleter;
    QCompleter *placaCompleter;
    void loadReferenceData();
    void setupCompleters();
    void loadCompleterData();
};

#endif // NEWREGISTRATIONWIDGET_H
