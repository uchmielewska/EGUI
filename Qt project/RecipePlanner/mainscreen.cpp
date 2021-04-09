#include "mainscreen.h"
#include "ui_mainscreen.h"
#include "entrydialog.h"
#include "menudialog.h"
#include <QFile>
#include <QMessageBox>
#include <QAction>

MainScreen::MainScreen(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainScreen)
{
    ui->setupUi(this);

    //open file
    readRecipesFromJson();

    //file objects into table
    showRecipesList(this->listOfObjectsFromJson, this->nameOfObjectsFromJson);
}

MainScreen::~MainScreen(){
    delete ui;
}

void MainScreen::on_btnAdd_clicked(){
    EntryDialog* dlgAdd = new EntryDialog(this);
    dlgAdd->show();

    dlgAdd->listOfObjectsFromJson = this->listOfObjectsFromJson;
    dlgAdd->nameOfObjectsFromJson = this->nameOfObjectsFromJson;

    dlgAdd->boolAddingNew = true;

    connect(dlgAdd, SIGNAL(recipeViewCloses()), this, SLOT(updateRecipeList()));
}

void MainScreen::on_btnEdit_clicked()
{
    //new selection model
    QItemSelectionModel *selectionModel = ui->listOfRecipes->selectionModel();

    //get higlighted row
    QModelIndexList selectedRows = selectionModel->selectedRows();

    //error if num of rows is not 1
    if (selectedRows.size() != 1){
        QMessageBox::critical(this, "Error", "Select one recipe to edit. To select, click on the row index.");
        return;
    }

    //take selected row
    QModelIndex selectedRow = selectedRows.takeLast();

    //new object
    EntryDialog *dlgEdit = new EntryDialog(this);
    dlgEdit->show();

    //load existing elements
    dlgEdit->listOfObjectsFromJson = this->listOfObjectsFromJson;
    dlgEdit->nameOfObjectsFromJson = this->nameOfObjectsFromJson;

    //we are NOT adding new, we are editing
    dlgEdit->boolAddingNew = false;

    connect(dlgEdit, SIGNAL(recipeViewCloses()), this, SLOT(updateRecipeList()));

    //push selected id
    dlgEdit->idToEdit = selectedRow.row();

    //create new object from highlighted one
    dlgEdit->newObject = this->listOfObjectsFromJson.takeAt(selectedRow.row());

    //selected object name
    dlgEdit->newObjectName = this->nameOfObjectsFromJson.takeAt(selectedRow.row());

    //fill labels with describtion
    dlgEdit->fillTitleAndDescription();

    //reload table
    dlgEdit->fillIngredients();
}


void MainScreen::on_btnDelete_clicked(){

    //selection model
    QItemSelectionModel *selectionModel = ui->listOfRecipes->selectionModel();

    //find selected rows
    QModelIndexList selectedRows = selectionModel->selectedRows();

    //if wrong number of rows is selected
    if(selectedRows.size() != 1){
        QMessageBox::critical(this, "Error", "Select one recipe to delete.");
        return;
    }

    //take selected row
    QModelIndex selectedRow = selectedRows.takeLast();

    //message box
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Delete","Are you sure you want to delete the selected item?", QMessageBox::Yes | QMessageBox::No);

    //delete if "Yes", nothing happens if "No"
    if (reply == QMessageBox::Yes){
        qDebug("Yes");
        deleteRecipe(selectedRow.row());
    } else{
        qDebug("No");
    }
}

void MainScreen::readRecipesFromJson(){
    QFile file("../recipes.json");

    //failed to open file
    if (!file.open(QIODevice::ReadWrite)){
        qDebug("File NOT open!");

        //show message in MessageBox
        QMessageBox::critical(this, "Error", "JSON file not open!");
        return;
    }

    //create json document
    QJsonDocument recipesJson = QJsonDocument::fromJson(file.readAll());

    if(!recipesJson.isNull()){
        if(recipesJson.isObject()){
            //take object from document
            QJsonObject recipesTitlesJson = recipesJson.object();

            //to go through objects
            QJsonObject::iterator it;

            //to store json object keys and values separately
            QList<QJsonObject> listOfObjects;
            QStringList nameOfObjects;

            for(it = recipesTitlesJson.begin(); it != recipesTitlesJson.end(); ++it){
                //store objects keys
                nameOfObjects.push_back(it.key());

                //store object value
                QJsonObject objectInside = it.value().toObject();

                //push objects into list
                listOfObjects.push_back(objectInside);
            }

            //store variables imported from json in program
            this->listOfObjectsFromJson = listOfObjects;
            this->nameOfObjectsFromJson = nameOfObjects;
        } else{
            //json file empty
            qDebug("Json file is empty");
        }
    } else{
          QMessageBox::critical(this, "Error", "Json file not imported!");
          return;
        }
    //close file
    file.close();
}

void MainScreen::showRecipesList(QList<QJsonObject> array, QStringList names){
    //new model
    model = new QStandardItemModel(names.length(),3,this);

    //row and column loop
    for(int row = 0; row < names.length(); ++row){
        for(int col = 0; col < 3; col++){

            //index
            QModelIndex index = model->index(row,col,QModelIndex());

            //take object from list
            QJsonObject object = array[row];

            //name of object (0), recipe (1), ingredients (other)
            if(col==0){
                model->setData(index,names[row]);
            } else if (col == 1){
                QJsonArray recipeLines = object.value("recipe").toArray();
                QString stringRecipeToInsert = "";

                for(const auto obj : recipeLines){
                    stringRecipeToInsert += obj.toString() + "\n";
                }
                model->setData(index,stringRecipeToInsert);
            } else{
                QStringList keyStrings = object.keys();
                QString stringIngredientsToInsert = "";

                for(int i=0; i < keyStrings.length(); i++){
                    if(keyStrings[i] != "recipe"){
                        stringIngredientsToInsert = stringIngredientsToInsert + keyStrings[i] + " " + object.value(keyStrings[i]).toString() + "\n";
                    }
                }
                model->setData(index,stringIngredientsToInsert);
            }
        }
    }

    //set headers
    model->setHeaderData(0,Qt::Horizontal,tr("Title"));
    model->setHeaderData(1,Qt::Horizontal,tr("Recipe"));
    model->setHeaderData(2,Qt::Horizontal,tr("Ingredients"));

    //model into QTableView
    ui->listOfRecipes->setModel(model);

    //columns width
    ui->listOfRecipes->setColumnWidth(0,100);
    ui->listOfRecipes->setColumnWidth(1,370);
    ui->listOfRecipes->setColumnWidth(2,270);

    //word wprapping true
    ui->listOfRecipes->setWordWrap(true);

    //set row height
    ui->listOfRecipes->verticalHeader()->setDefaultSectionSize(200);

    //set window size
    ui->listOfRecipes->setMinimumSize(780,430);
}

void MainScreen::deleteRecipe(int nrOfRow){

    //remove object from vectors which stores data
    this->listOfObjectsFromJson.removeAt(nrOfRow);
    this->nameOfObjectsFromJson.removeAt(nrOfRow);

    //temporary Entry Dialog object
    EntryDialog *dlgToRemove = new EntryDialog(this);

    //save remaining recipes in EntryDialog
    dlgToRemove->listOfObjectsFromJson = this->listOfObjectsFromJson;
    dlgToRemove->nameOfObjectsFromJson = this->nameOfObjectsFromJson;

    //reload Json file with remaining objects
    dlgToRemove->saveRecipeToJson();

    //refresh table
    showRecipesList(this->listOfObjectsFromJson, this->nameOfObjectsFromJson);
}

void MainScreen::on_actionCreate_shopping_list_triggered(){
    //create new object
    MenuDialog *newMenuDialog = new MenuDialog(this);
    //show window
    newMenuDialog->show();

    //pass arguments
    newMenuDialog->listOfObjectsFromJson = this->listOfObjectsFromJson;
    newMenuDialog->nameOfObjectsFromJson = this->nameOfObjectsFromJson;

    newMenuDialog->showRecipes();
}

void MainScreen::updateRecipeList(){
    //open file
    readRecipesFromJson();

    //file objects into table
    showRecipesList(this->listOfObjectsFromJson, this->nameOfObjectsFromJson);
}

