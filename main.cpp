#include <iostream>
#include <string>
#include <fstream>
#include <vector>

std::string desiredName;
std::string desiredPassword;

class client {
private:
    std::string username;
    std::string password;

public:
    client(std::string us, std::string pass) : username(us), password(pass) {}
    client() {}
    client(const client& other_client) : username(other_client.getUsername()), password(other_client.getPassword()) {}
    std::string getUsername() const { return username; }
    void setUsername(const std::string& str) { username = str; }

    std::string getPassword() const { return password; }
    void setPassword(const std::string& str) { password = str; }
    friend void display(const client& cli);
};

void display(const client& cli) {
    std::cout << "Username: " << cli.username << '\n' << "Password: " << cli.password << "\n\n";
}

int getCharacter(std::fstream& stream, const std::string& filename) {
    int elementsNum;
    stream.open(filename, std::ios_base::binary | std::ios_base::in);
    if (!stream.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        exit(1);
    }
    stream.read(reinterpret_cast<char*>(&elementsNum), sizeof(int));
    if (stream.eof()) {
        stream.close();
        return 0;
    }
    else {
        stream.close();
        return elementsNum;
    }
}

void setCharacter(std::fstream& stream, const std::string& filename, int num) {
    stream.open(filename, std::ios_base::binary | std::ios_base::out);
    if (!stream.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        exit(1);
    }
    stream.write(reinterpret_cast<char*>(&num), sizeof(int));
    std::cout << "Number set\n\n";
    stream.close();
}

void writeElement(std::fstream& stream, const std::string& filename, const std::string& numfile, const client& user) {
    int num = getCharacter(stream, numfile);
    num++;
    stream.open(numfile, std::ios_base::binary | std::ios_base::out);
    if (!stream.is_open()) {
        std::cerr << "Failed to open file: " << numfile << std::endl;
        exit(1);
    }
    stream.write(reinterpret_cast<char*>(&num), sizeof(int));
    stream.close();

    stream.open(filename, std::ios_base::binary | std::ios_base::out | std::ios_base::app);
    if (!stream.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        exit(1);
    }

    // Serialize username
    int usernameSize = user.getUsername().size();
    stream.write(reinterpret_cast<char*>(&usernameSize), sizeof(int));
    stream.write(user.getUsername().c_str(), usernameSize);

    // Serialize password
    int passwordSize = user.getPassword().size();
    stream.write(reinterpret_cast<char*>(&passwordSize), sizeof(int));
    stream.write(user.getPassword().c_str(), passwordSize);
    std::cout << "Registered successfuly!\n\n";
    stream.close();
}

void ReadElements(std::fstream& stream, std::fstream& numstream, const std::string& filename, const std::string& numfile) {
    int num = getCharacter(numstream, numfile);
    

    stream.open(filename, std::ios_base::binary | std::ios_base::in);
    if (!stream.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        exit(1);
    }

    for (int i = 0; i < num; ++i) {
        client User;
        // Read username
        int nameLen;
        stream.read(reinterpret_cast<char*>(&nameLen), sizeof(int));
        std::string name(nameLen, '\0');
        stream.read(&name[0], nameLen);
        User.setUsername(name);
        

        // Read password
        int passLen;
        stream.read(reinterpret_cast<char*>(&passLen), sizeof(int));
        std::string pass(passLen, '\0');
        stream.read(&pass[0], passLen);
        User.setPassword(pass);
        display(User);
    }

    stream.close();
}
void login(std::string& loginName, std::string& loginPass,std::fstream &stream, std::string writefile, std::fstream& numstream, std::string &numfile) {
	bool isLogged = false;
    int num = getCharacter(numstream, numfile);
    int numFailed = 0;
    int i = 1;
    stream.open(writefile, std::ios_base::in | std::ios_base::binary);
    if (!stream.is_open()) {
        std::cerr << "Failed to open file: " << writefile << std::endl;
        exit(1);
    }
    for (i; i <= num; i++) {
        // Read username
        int nameLen = 0;
        stream.read(reinterpret_cast<char*>(&nameLen), sizeof(int));
        std::string name(nameLen, '\0');
        stream.read(&name[0], nameLen);
        // Read password
        int passLen = 0;
        stream.read(reinterpret_cast<char*>(&passLen), sizeof(int));
        std::string pass(passLen, '\0');
        stream.read(&pass[0], passLen);
    
        if (name == loginName && pass == loginPass) {
            std::cout << "Logged in successfuly!\n\n";
            isLogged = true;
            break;
        }
    }
    if (isLogged == false) {
        std::cout << "Username or Password incorrect!\n\n";
    }
}
int option;

int main() {
    std::fstream stream;
    std::fstream numstream;
    std::string writefile = "data.dat";
    std::string numfile = "numbers.dat";

    while (option != 4) {
        std::cout << "What would you like to do?\n1:register  2:read users  3:login(pending)  4:exit\n";
        std::cin >> option;
        if (option == 1) { // Register
            std::cin.ignore(1, '\n');

            std::cout << "Username: ";
            std::cin >> desiredName;
            std::cout << "Password: ";
            std::cin >> desiredPassword;

            client desiredClient(desiredName, desiredPassword);
            writeElement(stream, writefile, numfile, desiredClient);
        }
        else if (option == 2) {
            ReadElements(stream, numstream, writefile, numfile);
        }
        else if (option == 3) {
            std::string user;
            std::string pass;
            std::cin.ignore(1, '\n');

            std::cout << "Username: ";
            std::cin >> user;
            std::cout << "Password: ";
            std::cin >> pass;
            login(user, pass, stream, writefile, numstream, numfile);
        }
        else if (option == 5) {
            std::cin.ignore(1, '\n');
            int num;
            std::cin >> num;
            setCharacter(stream, numfile, num);
        }
        else if (option == 6) {
            stream.open(writefile, std::ios_base::out | std::ios_base::trunc);
            stream.close();
            std::cout << "file cleared.\n\n";
        }
        else if(option == 7){
            std::cout << getCharacter(numstream, numfile) << "\n\n";
        }
    }
    return 0;
}
