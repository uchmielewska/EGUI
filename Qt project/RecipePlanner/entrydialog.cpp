#include "entrydialog.h"
#include "ui_entrydialog.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QJsonDocument>

EntryDialog::EntryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EntryDialog)
{
    ui->setupUi(this);
}

EntryDialog::~EntryDialog(){
    delete ui;
}

void EntryDialog::on_btnOk_clicked(){
    //placeholder for recipe
    QStringList newObjectRecipe;

    //set name
    this->newObjectName = ui->titleRecipe->text();

    //recipe separated with newline symbol stored in list
    newObjectRecipe = ui->txtRecipe->toPlainText().split('\n');

    //placeholder for recipe in array
    QJsonArray newObjectRecipeArray;
    QJsonValue value;

    //place recipe from list into jsonarray
    for(int i = 0; i < newObjectRecipe.count(); i++) {

        value = newObjectRecipe.at(i);
        newObjectRecipeArray.push_back(value);
    }

    //insert recipe into newObject
    this->newObject.insert("recipe",newObjectRecipeArray);

    if(this->boolAddingNew) { //we are adding object

        //add object and its name to program list of objects
        this->listOfObjectsFromJson.push_back(this->newObject);
        this->nameOfObjectsFromJson.push_back(this->newObjectName);

        this->saveRecipeToJson();
        this->hide();
        emit recipeViewCloses();

    } else { //we are editing an existing object

        //remove object at given id
        this->listOfObjectsFromJson.removeAt(this->idToEdit);
        this->nameOfObjectsFromJson.removeAt(this->idToEdit);

        //insert objecy at given id
        this->listOfObjectsFromJson.insert(this->idToEdit,this->newObject);
        this->nameOfObjectsFromJson.insert(this->idToEdit,this->newObjectName);

        this->saveRecipeToJson();
        this->hide();
        emit recipeViewCloses();
    }
}

void EntryDialog::on_btnCancel_clicked(){
    close();
}

void EntryDialog::saveRecipeToJson(){
    //new file
    QFile file("../recipes.json");

    //object to store products
    QJsonObject productsInList;

    //insert all products in loop
    for(int i = 0; i < this->nameOfObjectsFromJson.size(); i++){
        productsInList.insert(nameOfObjectsFromJson[i], listOfObjectsFromJson[i]);
    }

    //creates json document from object
    QJsonDocument document( productsInList );

    //check file
    if(!file.open(QIODevice::WriteOnly)){
        qDebug("Failed to open file");
        return;
    }

    //overwrite existing file and close
    file.write(document.toJson());
    file.close();
}

void EntryDialog::fillIngredients(){

    QStringList keys = this->newObject.keys();

    for (int i = 0; i < keys.length(); i++){
        if(keys[i] == "recipe")
            keys.removeAt(i);
    }

    //new model
    model = new QStandardItemModel(keys.length(),2,this);

    //fill ingredients
    for(int i = 0; i < keys.length(); ++i){
        for(int j = 0; j < 2; ++j){
           QModelIndex index = model->index(i, j, QModelIndex());

           if(j == 0){
                   model->setData(index, keys[i]);
           } else{
                    QString amountAndUnit = this->newObject.value(keys[i]).toString();
                    model->setData(index, amountAndUnit);
           }
       }
    }
    //set table headers
    model->setHeaderData(0,Qt::Horizontal,tr("Products"));
    model->setHeaderData(1,Qt::Horizontal,tr("Amount and unit"));

    //set model
    this->ui->ingredientsList->setModel(model);

    //set size
    this->ui->ingredientsList->setColumnWidth(0,255);
    this->ui->ingredientsList->setColumnWidth(1,255);
}

void EntryDialog::fillTitleAndDescription(){
    //fill title
    this->ui->titleRecipe->setText(this->newObjectName);

    //fill recipe
    QJsonArray recipeLines = this->newObject.value("recipe").toArray();
    QString recipe;

    //fill recipe to variable
    for(int i = 0; i < recipeLines.size(); i++)
        recipe += recipeLines[i].toString() + "\n";

    //fill recipe
    this->ui->txtRecipe->setText(recipe);
}

void EntryDialog::on_btnAdd_clicked(){
    QString product = this->ui->product->text();
    QString amount = this->ui->amount->text();
    QString unit = this->ui->unit->text();

    //checking errors
    if(product.isEmpty() || amount.isEmpty() || unit.isEmpty()){
        QMessageBox::critical(this, "Error", "Do not leave empty fields!");
        return;
    }
    if(!amount.toDouble() || amount.toInt()){
        QMessageBox::critical(this, "Error", "Amount has to be a float number");
        return;
    }
    if(product == "recipe"){
        QMessageBox::critical(this, "Error", "You can't call product as 'recipe'!");
        return;
    }

    this->newObject.insert(product, amount + " " + unit);
    this->fillIngredients();

    //clear boxes
    this->ui->product->clear();
    this->ui->amount->clear();
    this->ui->unit->clear();
}

void EntryDialog::on_btnRemove_clicked(){
    QItemSelectionModel *selModel = this->ui->ingredientsList->selectionModel();

    QModelIndexList rows = selModel->selectedRows();

    //if wrong number of rows is selected
    if(rows.size() != 1){
        QMessageBox::critical(this, "Error", "Select one row with ingredient to delete.");
        return;
    }

    QModelIndex row = rows.takeLast();

    //message box
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Delete","Are you sure you want to delete the selected ingredient?", QMessageBox::Yes | QMessageBox::No);
    //delete if "Yes", nothing happens if "No"
    if (reply == QMessageBox::Yes){
        qDebug("Yes");
        this->deleteIngredient(row.row());
    } else{
        qDebug("No");
    }
}

void EntryDialog::deleteIngredient(int rowIndex){
    //remove ingredient
    this->newObject.remove(this->model->takeItem(rowIndex,0)->text());

    //fill updated ingredients table
    this->fillIngredients();
}
