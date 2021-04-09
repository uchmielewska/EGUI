#include "menudialog.h"
#include "ui_menudialog.h"

MenuDialog::MenuDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MenuDialog)
{
    ui->setupUi(this);
}

MenuDialog::~MenuDialog(){
    delete ui;
}

void MenuDialog::on_btnAdd_clicked(){
    QItemSelectionModel *selectionModel = ui->availableRecipes->selectionModel();

    QModelIndexList  selectedRow = selectionModel->selectedRows();

    if(selectedRow.size() != 1){
        QMessageBox::critical(this, "Error", "Select one row!");
        return;
    }
    QModelIndex oneSelectedRow = selectedRow.takeLast();
    int chosenRecipeIndex = oneSelectedRow.row();
    QString chosenRecipeName = this->nameOfObjectsFromJson[chosenRecipeIndex];

    //insert elements to table
    this->chosenRecipes.push_back(chosenRecipeName);
    this->showChosenRecipes();

    calculateShoppingList();
}

void MenuDialog::on_btnRemove_clicked(){
    QItemSelectionModel *selectionModelChosen = ui->chosenRecipes->selectionModel();

    QModelIndexList selectedRow = selectionModelChosen->selectedRows();

    if(selectedRow.size() != 1){
        QMessageBox::critical(this, "Error", "Select one row!");
        return;
    }
    QModelIndex oneSelectedRow = selectedRow.takeLast();
    int chosenRecipeIndex = oneSelectedRow.row();
    QString chosenRecipeName = this->chosenRecipes[chosenRecipeIndex];

    for(int i = 0; i < chosenRecipes.length(); i++){
        if(chosenRecipes[i] == chosenRecipeName){
            this->chosenRecipes.removeAt(i);
        }
    }
    this->showChosenRecipes();
}

void MenuDialog::showRecipes(){
    QStringList recipes = this->nameOfObjectsFromJson;

    //new model
    model = new QStandardItemModel(recipes.length(),1,this);

    //insert elements into table
    for(int i = 0; i < recipes.length(); i++){
        QModelIndex index = model->index(i,0,QModelIndex());
        model->setData(index, recipes[i]);
    }
    this->ui->availableRecipes->setModel(model);
    //set header title
    model->setHeaderData(0,Qt::Horizontal,tr("Recipe title"));
    this->ui->availableRecipes->setColumnWidth(0,250);
}

void MenuDialog::showChosenRecipes(){
    //new model
    modelChosen = new QStandardItemModel(chosenRecipes.length(),1,this);

    //insert elements into table
    for(int i = 0; i < chosenRecipes.length(); i++){
        QModelIndex index = modelChosen->index(i,0,QModelIndex());
        modelChosen->setData(index, chosenRecipes[i]);
    }
    this->ui->chosenRecipes->setModel(modelChosen);
    //set header title
    modelChosen->setHeaderData(0,Qt::Horizontal,tr("Chosen Recipes"));
    this->ui->chosenRecipes->setColumnWidth(0,250);
}

void MenuDialog::calculateShoppingList(){

    QStringList chosenKeys;
    QStringList amount;
    for(int i = 0; i < this->chosenRecipes.length(); i++){
        for(int j = 0; j < this->nameOfObjectsFromJson.length(); j++){
            if(this->chosenRecipes[i] == this->nameOfObjectsFromJson[j]){
                //add ingredients
                for(int z = 0, g = 0; z < this->listOfObjectsFromJson[j].keys().length(); z++){
                    if(this->listOfObjectsFromJson[j].keys()[z] != "recipe"){
                        chosenKeys += this->listOfObjectsFromJson[j].keys()[z];
                        while(g < chosenKeys.length()){
                            amount += this->listOfObjectsFromJson[j].value(chosenKeys[g]).toString();
                            g++;
                        }
                    }
                }
            }
        }
    }

    modelList = new QStandardItemModel(chosenKeys.length(),2,this);
    //go through all ingredients
    for(int k = 0; k < chosenKeys.length(); k++){
        QModelIndex index = modelList->index(k,0,QModelIndex());
        modelList->setData(index, chosenKeys[k]);
        index = modelList->index(k,1,QModelIndex());
        modelList->setData(index, amount[k]);
    }
    this->ui->shopList->setModel(modelList);
    modelList->setHeaderData(0,Qt::Horizontal,tr("Products"));
    modelList->setHeaderData(1,Qt::Horizontal,tr("Amount with unit"));
    this->ui->shopList->setColumnWidth(0,120);
    this->ui->shopList->setColumnWidth(1,120);
}


