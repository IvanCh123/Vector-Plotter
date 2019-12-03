#include <vector>
#include <complex>
#include <cmath>
#include <sys/stat.h>
#include <fcntl.h>

#include <boost/tuple/tuple.hpp>
#include <boost/array.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/irange.hpp>
#include <boost/bind.hpp>
#include "gnuplot-iostream.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


template <typename T>
struct MyTriple {
    MyTriple() : x(0), y(0), z(0) { }
    MyTriple(T _x, T _y, T _z) : x(_x), y(_y), z(_z) { }

    T x, y, z;
};

namespace gnuplotio {
template<typename T>
struct BinfmtSender<MyTriple<T> > {
    static void send(std::ostream &stream) {
        BinfmtSender<T>::send(stream);
        BinfmtSender<T>::send(stream);
        BinfmtSender<T>::send(stream);
    }
};

template <typename T>
struct BinarySender<MyTriple<T> > {
    static void send(std::ostream &stream, const MyTriple<T> &v) {
        BinarySender<T>::send(stream, v.x);
        BinarySender<T>::send(stream, v.y);
        BinarySender<T>::send(stream, v.z);
    }
};

// We don't use text mode in this demo.  This is just here to show how it would go.
template<typename T>
struct TextSender<MyTriple<T> > {
    static void send(std::ostream &stream, const MyTriple<T> &v) {
        TextSender<T>::send(stream, v.x);
        stream << " ";
        TextSender<T>::send(stream, v.y);
        stream << " ";
        TextSender<T>::send(stream, v.z);
    }
};
}

const int num_u = 10;
int num_v_total;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_pushButtonGraficar_clicked()
{
    bool valido = caracteresValidos(this->ui->lineEditEpsilon->text());
    if(valido){
        valido = caracteresValidos(this->ui->lineEditDimension1->text());
        if(valido){
            valido = existeCero(this->ui->lineEditDimension1->text());
        }
    }
    if(valido){
        valido = caracteresValidos(this->ui->lineEditDimension2->text());
        if(valido){
            valido = existeCero(this->ui->lineEditDimension2->text());
        }
    }
    if(valido){
        valido = caracteresValidos(this->ui->lineEditDimension3->text());
        if(valido){
            valido = existeCero(this->ui->lineEditDimension3->text());
        }
    }
    if(valido){
        valido = caracteresValidos(this->ui->lineEditIntervaloX->text());
    }
    if(valido){
        valido = caracteresValidos(this->ui->lineEditIntervaloY->text());
    }
    if(!valido){
        this->ui->statusBar->showMessage("Las expresiones contienen caracteres inválidos");
    }

    else if(this->caracteres < 0){

    }
    else{
        char* array = datos();
        int fd, ret;
        fd = open("/dev/GMatrixDev", O_RDWR);
        if(fd < 0){
            perror("Failed to open the device");
        }


        ret = write(fd, array, 200 - this->caracteres);
        ret = read(fd, &(this->puntosChar), 3000);
        paint();
        setImage();
    }
}

MyTriple<double> get_point(int u, int v) {
    double a = 2.0*M_PI*u/(num_u-1);
    double b = 2.0*M_PI*v/(num_v_total-1);
    double z = 0.3*std::cos(a);
    double r = 1 + 0.3*std::sin(a);
    double x = r * std::cos(b);
    double y = r * std::sin(b);
    return MyTriple<double>(x, y, z);
}

int MainWindow::paint()
{
    Gnuplot gp;
    // for debugging, prints to console
    //Gnuplot gp(stdout);

    int num_examples = 7;
#ifdef USE_ARMA
    num_examples += 3;
#endif
#ifdef USE_BLITZ
    num_examples += 3;
#endif

    int num_v_each = 50 / num_examples + 1;

    num_v_total = (num_v_each-1) * num_examples + 1;
    int shift = 0;


    gp << "set zrange [-1:1]\n";
    gp << "set hidden3d nooffset\n";

    // I use temporary files rather than stdin because the syntax ends up being easier when
    // plotting several datasets.  With the stdin method you have to give the full plot
    // command, then all the data.  But I would rather give the portion of the plot command for
    // the first dataset, then give the data, then the command for the second dataset, then the
    // data, etc.


    gp << "set terminal png \n";
    gp << "set output 'b.png' \n";
    gp << "splot ";

//    {
//        std::vector<std::vector<MyTriple<double> > > pts(num_u);
//        for(int u=0; u<num_u; u++) {
//            pts[u].resize(num_v_each);
//            for(int v=0; v<num_v_each; v++) {
//                pts[u][v] = get_point(u, v+shift);
//            }
//        }
//        gp << gp.binFile2d(pts, "record") << "with lines title 'vec of vec of MyTriple'";
//    }

    //        gp << ", ";
    //        shift += num_v_each-1;

    //Begins my code
//    {
//        std::vector<std::vector<boost::tuple<double,double,double> > > pts(1000);
//        for(int u=0; u<1000; u+=3) {
//            pts[u].resize(1000);
//            for(int v=0; v<1000; v++) {
//                pts[u][v] = boost::make_tuple(
//                    get_point(u, v+shift).x,
//                    get_point(u, v+shift).y,
//                    get_point(u, v+shift).z
//                );
//            }
//        }
//        gp << gp.binFile2d(pts, "record") << "with lines title 'vec of vec of boost::tuple'";
//    }

//            {
//                std::vector<std::vector<boost::tuple<double,double,double> > > pts(num_u);
//                for(int u=0; u<num_u; u++) {
//                    pts[u].resize(num_v_each);
//                    for(int v=0; v<num_v_each; v++) {
//                        pts[u][v] = boost::make_tuple(
//                            get_point(u, v+shift).x,
//                            get_point(u, v+shift).y,
//                            get_point(u, v+shift).z
//                        );
//                    }
//                }
//                gp << gp.binFile2d(pts, "record") << "with lines title 'vec of vec of boost::tuple'";
//            }

    // Código modificado
//    {
//        std::vector<std::vector<boost::tuple<double,double,double> > > pts(250);
//        for(int u=0; u<250; u++) {
//            pts[u].resize(num_v_each);
//            for(int v=0; v<1; v++) {
//                pts[u][v] = boost::make_tuple(
//                    this->puntosChar[u],
//                    this->puntosChar[u+1] ,
//                    this->puntosChar[u+2]
//                );
//            }
//        }
//        gp << gp.binFile2d(pts, "record") << "with lines title 'vec of vec of boost::tuple'";
//    }

    {
            std::vector<std::vector<boost::tuple<double,double,double> > > pts(5);
            for(int u=0; u<5; u++) {
                pts[u].resize(1);
                for(int v=0; v<1; v++) {
                    pts[u][v] = boost::make_tuple(
                        u,
                        v ,
                        u
                    );
                }
            }
            gp << gp.binFile2d(pts, "record") << "with lines title 'vec of vec of boost::tuple'";
        }

    //        gp << ", ";
    //        shift += num_v_each-1;

    //        {
    //            std::vector<std::vector<double> > x_pts(num_u);
    //            std::vector<std::vector<double> > y_pts(num_u);
    //            std::vector<std::vector<double> > z_pts(num_u);
    //            for(int u=0; u<num_u; u++) {
    //                x_pts[u].resize(num_v_each);
    //                y_pts[u].resize(num_v_each);
    //                z_pts[u].resize(num_v_each);
    //                for(int v=0; v<num_v_each; v++) {
    //                    x_pts[u][v] = get_point(u, v+shift).x;
    //                    y_pts[u][v] = get_point(u, v+shift).y;
    //                    z_pts[u][v] = get_point(u, v+shift).z;
    //                }
    //            }
    //            gp << gp.binFile2d(boost::make_tuple(x_pts, y_pts, z_pts), "record") <<
    //                "with lines title 'boost::tuple of vec of vec'";
    //        }

    //        gp << ", ";
    //        shift += num_v_each-1;

    //        {
    //            std::vector<boost::tuple<
    //                    std::vector<double>,
    //                    std::vector<double>,
    //                    std::vector<double>
    //                > > pts;
    //            for(int u=0; u<num_u; u++) {
    //                std::vector<double> x_pts(num_v_each);
    //                std::vector<double> y_pts(num_v_each);
    //                std::vector<double> z_pts(num_v_each);
    //                for(int v=0; v<num_v_each; v++) {
    //                    x_pts[v] = get_point(u, v+shift).x;
    //                    y_pts[v] = get_point(u, v+shift).y;
    //                    z_pts[v] = get_point(u, v+shift).z;
    //                }
    //                pts.push_back(boost::make_tuple(x_pts, y_pts, z_pts));
    //            }
    //            gp << gp.binFile2d(pts, "record") <<
    //                "with lines title 'vec of boost::tuple of vec'";
    //        }

    //        gp << ", ";
    //        shift += num_v_each-1;

    //        {
    //            std::vector<std::vector<double> > x_pts(num_u);
    //            std::vector<std::vector<std::pair<double, double> > > yz_pts(num_u);
    //            for(int u=0; u<num_u; u++) {
    //                x_pts[u].resize(num_v_each);
    //                yz_pts[u].resize(num_v_each);
    //                for(int v=0; v<num_v_each; v++) {
    //                    x_pts [u][v] = get_point(u, v+shift).x;
    //                    yz_pts[u][v] = std::make_pair(
    //                        get_point(u, v+shift).y,
    //                        get_point(u, v+shift).z);
    //                }
    //            }
    //            gp << gp.binFile2d(std::make_pair(x_pts, yz_pts), "record") <<
    //                "with lines title 'pair(vec(vec(dbl)),vec(vec(pair(dbl,dbl))))'";
    //        }

    //        gp << ", ";
    //        shift += num_v_each-1;

    //        {
    //            std::vector<std::vector<std::vector<double> > > pts(num_u);
    //            for(int u=0; u<num_u; u++) {
    //                pts[u].resize(num_v_each);
    //                for(int v=0; v<num_v_each; v++) {
    //                    pts[u][v].resize(3);
    //                    pts[u][v][0] = get_point(u, v+shift).x;
    //                    pts[u][v][1] = get_point(u, v+shift).y;
    //                    pts[u][v][2] = get_point(u, v+shift).z;
    //                }
    //            }
    //            gp << gp.binFile2d(pts, "record") << "with lines title 'vec vec vec'";
    //        }

    //        gp << ", ";
    //        shift += num_v_each-1;

    //        {
    //            std::vector<std::vector<std::vector<double> > > pts(3);
    //            for(int i=0; i<3; i++) pts[i].resize(num_u);
    //            for(int u=0; u<num_u; u++) {
    //                for(int i=0; i<3; i++) pts[i][u].resize(num_v_each);
    //                for(int v=0; v<num_v_each; v++) {
    //                    pts[0][u][v] = get_point(u, v+shift).x;
    //                    pts[1][u][v] = get_point(u, v+shift).y;
    //                    pts[2][u][v] = get_point(u, v+shift).z;
    //                }
    //            }
    //            gp << gp.binFile2d_colmajor(pts, "record") << "with lines title 'vec vec vec (colmajor)'";
    //        }

#ifdef USE_ARMA
    gp << ", ";
    shift += num_v_each-1;

    {
        arma::cube pts(num_u, num_v_each, 3);
        for(int u=0; u<num_u; u++) {
            for(int v=0; v<num_v_each; v++) {
                pts(u, v, 0) = get_point(u, v+shift).x;
                pts(u, v, 1) = get_point(u, v+shift).y;
                pts(u, v, 2) = get_point(u, v+shift).z;
            }
        }
        gp << gp.file2d(pts) << "with lines title 'arma::cube(U*V*3)'";
    }

    gp << ", ";
    shift += num_v_each-1;

    {
        arma::cube pts(3, num_u, num_v_each);
        for(int u=0; u<num_u; u++) {
            for(int v=0; v<num_v_each; v++) {
                pts(0, u, v) = get_point(u, v+shift).x;
                pts(1, u, v) = get_point(u, v+shift).y;
                pts(2, u, v) = get_point(u, v+shift).z;
            }
        }
        gp << gp.binFile2d_colmajor(pts, "record") << "with lines title 'arma::cube(3*U*V) (colmajor)'";
    }

    gp << ", ";
    shift += num_v_each-1;

    {
        arma::field<MyTriple<double> > pts(num_u, num_v_each);
        for(int u=0; u<num_u; u++) {
            for(int v=0; v<num_v_each; v++) {
                pts(u, v) = get_point(u, v+shift);
            }
        }
        gp << gp.binFile2d(pts, "record") << "with lines title 'arma::field'";
    }
#endif

#ifdef USE_BLITZ
    gp << ", ";
    shift += num_v_each-1;

    {
        blitz::Array<blitz::TinyVector<double, 3>, 2> pts(num_u, num_v_each);
        for(int u=0; u<num_u; u++) {
            for(int v=0; v<num_v_each; v++) {
                pts(u, v)[0] = get_point(u, v+shift).x;
                pts(u, v)[1] = get_point(u, v+shift).y;
                pts(u, v)[2] = get_point(u, v+shift).z;
            }
        }
        gp << gp.binFile2d(pts, "record") << "with lines title 'blitz::Array<blitz::TinyVector<double, 3>, 2>'";
    }

    gp << ", ";
    shift += num_v_each-1;

    {
        blitz::Array<double, 3> pts(num_u, num_v_each, 3);
        for(int u=0; u<num_u; u++) {
            for(int v=0; v<num_v_each; v++) {
                pts(u, v, 0) = get_point(u, v+shift).x;
                pts(u, v, 1) = get_point(u, v+shift).y;
                pts(u, v, 2) = get_point(u, v+shift).z;
            }
        }
        gp << gp.binFile2d(pts, "record") << "with lines title 'blitz<double>(U*V*3)'";
    }

    gp << ", ";
    shift += num_v_each-1;

    {
        blitz::Array<double, 3> pts(3, num_u, num_v_each);
        for(int u=0; u<num_u; u++) {
            for(int v=0; v<num_v_each; v++) {
                pts(0, u, v) = get_point(u, v+shift).x;
                pts(1, u, v) = get_point(u, v+shift).y;
                pts(2, u, v) = get_point(u, v+shift).z;
            }
        }
        gp << gp.binFile2d_colmajor(pts, "record") << "with lines title 'blitz<double>(3*U*V) (colmajor)'";
    }
#endif


    gp << std::endl;

    std::cout << shift+num_v_each << "," << num_v_total << std::endl;
    //assert(shift+num_v_each == num_v_total);
    return 0;
}

void MainWindow::setImage()
{
    QPixmap pixmapTarget = QPixmap("b.png");
    pixmapTarget = pixmapTarget.scaled(this->ui->RossLabel->width(), this->ui->RossLabel->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    this->ui->RossLabel->setPixmap(pixmapTarget);
}



void MainWindow::on_lineEditDimension1_textEdited(const QString &arg1)
{
    Q_UNUSED(arg1)
    this->caracteres = 200;
    this->caracteres -= this->ui->lineEditEpsilon->text().length();
    this->caracteres -= this->ui->lineEditDimension1->text().length();
    this->caracteres -= this->ui->lineEditDimension2->text().length();
    this->caracteres -= this->ui->lineEditDimension3->text().length();
    this->caracteres -= this->ui->lineEditIntervaloX->text().length();
    this->caracteres -= this->ui->lineEditIntervaloY->text().length();
    if(this->caracteres >= 0){
        this->ui->statusBar->showMessage(QString( tr("%1 caracteres restantes") ).arg(this->caracteres));
    }

    else {
        this->ui->statusBar->showMessage(QString( tr("Ha escrito más de 200 caracteres. Borre %1 caracteres para graficar") ).arg(this->caracteres* -1));
    }
    enableButton();
}

void MainWindow::on_lineEditIntervaloX_textEdited(const QString &arg1)
{
    Q_UNUSED(arg1)
    this->caracteres = 200;
    this->caracteres -= this->ui->lineEditEpsilon->text().length();
    this->caracteres -= this->ui->lineEditDimension1->text().length();
    this->caracteres -= this->ui->lineEditDimension2->text().length();
    this->caracteres -= this->ui->lineEditDimension3->text().length();
    this->caracteres -= this->ui->lineEditIntervaloX->text().length();
    this->caracteres -= this->ui->lineEditIntervaloY->text().length();
    if(this->caracteres >= 0){
        this->ui->statusBar->showMessage(QString( tr("%1 caracteres restantes") ).arg(this->caracteres));
    }

    else {
        this->ui->statusBar->showMessage(QString( tr("Ha escrito más de 200 caracteres. Borre %1 caracteres para graficar") ).arg(this->caracteres * -1));
    }
    enableButton();
}

void MainWindow::on_lineEditDimension2_textEdited(const QString &arg1)
{
    Q_UNUSED(arg1)
    this->caracteres = 200;
    this->caracteres -= this->ui->lineEditEpsilon->text().length();
    this->caracteres -= this->ui->lineEditDimension1->text().length();
    this->caracteres -= this->ui->lineEditDimension2->text().length();
    this->caracteres -= this->ui->lineEditDimension3->text().length();
    this->caracteres -= this->ui->lineEditIntervaloX->text().length();
    this->caracteres -= this->ui->lineEditIntervaloY->text().length();
    if(this->caracteres >= 0){
        this->ui->statusBar->showMessage(QString( tr("%1 caracteres restantes") ).arg(this->caracteres));
    }

    else {
        this->ui->statusBar->showMessage(QString( tr("Ha escrito más de 200 caracteres. Borre %1 caracteres para graficar") ).arg(this->caracteres * -1));
    }
    enableButton();
}

void MainWindow::on_lineEditIntervaloY_textEdited(const QString &arg1)
{
    Q_UNUSED(arg1)
    this->caracteres = 200;
    this->caracteres -= this->ui->lineEditEpsilon->text().length();
    this->caracteres -= this->ui->lineEditDimension1->text().length();
    this->caracteres -= this->ui->lineEditDimension2->text().length();
    this->caracteres -= this->ui->lineEditDimension3->text().length();
    this->caracteres -= this->ui->lineEditIntervaloX->text().length();
    this->caracteres -= this->ui->lineEditIntervaloY->text().length();
    if(this->caracteres >= 0){
        this->ui->statusBar->showMessage(QString( tr("%1 caracteres restantes") ).arg(this->caracteres));
    }

    else {
        this->ui->statusBar->showMessage(QString( tr("Ha escrito más de 200 caracteres. Borre %1 caracteres para graficar") ).arg(this->caracteres * -1));
    }
    enableButton();
}

void MainWindow::on_lineEditDimension3_textEdited(const QString &arg1)
{
    Q_UNUSED(arg1)
    this->caracteres = 200;
    this->caracteres -= this->ui->lineEditEpsilon->text().length();
    this->caracteres -= this->ui->lineEditDimension1->text().length();
    this->caracteres -= this->ui->lineEditDimension2->text().length();
    this->caracteres -= this->ui->lineEditDimension3->text().length();
    this->caracteres -= this->ui->lineEditIntervaloX->text().length();
    this->caracteres -= this->ui->lineEditIntervaloY->text().length();
    if(this->caracteres >= 0){
        this->ui->statusBar->showMessage(QString( tr("%1 caracteres restantes") ).arg(this->caracteres));
    }

    else {
        this->ui->statusBar->showMessage(QString( tr("Ha escrito más de 200 caracteres. Borre %1 caracteres para graficar") ).arg(this->caracteres * -1));
    }
    enableButton();
}

void MainWindow::on_lineEditEpsilon_textEdited(const QString &arg1)
{
    Q_UNUSED(arg1)
    this->caracteres = 200;
    this->caracteres -= this->ui->lineEditEpsilon->text().length();
    this->caracteres -= this->ui->lineEditDimension1->text().length();
    this->caracteres -= this->ui->lineEditDimension2->text().length();
    this->caracteres -= this->ui->lineEditDimension3->text().length();
    this->caracteres -= this->ui->lineEditIntervaloX->text().length();
    this->caracteres -= this->ui->lineEditIntervaloY->text().length();
    if(this->caracteres >= 0){
        this->ui->statusBar->showMessage(QString( tr("%1 caracteres restantes") ).arg(this->caracteres));
    }

    else {
        this->ui->statusBar->showMessage(QString( tr("Ha escrito más de 200 caracteres. Borre %1 caracteres para graficar") ).arg(this->caracteres * -1));
    }
    enableButton();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    setImage();
}

void MainWindow::enableButton()
{
    bool campos = true;

    if(this->ui->lineEditEpsilon->text().isEmpty()){
        campos = false;
    }

    else if(this->ui->lineEditDimension1->text().isEmpty() && this->ui->lineEditDimension2->text().isEmpty()){
        campos = false;
    }

    else if(this->ui->lineEditDimension1->text().isEmpty() && this->ui->lineEditDimension3->text().isEmpty()){
        campos = false;
    }

    else if(this->ui->lineEditDimension2->text().isEmpty() && this->ui->lineEditDimension3->text().isEmpty()){
        campos = false;
    }

    else if(this->ui->lineEditDimension1->text().contains('y', Qt::CaseInsensitive)){
        if(this->ui->lineEditIntervaloY->text().isEmpty()){
            campos = false;
        }
    }
    else if(this->ui->lineEditDimension2->text().contains('y', Qt::CaseInsensitive)){
        if(this->ui->lineEditIntervaloY->text().isEmpty()){
            campos = false;
        }
    }
    else if(this->ui->lineEditDimension3->text().contains('y', Qt::CaseInsensitive)){
        if(this->ui->lineEditIntervaloY->text().isEmpty()){
            campos = false;
        }
    }
    else if(this->ui->lineEditDimension1->text().contains('x', Qt::CaseInsensitive)){
        if(this->ui->lineEditIntervaloX->text().isEmpty()){
            campos = false;
        }
    }
    else if(this->ui->lineEditDimension2->text().contains('x', Qt::CaseInsensitive)){
        if(this->ui->lineEditIntervaloX->text().isEmpty()){
            campos = false;
        }
    }
    else if(this->ui->lineEditDimension3->text().contains('x', Qt::CaseInsensitive)){
        if(this->ui->lineEditIntervaloX->text().isEmpty()){
            campos = false;
        }
    }

    else if(this->caracteres < 0){
        campos = false;
    }

    if(this->ui->pushButtonGraficar->isEnabled()){
        if(!campos){
            this->ui->pushButtonGraficar->setDisabled(true);
        }
    }
    else{
        if(campos){
            this->ui->pushButtonGraficar->setEnabled(true);
        }
    }
}

bool MainWindow::caracteresValidos(QString texto)
{
    bool valido = true;
    int indice = 0;
//    QString texto = this->ui->lineEditDimension1->text();
    if(texto.length() > 0){
        while(valido && indice < texto.length()){
            if(texto.at(indice).toLatin1() < 40 && texto.at(indice).toLatin1() != 32){
                valido = false;
            }

            else if(texto.at(indice).toLatin1() > 57 && texto.at(indice).toLatin1() != 99 && texto.at(indice).toLatin1() != 115
                    && texto.at(indice).toLatin1() != 116 && texto.at(indice).toLatin1() != 120 && texto.at(indice).toLatin1() != 121){
                valido = false;
            }

            ++indice;
        }
    }

    return valido;
}

char* MainWindow::datos()
{
    char* array = new char [200 - this->caracteres];
    int indice = 0;
    int caracteres = 0;
    QString texto;
    if(!this->ui->lineEditDimension1->text().isEmpty()){
        texto = this->ui->lineEditDimension1->text();
        while(indice < texto.length()){
            array[caracteres] = texto.at(indice).toLatin1();
            ++indice;
            ++caracteres;
        }
        array[caracteres] = ',';
        ++caracteres;
    }

    if(!this->ui->lineEditDimension2->text().isEmpty()){
        indice = 0;
        texto = this->ui->lineEditDimension2->text();
        while(indice < texto.length()){
            array[caracteres] = texto.at(indice).toLatin1();
            ++indice;
            ++caracteres;
        }
        array[caracteres] = ',';
        ++caracteres;
    }

    if(!this->ui->lineEditDimension3->text().isEmpty()){
        indice = 0;
        texto = this->ui->lineEditDimension3->text();
        while(indice < texto.length()){
            array[caracteres] = texto.at(indice).toLatin1();
            ++indice;
            ++caracteres;
        }
        array[caracteres] = ',';
        ++caracteres;
    }

    if(!this->ui->lineEditIntervaloX->text().isEmpty()){
        indice = 0;
        texto = this->ui->lineEditIntervaloX->text();
        while(indice < texto.length()){
            array[caracteres] = texto.at(indice).toLatin1();
            ++indice;
            ++caracteres;
        }
        array[caracteres] = ',';
        ++caracteres;
    }

    if(!this->ui->lineEditIntervaloY->text().isEmpty()){
        indice = 0;
        texto = this->ui->lineEditIntervaloY->text();
        while(indice < texto.length()){
            array[caracteres] = texto.at(indice).toLatin1();
            ++indice;
            ++caracteres;
        }
        array[caracteres] = ',';
        ++caracteres;
    }

    if(!this->ui->lineEditEpsilon->text().isEmpty()){
        indice = 0;
        texto = this->ui->lineEditEpsilon->text();
        while(indice < texto.length()){
            array[caracteres] = texto.at(indice).toLatin1();
            ++indice;
            ++caracteres;
        }
        array[caracteres] = ',';
        ++caracteres;
    }

    return &(*(array));
}

bool MainWindow::existeCero(QString texto)
{
    bool valido = true;
    //    QString texto = this->ui->lineEditDimension1->text();
    if(texto.at(0).toLatin1() != 48){
        valido = false;
    }

    if(texto.at(1).toLatin1() != 32){
        valido = false;
    }

    if(texto.at(2).toLatin1() != 43 && texto.at(1).toLatin1() != 45){
        valido = false;
    }

    return valido;
}

//if(texto.at(indice).toLatin1() < 39 && texto.at(indice).toLatin1() > 45 && texto.at(indice).toLatin1() < 47 &&
//        texto.at(indice).toLatin1() > 57 && texto.at(indice).toLatin1() != 99 && texto.at(indice).toLatin1() != 115 &&
//        texto.at(indice).toLatin1() != 116 && texto.at(indice).toLatin1() != 120 && texto.at(indice).toLatin1() != 121){


