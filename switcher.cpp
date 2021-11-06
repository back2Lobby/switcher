#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <iomanip>
#include <windows.h>
using namespace std;

void copyCharArray(char*a[],char*b[],int size){
    for(int i=0;i<size;i++){
        a[i] = b[i];
    }
}

string toRawString(std::string const& in)
{
   string ret = in;
   auto p = ret.find('\t');
   if ( p != ret.npos )
   {
      ret.replace(p, 1, "\\t");
   }

   return ret;
}

string join(vector<string> str,string separator){
    string res = "";
    for(int i=0;i<str.size();i++){
        if(str[i].length() > 0){
            res += str[i] + separator;
        }
    }
    return res;
}

vector<string> split(string str,string delimiter){
    vector<string> splitted;
    int pos = 0;
    string token;
    while((pos = str.find(delimiter)) != string::npos){
        token = str.substr(0,pos);
        splitted.push_back(token);
        str.erase(0,pos + delimiter.length());
    }
    splitted.push_back(str);

    return splitted;
}

class PHP {
    public:
        string path;
        string version;
        static const string VERSION_KEY;
        static const string PATH_KEY;
        PHP (string version,string path){
            this->version = version;
            this->path = path;
        }
};
const string PHP::VERSION_KEY = "--version";
const string PHP::PATH_KEY = "--path";

struct Color{
    string name;
    int code;
};

class Output{
    public:
        int defaultColor;
        HANDLE hConsole;

        Output(){
            this->hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

            CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
            GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbiInfo);
            this->defaultColor = csbiInfo.wAttributes;

            this->setColors();
        }
        string changeColor(string str,string color){
            Color c = this->getColor(color);

            if(c.name != ""){
                SetConsoleTextAttribute(this->hConsole, c.code);

                // return c.code+str+this->getColor("reset").code;
                cout << str;

                // reset color to default terminal color
                SetConsoleTextAttribute(this->hConsole, this->defaultColor);
            }

            return "";
        }
    private:
        Color colors[4];
        void setColors(){
            Color c1,c2,c3,c4;
            c1.name = "reset";
            c1.code = this->defaultColor;
            c2.name = "red";
            c2.code = 4;
            c3.name = "green";
            c3.code = 2;
            c4.name = "blue";
            c4.code = 1;
            colors[0] = c1;
            colors[1] = c2;
            colors[2] = c3;
            colors[3] = c4;
        }
        Color getColor(string name){
            for(int i=0;i<4;i++){
                if(this->colors[i].name == name){
                    return this->colors[i];
                }
            }
            Color empColor;
            empColor.name = "default";
            empColor.code = this->defaultColor;
            return empColor;
        }
};

class FileManager{
    public:
        vector<PHP>availableVersions;
        FileManager(){
            this->setupAvailableVersions();
        }
        void addLine(string filename,string line){
                //create if file doesn't exists and append anything to its end
            fstream file(filename,ios::ate | ios::app);
                // file empty of not
                if(file.tellg() == 0){
                    file << line;
                }else{
                    file << "\n"+line;
                }
            file.close();
        }
        void removeVersion(string version){
            // update the vector
            for(int i=0;i<this->availableVersions.size();i++){
                if(this->availableVersions[i].version == version){
                    this->availableVersions.erase(this->availableVersions.begin()+i);
                    break;
                }
            }
            // update the db file
            this->updateDatabase();
        }
    private:
        // load all the saved php versions from database to the array/vector "availableVersions"
        void setupAvailableVersions(){
            fstream dbfile;
            dbfile.open("database.txt",ios::in);
            if(dbfile.is_open()){
                string tp;
                while(getline(dbfile,tp)){
                    tp = toRawString(tp);
                    if(tp.length() > 0){
                        string delimiter = "<===>";
                        string version = tp.substr(0,tp.find("<===>"));
                        string path = tp.substr(tp.find("<===>")+delimiter.length(),tp.length());
                        if(version.length() > 0 && path.length() > 0){
                            PHP p(version,path);
                            this->availableVersions.push_back(p);
                        }
                    }
                }

                dbfile.close();
            }
        }
        void updateDatabase(){
            fstream file("database.txt",fstream::out | fstream::trunc);
                for(int i=0;i<this->availableVersions.size();i++){
                    file << this->availableVersions[i].version+"<===>"+this->availableVersions[i].path+"\n";
                }
            file.close();
        }
};

class Registrar{
    public:
        vector<string> all_path_envs;
        FileManager *fileManager;
        Output *output;

        Registrar(FileManager *fm,Output *op){
            this->fileManager = fm;
            this->output = op;
            this->loadPathEnv();
        }

        //loads the user path env variable and saves it as vector in "all_path_envs"
        void loadPathEnv(){
            string path_env = this->registry_read(TEXT("Environment"),TEXT("Path"),REG_EXPAND_SZ);

            vector<string> all_paths = split(path_env,";");

            for(int i=0;i<all_paths.size();i++){
                this->all_path_envs.push_back(all_paths[i]);
            }
        }

        string registry_read (LPCTSTR subkey,LPCTSTR name,DWORD type)
        {
            HKEY key;
            char value[1024];
            DWORD value_length = 1024;
            RegOpenKey(HKEY_CURRENT_USER,subkey,&key);
            RegQueryValueEx(key,name, NULL, &type, (LPBYTE)value, &value_length);
            RegCloseKey(key);
            string str(value);
            return str;
        }
        void registry_write (LPCTSTR subkey,LPCTSTR name,DWORD type,const char* value)
        {
            HKEY key;
            RegOpenKey(HKEY_CURRENT_USER,subkey,&key);
            RegSetValueEx(key, name, 0, type, (LPBYTE)value, strlen(value)*sizeof(char));
            RegCloseKey(key);
        }
        void deactivate_all_versions(bool save = false){
            for(int i=0;i < this->all_path_envs.size();i++){
                for(int j=0;j< this->fileManager->availableVersions.size();j++){
                    if(this->all_path_envs[i] == this->fileManager->availableVersions[j].path){

                        this->all_path_envs.erase(this->all_path_envs.begin()+i);

                        cout << this->output->changeColor("INFO: ","blue") + "Version "+ this->fileManager->availableVersions[j].version + " has been deactivated" << endl;
                    }
                }
            }

            if(save){
                this->save_new_path();
            }
        }

        PHP* current_activated_version(){
            for(int i=0;i<this->all_path_envs.size();i++){
                for(int j=0;j< this->fileManager->availableVersions.size();j++){
                    if(this->all_path_envs[i] == this->fileManager->availableVersions[j].path){
                        return new PHP(this->fileManager->availableVersions[j].version,this->fileManager->availableVersions[j].path);
                    }
                }
            }
            return new PHP("","");
        }

        void save_new_path(){
            string new_path = join(this->all_path_envs,";");
            this->registry_write(TEXT("Environment"),TEXT("Path"),REG_EXPAND_SZ,new_path.c_str());
        }

        void activate_version(PHP php){

            this->deactivate_all_versions();

            this->all_path_envs.push_back(php.path);

            this->save_new_path();

            cout << this->output->changeColor("SUCCESS: ","green") + "Activated Version " + php.version;
            cout << endl << "(Please Open A New Terminal To See Changes)";
        }

};

class Manager;

class Validation{
    public:
        static PHP validatePHPVersion(Manager *manager,string version);
        static bool alreadyExistCheck(Manager *manager,string version,bool shouldExistOrNot);
        static void isNotActivatedCheck(Manager *manager,string version);
};

class Manager{
    public:
        Output *output;
        int sizeOfArgs;
        Manager(char* allArgs[],int argsSize){
            copyCharArray(this->args,allArgs,argsSize);
            this->sizeOfArgs = argsSize;
        }
        void setup(){
            this->output = new Output;
            this->fileManager = new FileManager;
            this->registrar = new Registrar(this->fileManager,this->output);
        }
        void addPHP(){
            string version = this->getArgumentValue(this->getArgument(PHP::VERSION_KEY),10);
            string path = this->getArgumentValue(this->getArgument(PHP::PATH_KEY));

            Validation::alreadyExistCheck(this,version,false);

            this->php = new PHP(version,path);

            this->fileManager->addLine("database.txt",this->php->version+"<===>"+this->php->path);

            cout << this->output->changeColor("Success: ","green")+"Added PHP "+this->php->version + " in Database";
        }
        void setPHP(){
            string version = this->getArgumentValue(this->getArgument(PHP::VERSION_KEY),10);
            
            PHP matched_version = Validation::validatePHPVersion(this,version);

            this->registrar->activate_version(matched_version);
        }
        void listPHPs(){
            if(this->fileManager->availableVersions.size() > 0){
                for(int i=0;i<this->fileManager->availableVersions.size();i++){
                    cout << setw(30) << left << (this->output->changeColor("("+to_string(i+1)+") ","blue") + this->fileManager->availableVersions[i].version) << this->fileManager->availableVersions[i].path << endl;
                }
            }else{
                cout << this->output->changeColor("INFO: ","blue") + "No Version Available in local database";
            }
        }
        void unsetPHP(){
            this->registrar->deactivate_all_versions(true);
        }
        void removePHP(){
            string version = this->getArgumentValue(this->getArgument(PHP::VERSION_KEY),10);
            
            Validation::alreadyExistCheck(this,version,true);

            Validation::isNotActivatedCheck(this,version);

            this->fileManager->removeVersion(version);

            cout << this->output->changeColor("SUCCESS: ","green") + "Removed version " + version + " from Database";
        }

        void help(){
            cout << this->output->changeColor("Syntax: ","blue") << endl << "   " << "switcher action [--arguments]" << endl;

            cout << this->output->changeColor("Actions: ","blue") << endl;
            cout << "   " << setw(25) << left << this->output->changeColor("add ","green") << "Add a new version data to Local Database using given arguments." << endl;
            cout << "   " << setw(25) << left << this->output->changeColor("remove ","green") << "Remove a version data to Local Database using given arguments." << endl;
            cout << "   " << setw(25) << left << this->output->changeColor("set ","green") << "Activate (switch to) a version from database." << endl;
            cout << "   " << setw(25) << left << this->output->changeColor("unset ","green") << "Deactivate any activated version" << endl;
            cout << "   " << setw(25) << left << this->output->changeColor("list ","green") << "List all available versions" << endl;

            cout << this->output->changeColor("Arguments: ","blue") << endl;
            cout << "   " << setw(25) << left << this->output->changeColor("--version ","green") << "Specify a version" << endl;
            cout << "   " << setw(25) << left << this->output->changeColor("--path ","green") << "Specify a path for version" << endl;
        }

        void refreshEnv(){
            LPCTSTR keyPath = TEXT("Environment");
            SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)keyPath, SMTO_BLOCK, 100, NULL);
        }

        FileManager *fileManager;
        PHP *php;
        Registrar *registrar;
    private:
        char *args[4];
        string getArgument(string argKey){
            try{
                bool found = false;
                string current;
                for(int i=0;i<this->sizeOfArgs;i++){
                    if(!found){
                        current = this->args[i];
                        if(current.find(argKey) != string::npos){
                            found = true;
                            break;
                        }
                    }else{
                        break;
                    }
                }
                if(found){
                    return current;
                }
                throw("Argument '"+argKey+"' is missing");
            }catch(string e){
                cout << this->output->changeColor("ERROR: ","red") + e;
                exit(1);
            }
        }

        string getArgumentValue(string arg,int maxLength = 50){
            string this_arg = arg;
            string res;
            try{
                res = arg.replace(0,arg.find("=")+1,"");
                if(res == ""){
                    throw("Invalid Argument Value For '"+this_arg+"'");
                }else if(res.length() > maxLength){
                    throw((string)"Argument value is too long (Max length for that argument is: "+to_string(maxLength)+")");
                }
            }catch(string e){
                cout << this->output->changeColor("ERROR: ","red") + e;
                exit(1);
            }
            return res;
        }
};

PHP Validation::validatePHPVersion(Manager *manager,string version = ""){
    try{
        for(int i=0;i<manager->fileManager->availableVersions.size();i++){
            if(manager->fileManager->availableVersions[i].version == version){
                return manager->fileManager->availableVersions[i];
            }
        }
        throw("Version "+version+" does not exists in Database, please add it first");
    }catch(string e){
        cout << manager->output->changeColor("ERROR: ","red") + e;
        exit(1);
    }
}
bool Validation::alreadyExistCheck(Manager *manager,string version,bool shouldExistOrNot = false){
    try{
        for(int i=0;i<manager->fileManager->availableVersions.size();i++){
            if(manager->fileManager->availableVersions[i].version == version){
                if(shouldExistOrNot){
                    return true;
                }else{
                    throw("The version "+version+" already exists in Database");
                }
            }
        }
        if(shouldExistOrNot){
            throw("The version "+version+" is already removed from database");
        }else{
            return true;
        }
    }catch(string e){
        cout << manager->output->changeColor("ERROR: ","red") + e;
        exit(1);
    }
}
void Validation::isNotActivatedCheck(Manager *manager,string version){
    PHP *current_activated = manager->registrar->current_activated_version();
    try{
        if(version == current_activated->version){
            throw((string)"Currently activated versions can't be removed.");
        }
    }catch(string e){
        cout << manager->output->changeColor("ERROR: ","red") + e;
        exit(1);
    }
}


int main(int argc,char* argv[]){
    Manager *manager = new Manager(argv,argc);
    manager->setup();

    if(argc > 1){
        string action = argv[1];
        if(action == "add"){
            manager->addPHP();
        }else if(action == "remove"){
            manager->removePHP();
        }else if(action == "set"){
            manager->setPHP();
            manager->refreshEnv();
        }else if(action == "list"){
            manager->listPHPs();
        }else if(action == "unset"){
            manager->unsetPHP();
            manager->refreshEnv();
        }else if(action == "--help"){
            manager->help();
        }
    }else{
        cout << manager->output->changeColor("ERROR: ","red") + "Some Arguments Are Missing";
        exit(1);
    }

    return 0;
}