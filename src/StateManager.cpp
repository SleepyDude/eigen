#include "StateManager.h"
#include "G4AutoDelete.hh"
#include "G4AutoLock.hh"
#include "Utils.h"
#include <fstream>
#include <sstream>

#define DEBUGSM 

G4ThreadLocal StateManager* the_stateManager = 0;

static G4Mutex rMutex;

StateManager* StateManager::GetStateManager() {
    if (!the_stateManager) {
        the_stateManager = new StateManager();
        #ifdef DEBUGSM
            G4cout << the_stateManager->m_prefix << "Created" << G4endl;
        #endif
        the_stateManager->createPrefix();
        the_stateManager->readSettings("settings.mac");
        G4AutoDelete::Register(the_stateManager);
    }
    return the_stateManager;
}

StateManager::StateManager() {}
StateManager::~StateManager() {}

void StateManager::addFilename(std::string const& key, std::string const& filename) {
    m_filenames[key] = filename;
}

std::string const& StateManager::getFilename(std::string const& key) {
    return m_filenames[key];
}

/*
    Read settings from file `fn`
    If string in file looks like:
        SET <key> <value>
    Then key and value will be in filenames
*/
void StateManager::readSettings(std::string const& fn) {
    G4AutoLock lock(&rMutex);

    std::ifstream inputFileStream(fn.c_str());

    if (inputFileStream.fail()) {
        G4cout << m_prefix << "Can't open setting file " << fn << G4endl;
        return;
    } else {
        G4cout << m_prefix << "Settings got from " << fn.c_str() << G4endl;
    }

    for (std::string line; std::getline(inputFileStream, line);) {
        if (line[0] != '#') {
            std::vector<std::string> tokens;
            ::tokenize(line, tokens, " ", true);
            if (tokens[0] == "SET") {
                m_filenames[tokens[1]] = tokens[2];
            }
        }
    }
}

void StateManager::createPrefix() {
    int thNum = threadNum()++;
    auto myid = G4ThisThread::get_id();
    std::stringstream ss;
    ss << myid;
    m_prefix = "[StateManager id:" + ss.str() +\
               " num:" + std::to_string(thNum) + "] ";
}