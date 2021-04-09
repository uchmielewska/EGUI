#ifndef MENUDIALOG_H
#define MENUDIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include <QModelIndex>
#include <QStringList>
#include <QDialog>
#include <QStandardItem>
#include <QJsonObject>
#include <QList>
#include <QItemSelectionModel>
#include <QMessageBox>

namespace Ui {
class MenuDialog;
}

class MenuDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MenuDialog(QWidget *parent = nullptr);
    ~MenuDialog();

    QList<QJsonObject> listOfObjectsFromJson;
    QStringList nameOfObjectsFromJson;
    QStringList chosenRecipes;

    void showRecipes();

    void showChosenRecipes();

private slots:
    void on_btnAdd_clicked();

    void on_btnRemove_clicked();

    void calculateShoppingList();

private:
    Ui::MenuDialog *ui;
    QStandardItemModel *model;
    QStandardItemModel *modelChosen;
    QStandardItemModel *modelList;
    QItemSelectionModel *selectionModel;
    QItemSelectionModel *selectionModelChosen;
};

#endif // MENUDIALOG_H
