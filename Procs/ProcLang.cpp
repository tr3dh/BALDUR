#include "templateDecls.h"

#undef LOG
#undef _ERROR

#define LOG std::cout
#define _ERROR std::cerr

std::string getExecutablePath() {
    
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string fullPath(buffer);
    return fullPath;
}

std::string getExecutableDir() {

    std::string fullPath = getExecutablePath();
    size_t pos = fullPath.find_last_of("\\/");
    if (pos != std::string::npos) {
        return fullPath.substr(0, pos);
    }
    return "";
}

int main(int argc, char* argv[]){

    // argc ist anzahl übergebener Argumente + 1
    // argv dann das array der größe argc die die übergebene Argumente aus char Ptrs enthält
    // um auf n-tes übergebenes argument zu kommen argv[n] abfragen -> Indizierung ab 1 nicht ab Null

    mkdir("../bin");

    openLogFile();

    // Programm ohne args gestartet
    if(argc < 2){

        LOG << "!! API wurde ohne Argumente angesprochen, invalider Aufruf" << ENDL;
        LOG << "   um mehr zu Erfahren die API mit dem Argument '--help' aufrufen" << ENDL;

        return 1;
    }

    std::string exeDir = getExecutableDir();
    std::string cwd = std::filesystem::current_path().string();

    std::unordered_map<std::string, std::string> options = {
        {"--license", "view license >> 0 args requiered"},
        {"--version", "view version >> 0 args requiered"},
        {"execute", "execute given Script"}
    };

    if(std::string(argv[1]) == "--help"){

        LOG << "Valid Commandline options are :" << ENDL;
        LOG << ENDL;

        for(const auto& [cm, desc] : options){
            LOG << "\t" << cm << "\t\t" << desc << ENDL;
        }

        return 0;
    }

    else if(std::string(argv[1]) == "--version"){

        std::ifstream file(exeDir + "/../.VERSION");
        if (!file) {
            _ERROR << "keine gültige Version gefunden an " << exeDir + "/../.VERSION" << ENDL;
            return 1;
        }

        std::string line;
        while (std::getline(file, line)) {
            LOG << line << ENDL;
        }

        return 0;
    }

    else if(std::string(argv[1]) == "--license"){
        
        std::ifstream file(exeDir + "/../LICENSE");
        if (!file) {
            _ERROR << "keine gültige Lizenz gefunden an " << exeDir + "/../LICENSE" << ENDL;
            return 1;
        }

        LOG << ENDL;

        std::string line;
        while (std::getline(file, line)) {
            LOG << "\t" << line << ENDL;
        }

        return 0;
    }
    // else if((argc - 1) % 2 != 0) {

    //     LOG << "!! API wurde ohne valide Argumente angesprochen, invalider Aufruf" << ENDL;
    //     LOG << "   um mehr zu Erfahren die API mit dem Argument '--help' aufrufen" << ENDL;

    //     return 1;
    // }

    // //
    // std::map<std::string, std::string> kwargs;

    // //
    // for(int kwargIdx = 1; kwargIdx < argc - 1; kwargIdx += 2){

    //     kwargs.try_emplace(argv[kwargIdx], argv[kwargIdx+1]);
    // }

    // LOG << kwargs << endl;

    if(std::string(argv[1]) == "execute"){

        RETURNING_ASSERT(argc > 2, "bitte Pfad des Modells für die Simulation übergeben", 1);

        std::string path;

        if(argc > 3 && std::string(argv[3]) == "--absolute" || string::startsWith(std::string(argv[2]), "C:")){
            path = argv[2];
        }
        else{
            path = cwd + "/" + argv[2];
        }

        RETURNING_ASSERT(fs::exists(path), "Script existiert nicht", 1);

        std::filesystem::current_path(exeDir);
        // LOG << "Arbeitsverzeichnis auf " << exeDir << " gesetzt" << ENDL;
        // LOG << "Start Unpacking and Simulating " << path << ENDL;

        auto result = executeProgram(path);
    }

    else if(std::string(argv[1]) == "package"){

        RETURNING_ASSERT(argc > 2, "bitte Pfad des Modells für die Simulation übergeben", 1);

        std::string path;

        if(argc > 3 && std::string(argv[3]) == "--absolute" || string::startsWith(std::string(argv[2]), "C:")){
            path = argv[2];
        }
        else{
            path = cwd + "/" + argv[2];
        }

        RETURNING_ASSERT(fs::exists(path), "Script existiert nicht", 1);

        std::filesystem::current_path(exeDir);
        // LOG << "Arbeitsverzeichnis auf " << exeDir << " gesetzt" << ENDL;
        // LOG << "Start Unpacking and Simulating " << path << ENDL;

        auto result = executeProgram(path);
    }

    // Programm ohne args gestartet
    else {

        LOG << "!! API wurde ohne valide Argumente angesprochen, invalider Aufruf" << ENDL;
        LOG << "   um mehr zu Erfahren die API mit dem Argument '--help' aufrufen" << ENDL;

        return 1;
    }

    closeLogFile();

    return 0;
}