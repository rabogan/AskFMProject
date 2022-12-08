#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<map>
#include<assert.h>

std::vector<std::string>readFileLines(std::string path) {
    std::vector<std::string>lines;
    std::fstream fileHandler(path.c_str());
    if (fileHandler.fail()) {
        std::cout << "Error:  file failed to open!\n";
        return lines;
    }
    std::string line;
    while (std::getline(fileHandler, line)) {
        if (line.size() == 0)
            continue;
        lines.push_back(line);
    }
    fileHandler.close();
    return lines;
}

void writeFileLines(std::string path, std::vector<std::string>lines, bool append = true) {
    auto status = std::ios::in | std::ios::out | std::ios::app;

    if (!append)
        status = std::ios::in | std::ios::out | std::ios::trunc;

    std::fstream fileHandler(path.c_str(), status);
    if (fileHandler.fail()) {
        std::cout << "Error: unable to open file\n";
        return;
    }
    for (auto& line : lines)
        fileHandler << line << "\n";
    fileHandler.close();
}

std::vector<std::string>splitString(std::string s, std::string delimiter = ",") {
    std::vector<std::string>vectorOfStrings;
    int pos = 0;
    std::string substrings;
    while ((pos = (int)s.find(delimiter)) != -1) {
        substrings = s.substr(0, pos);
        vectorOfStrings.push_back(substrings);
        s.erase(0, pos + delimiter.length());
    }
    vectorOfStrings.push_back(s);
    return vectorOfStrings;
}

int toInt(std::string str) {
    std::istringstream iss(str);
    int num;
    iss >> num;
    return num;
}

int userChoice(int low, int high) {
    int choice = 0;
    std::cout << "Please enter a choice between " << low << "-" << high << ":  ";
    std::cin >> choice;
    if (low <= choice && choice <= high)
        return choice;
    return userChoice(low, high);
}

int optionsMenu(std::vector<std::string>menu) {
    std::cout << "Menu:\n";
    for (int i = 0; i < (int)menu.size(); ++i)
        std::cout << i + 1 << ")" << menu[i] << "\n";
    return userChoice(1, (int)menu.size());
}

struct User {
    int userId;
    std::string username;
    std::string password;
    std::string name;
    std::string email;
    int allowsAnonymousQuestions;

    std::vector<int>myQuestionIds;
    std::map<int, std::vector<int>>questionIdThreadMap;

    User() {
        userId = allowsAnonymousQuestions = -1;
    }

    User(std::string line) {
        std::vector<std::string>substrings = splitString(line);
        assert(substrings.size() == 6);

        userId = toInt(substrings[0]);
        username = substrings[1];
        password = substrings[2];
        name = substrings[3];
        email = substrings[4];
        allowsAnonymousQuestions = toInt(substrings[5]);
    }

    std::string toString() {
        std::ostringstream oss;
        oss << userId << "," << username << "," << password << "," << name << ","
            << email << "," << allowsAnonymousQuestions;
        return oss.str();
    }
};

struct Question {
    int questionId;
    int parentQuestionId;
    int fromUserId;
    int toUserId;
    int isAnonymous;
    std::string questionText;
    std::string answerText;

    Question() {
        questionId = fromUserId = toUserId = parentQuestionId = -1;
        isAnonymous = 1;
    }

    Question(std::string line) {
        std::vector<std::string>substrings = splitString(line);
        assert(substrings.size() == 7);

        questionId = toInt(substrings[0]);
        parentQuestionId = toInt(substrings[1]);
        fromUserId = toInt(substrings[2]);
        toUserId = toInt(substrings[3]);
        isAnonymous = toInt(substrings[4]);
        questionText = substrings[5];
        answerText = substrings[6];
    }

    std::string toString() {
        std::ostringstream oss;
        oss << questionId << "," << parentQuestionId << ","
            << fromUserId << "," << toUserId << "," << isAnonymous
            << "," << questionText << "," << answerText;
        return oss.str();
    }

    void printQuestionsReceived() {
        std::string prefix = "";
        if (parentQuestionId != -1)
            prefix = "Thread: ";
        std::cout << prefix << "Question ID: (" << questionId << ")";
        if (!isAnonymous)
            std::cout << " from user ID (" << fromUserId << "):\n";
        std::cout << "Question: " << questionText << "\n";
        if (answerText != "")
            std::cout << prefix << "Answer: " << answerText << "\n";
    }

    void printQuestionsSent() {
        std::cout << "Question ID (" << questionId << ") ";
        if (!isAnonymous)
            std::cout << "not an anonymous question";
        std::cout << " to User ID (" << toUserId << ")\n";
        std::cout << "Question: " << questionText << "\n";
        if (answerText != "")
            std::cout << "Answer: " << answerText << "\n";
        else
            std::cout << "Not answered yet!\n";
    }

    void printFeedQuestions() {
        if (parentQuestionId != -1)
            std::cout << "Parent Question ID (" << parentQuestionId << ") ";
        std::cout << "Question ID (" << questionId << ") ";
        if (!isAnonymous)
            std::cout << " from User ID (" << fromUserId << ") ";
        std::cout << "to User ID (" << toUserId << "):\n";
        std::cout << "\tQuestion:  " << questionText << "\n";
        std::cout << "\tAnswer:  " << answerText << "\n";
    }
};

struct UserManager {
    User currentUser;
    std::map<std::string, User>usernameObjectMap;
    int lastId;

    UserManager() {
        lastId = 0;
    }

    void loadDatabase() {
        lastId = 0;
        usernameObjectMap.clear();

        std::vector<std::string>lines = readFileLines("users.txt");
        for (auto& line : lines) {
            User user(line);
            usernameObjectMap[user.username] = user;
            lastId = std::max(lastId, user.userId);
        }
    }

    void accessSystem() {
        int choice = optionsMenu({ "Log In","Sign Up" });
        if (choice == 1)
            doLogIn();
        else
            doSignUp();
    }

    void doLogIn() {
        loadDatabase();
        while (true) {
            std::cout << "Please enter your username and password:  \n";
            std::cin >> currentUser.username >> currentUser.password;
            if (!usernameObjectMap.count(currentUser.username)) {
                std::cout << "Username or password incorrect; please try again!\n";
                continue;
            }
            User djangoLives = usernameObjectMap[currentUser.username];
            if (djangoLives.password != currentUser.password) {
                std::cout << "Username or password incorrect; please try again!\n";
                continue;
            }
            currentUser = djangoLives;
            break;
        }
    }

    void doSignUp() {
        while (true) {
            std::cout << "Please enter your username:   ";
            std::cin >> currentUser.username;
            if (usernameObjectMap.count(currentUser.username))
                std::cout << "This username has already been taken.  Please try again\n";
            else
                break;
        }
        std::cout << "Please enter a password:   ";
        std::cin >> currentUser.password;
        std::cout << "Please enter your name:   ";
        std::cin >> currentUser.name;
        std::cout << "Please enter your email:   ";
        std::cin >> currentUser.email;
        std::cout << "Will you allow anonymous questions?  (0=No, 1=Yes):    ";
        std::cin >> currentUser.allowsAnonymousQuestions;
        currentUser.userId = ++lastId;

        usernameObjectMap[currentUser.username] = currentUser;
        updateDatabase(currentUser);
    }

    void updateDatabase(User& currentUser) {
        std::string line = currentUser.toString();
        std::vector<std::string>lines(1, line);
        writeFileLines("users.txt", lines);
    }

    std::pair<int, int>toUserPair() {
        int id = 0;
        char response = 'n';
        std::cout << "Do you need to see the list of User IDs?   (y/n):   ";
        std::cin >> response;
        if (response == 'y' || response == 'Y')
            listSystemUsers();
        std::cout << "Please enter the user ID of the person whom you'll be sending the question to\n";
        std::cout << "Alternatively, enter -1 to cancel: ";
        std::cout << "Enter user ID, or -1 to exit\n";
        std::cin >> id;

        if (id == -1)
            return { -1,-1 };
        for (auto& pair : usernameObjectMap) {
            if (id == pair.second.userId)
                return { id,pair.second.allowsAnonymousQuestions };
        }
        std::cout << "Invalid user id!\n";
        return toUserPair();
    }

    void listSystemUsers() {
        for (auto& pair : usernameObjectMap)
            std::cout << "User ID: " << pair.second.userId
            << " " << "Name: " << pair.second.username << "\n";
    }
};

struct QuestionManager {
    std::map<int, Question>questionIdObjectMap;
    std::map<int, std::vector<int>>questionIdThreadMap;
    int lastId;

    QuestionManager() {
        lastId = 0;
    }

    void loadDatabase() {
        lastId = 0;
        questionIdObjectMap.clear();
        questionIdThreadMap.clear();

        std::vector<std::string>lines = readFileLines("questions.txt");
        for (auto& line : lines) {
            Question question(line);
            lastId = std::max(lastId, question.questionId);

            questionIdObjectMap[question.questionId] = question;

            if (question.parentQuestionId == -1)
                questionIdThreadMap[question.questionId].push_back(question.questionId);
            else
                questionIdThreadMap[question.parentQuestionId].push_back(question.questionId);
        }
    }

    void fillUserQuestions(User& user) {
        user.myQuestionIds.clear();  //Clear the vectors in the user struct
        user.questionIdThreadMap.clear();

        for (auto& pair : questionIdThreadMap) { //We iterate through this in the QM
            for (auto& questionId : pair.second) {
                Question& question = questionIdObjectMap[questionId];

                if (user.userId == question.fromUserId)
                    user.myQuestionIds.push_back(question.questionId);

                if (user.userId == question.toUserId)
                    if (question.parentQuestionId == -1)
                        user.questionIdThreadMap[question.questionId].push_back(question.questionId);
                    else
                        user.questionIdThreadMap[question.parentQuestionId].push_back(question.questionId);
            }
        }
    }

    void printQuestionsReceived(User& user) {
        if (user.questionIdThreadMap.size() == 0)
            std::cout << "No Questions!\n";

        for (auto& pair : user.questionIdThreadMap) {
            for (auto& questionId : pair.second) {
                Question& question = questionIdObjectMap[questionId];
                question.printQuestionsReceived();
            }
        }
        std::cout << "\n";
    }

    void printQuestionsSent(User& user) {
        if (user.myQuestionIds.size() == 0)
            std::cout << "No questions posed\n";

        for (auto& questionId : user.myQuestionIds)
        {
            Question& question = questionIdObjectMap[questionId];
            question.printQuestionsSent();
        }
        std::cout << "\n";
    }

    int readQuestionId(User& user) {
        int questionId;
        std::cout << "Please enter a question ID, or -1 to cancel:  ";
        std::cin >> questionId;
        if (questionId == -1)
            return -1;
        if (!questionIdObjectMap.count(questionId)) {
            std::cout << "No question found with this ID.  Please try again\n";
            return readQuestionId(user);
        }
        Question& question = questionIdObjectMap[questionId];
        if (question.toUserId != user.userId) {
            std::cout << "This question wasn't asked to you!  Please try again!\n";
            return readQuestionId(user);
        }
        return questionId;
    }

    void answerQuestion(User& user) {
        printQuestionsReceived(user);
        int questionId = readQuestionId(user);

        if (questionId == -1)
            return;

        Question& question = questionIdObjectMap[questionId];
        question.printQuestionsReceived();

        if (question.answerText != "")
            std::cout << "Caution!  Answer will be overwritten!\n";

        std::cout << "Enter answer:  \n";
        std::getline(std::cin, question.answerText);
        std::getline(std::cin, question.answerText);  //Two are used to allow for an 'Enter' key
    }

    void deleteQuestion(User& user) {
        printQuestionsReceived(user);

        std::cout << "Do you have any questions sent to you that you wish to delete?\n";
        int questionId = readQuestionId(user);
        if (questionId == -1)
            return;
        std::vector<int>idsToRemove;
        if (questionIdThreadMap.count(questionId)) {
            idsToRemove = questionIdThreadMap[questionId];
            questionIdThreadMap.erase(questionId);
        }
        else {
            idsToRemove.push_back(questionId);
            for (auto& pair : questionIdThreadMap) {
                std::vector<int>& crocodile = pair.second;
                for (int i = 0; i < (int)crocodile.size(); ++i) {
                    if (crocodile[i] == questionId) {
                        crocodile.erase(crocodile.begin() + i);
                        break;
                    }
                }
            }
        }
        for (auto id : idsToRemove)
            questionIdObjectMap.erase(questionId);
    }

    void askQuestion(User& user, std::pair<int, int>toUserPair) {
        Question question;
        if (!toUserPair.second) {
            std::cout << "This user doesn't allow anonymous questions.\n";
            question.isAnonymous = 0;
        }
        else {
            std::cout << "Do you want to make your question anonymous?  (0=Nope, 1=Yes)\n";
            std::cin >> question.isAnonymous;
        }

        question.parentQuestionId = readQuestionIdThread(user);

        std::cout << "What is your question?\n";
        std::getline(std::cin, question.questionText);
        std::getline(std::cin, question.questionText);

        question.fromUserId = user.userId;
        question.toUserId = toUserPair.first;

        question.questionId = ++lastId;
        questionIdObjectMap[question.questionId] = question;

        if (question.parentQuestionId == -1)
            questionIdThreadMap[question.questionId].push_back(question.questionId);
        else
            questionIdThreadMap[question.parentQuestionId].push_back(question.questionId);
    }

    void listFeed() {
        for (auto& pair : questionIdObjectMap) {
            Question& question = pair.second;
            if (question.answerText == "")
                continue;
            question.printFeedQuestions();
        }
    }

    int readQuestionIdThread(User& user) {
        int questionId;
        std::cout << "Enter the Question ID if adding; or enter -1 for a NEW question:   \n";
        std::cin >> questionId;
        if (questionId == -1)
            return -1;
        if (!questionIdThreadMap.count(questionId)) {
            std::cout << "Please try again.\n";
            return readQuestionIdThread(user);
        }
        return questionId;
    }

    void updateDatabase() {
        std::vector<std::string>lines;
        for (auto& pair : questionIdObjectMap)
            lines.push_back(pair.second.toString());
        writeFileLines("questions.txt", lines, false);
    }
};

struct AskFM {
    UserManager userManagerObject;
    QuestionManager questionManagerObject;

    void loadDatabases(bool fillUserQuestions = false) {
        userManagerObject.loadDatabase();
        questionManagerObject.loadDatabase();

        if (fillUserQuestions)  //Waiting for LogIn or SignUp
            questionManagerObject.fillUserQuestions(userManagerObject.currentUser);
    }

    void run() {
        loadDatabases();
        userManagerObject.accessSystem();
        questionManagerObject.fillUserQuestions(userManagerObject.currentUser);

        std::vector<std::string>menu;
        menu.push_back("Print Questions To User");
        menu.push_back("Print Questions From User");
        menu.push_back("Answer Question");
        menu.push_back("Delete Question");
        menu.push_back("Ask Question");
        menu.push_back("Print System Users");
        menu.push_back("List Feed");
        menu.push_back("Log Out");

        while (true) {
            int choice = optionsMenu(menu);
            loadDatabases(true);

            if (choice == 1)
                questionManagerObject.printQuestionsReceived(userManagerObject.currentUser);

            else if (choice == 2)
                questionManagerObject.printQuestionsSent(userManagerObject.currentUser);

            else if (choice == 3) {
                questionManagerObject.answerQuestion(userManagerObject.currentUser);
                questionManagerObject.updateDatabase();
            }

            else if (choice == 4) {
                questionManagerObject.deleteQuestion(userManagerObject.currentUser);
                questionManagerObject.fillUserQuestions(userManagerObject.currentUser);
                questionManagerObject.updateDatabase();
            }

            else if (choice == 5) {
                std::pair<int, int>userCombo = userManagerObject.toUserPair();
                if (userCombo.first != -1) {
                    questionManagerObject.askQuestion(userManagerObject.currentUser, userCombo);
                    questionManagerObject.updateDatabase();
                }
            }

            else if (choice == 6)
                userManagerObject.listSystemUsers();

            else if (choice == 7)
                questionManagerObject.listFeed();

            else
                break;
        }
        run();
    }
};

int main() {
    AskFM system;
    system.run();
    return 0;
}
