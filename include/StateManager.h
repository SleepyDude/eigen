#ifndef STATE_MANAGER_H
#define STATE_MANAGER_H

#include <unordered_map>
#include <string>

class StateManager {
public:
    ~StateManager();
    static StateManager* GetStateManager();

    void addFilename(std::string const& key, std::string const& filename);
    std::string const& getFilename(std::string const& key);

private:
    // Singleton specific
    StateManager();
    StateManager(StateManager const&) = delete;
    void operator=(StateManager const&) = delete;

    // User functionality
    void readSettings(std::string const&);
    void createPrefix();
    static int& threadNum() {
        static int theCount = 0;
        return theCount;
    }

    std::unordered_map<std::string, std::string> m_filenames;
    std::string m_prefix;
};

#endif
