#include "MyActionInitialization.h"
#include "MyPrimaryGeneratorAction.h"
#include "MyRunAction.h"
#include "MyRunActionMaster.h"
#include "MyEventAction.h"
#include "MySteppingAction.h"
#include "MyConstruction.h"


/// Обязательный класс, который должен быть объявлен в проекте Geant4
/// Имя класса может быть другим, и он долже наследоваться от
/// класса G4VUserActionInitialization
/// Конструктор
MyActionInitialization::MyActionInitialization(MyConstruction* detConstruction)
   : G4VUserActionInitialization(),
     m_detConstruction(detConstruction)
{}

//Деструктор
MyActionInitialization::~MyActionInitialization()
{}

void MyActionInitialization::BuildForMaster() const {
    SetUserAction(new MyRunActionMaster);
}


void MyActionInitialization::Build() const
{
    SetUserAction(new MyPrimaryGeneratorAction);
    SetUserAction(new MyEventAction);
    SetUserAction(new MySteppingAction);

    if ( G4Threading::IsMultithreadedApplication() )
        SetUserAction(new MyRunAction);
    else
        SetUserAction(new MyRunActionMaster);

}
