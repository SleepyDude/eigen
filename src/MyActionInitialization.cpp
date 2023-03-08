#include "MyActionInitialization.h"
#include "MyPrimaryGeneratorAction.h"
#include "MyRunAction.h"
#include "MyRunActionMaster.h"
#include "MyEventAction.h"
#include "MySteppingAction.h"

/// Обязательный класс, который должен быть объявлен в проекте Geant4
/// Имя класса может быть другим, и он долже наследоваться от
/// класса G4VUserActionInitialization
/// Конструктор
MyActionInitialization::MyActionInitialization()
   : G4VUserActionInitialization()
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
