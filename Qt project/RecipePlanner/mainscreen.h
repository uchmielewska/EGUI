#ifndef MAINSCREEN_H
#define MAINSCREEN_H

#include <QMainWindow>
#include <QCalendarWidget>
#include <QVector>
#include <QJsonDocument>
#include <QJsonValue>
#include <QFile>

#include <QHBoxLayout>
#include <QJsonArray>
#include <QDate>

#include <QStandardItemModel>
#include <QJsonObject>


QT_BEGIN_NAMESPACE
namespace Ui { class MainScreen; }
QT_END_NAMESPACE

class MainScreen : public QMainWindow
{
    Q_OBJECT

public:
    MainScreen(QWidget *parent = nullptr);
    ~MainScreen();

    QList<QJsonObject> listOfObjectsFromJson;
    QStringList nameOfObjectsFromJson;

private slots:
    void on_btnAdd_clicked();

    void on_btnDelete_clicked();

    void on_btnEdit_clicked();

    void readRecipesFromJson();

    void showRecipesList(QList<QJsonObject> array, QStringList names);

    void deleteRecipe(int nrOfRow);

    void on_actionCreate_shopping_list_triggered();

    void updateRecipeList();

private:
    Ui::MainScreen *ui;
    QJsonObject m_recipes;
    QStandardItemModel *model;
};
#endif // MAINSCREEN_H
