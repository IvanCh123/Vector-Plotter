#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int paint();
    void setImage();
    int caracteres;
    double puntosChar;


private slots:
    void on_pushButtonGraficar_clicked();

    void on_lineEditDimension1_textEdited(const QString &arg1);

    void on_lineEditIntervaloX_textEdited(const QString &arg1);

    void on_lineEditDimension2_textEdited(const QString &arg1);

    void on_lineEditIntervaloY_textEdited(const QString &arg1);

    void on_lineEditDimension3_textEdited(const QString &arg1);

    void on_lineEditEpsilon_textEdited(const QString &arg1);

    void resizeEvent(QResizeEvent* event);

private:
    Ui::MainWindow *ui;
    void enableButton();
    bool caracteresValidos(QString texto);
    char* datos();
    bool existeCero(QString texto);
};

#endif // MAINWINDOW_H
