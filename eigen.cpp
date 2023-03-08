#include "G4MTRunManager.hh"
#include "G4UImanager.hh"

#include "QGSP_BERT_HP.hh"

#define G4UI_USE
#define G4MULTITHREADED1

#ifdef G4UI_USE //Если используется интерфейс пользователя
    #include "G4VisExecutive.hh"//Визуализация
    #include "G4UIExecutive.hh"//Выбор соответствуещего интерфейса пользователя
#endif

#include "MyActionInitialization.h"
#include "MyConstruction.h"

int main(int argc,char** argv)
{
    CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
    CLHEP::HepRandom::setTheSeed(time(NULL));//Устанавливаем зерно для генератора

#ifdef G4MULTITHREADED1
    G4MTRunManager* runManager = new G4MTRunManager;
    runManager->SetNumberOfThreads(G4Threading::G4GetNumberOfCores());
    G4cout << "Using " << G4Threading::G4GetNumberOfCores() << "cores" << G4endl;
#else
    G4RunManager* runManager = new G4RunManager;
#endif
    runManager->SetVerboseLevel(0);
    // Геометрия детектора
    auto detConstruction = new MyConstruction;
//    detConstruction->RegisterParallelWorld(new MyParallelWorld("ScoringWorld"));
    runManager->SetUserInitialization(detConstruction);
    // ФизЛист QGSP_BERT_HP
	G4VModularPhysicsList* physList = new QGSP_BERT_HP;
//    physList->RegisterPhysics(new G4ParallelWorldPhysics("ScoringWorld"));

    runManager->SetUserInitialization(physList);
    // action
    auto actionInitialization = new MyActionInitialization();
    runManager->SetUserInitialization(actionInitialization);
    runManager->Initialize();
 #ifdef G4UI_USE
    G4VisManager* visManager = new G4VisExecutive;
    visManager->SetVerboseLevel(0);
    visManager->Initialize();
 #endif
    G4UImanager* UImanager = G4UImanager::GetUIpointer();
    if ( argc == 1 ) {
        #ifdef G4UI_USE
            G4UIExecutive* ui = new G4UIExecutive(argc, argv);//Создание интерфейса пользователя
            //G4UIExecutive* ui = new G4UIExecutive(argc, argv, "csh");//Создание интерфейса пользователя
            UImanager->ApplyCommand("/control/execute vis.mac");//Отрисовка по умолчанию
            ui->SessionStart();//Запуск интерфейса пользователя
            delete ui;//Удаление интерфейса пользователя
        #endif
    }
    else {
        G4String command = "/control/execute ";//Команда выполнить
        G4String fileName = argv[1];//Имя файла из командной строки при запуске проекта
        UImanager->ApplyCommand(command+fileName);//Выполнение команды
    }
    // Окончание работы, вызов деструктора (удаление) G4RunManager
    delete runManager;
    return 0;
}
