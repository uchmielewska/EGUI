#ifndef ENTRYDIALOG_H
#define ENTRYDIALOG_H

#include <QDialog>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardItemModel>
#include <QString>
#include <QItemSelectionModel>

namespace Ui {
class EntryDialog;
}

class EntryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EntryDialog(QWidget *parent = nullptr);
    ~EntryDialog();

    QList<QJsonObject> listOfObjectsFromJson;
    QStringList nameOfObjectsFromJson;

    void saveRecipeToJson();

    void fillTitleAndDescription();

    void fillIngredients();

    int idToEdit;
    int howManyIngreedients;

    QJsonObject newObject;
    QString newObjectName;

    bool boolAddingNew;

signals:
   void recipeViewCloses();

private slots:

    void on_btnOk_clicked();

    void on_btnCancel_clicked();

    void on_btnAdd_clicked();

    void on_btnRemove_clicked();

    void deleteIngredient(int rowIndex);

private:
    Ui::EntryDialog *ui;
    QStandardItemModel *model;
    QItemSelectionModel *selModel;
};

#endif // ENTRYDIALOG_H
