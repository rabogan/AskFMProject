#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<map>
#include<assert.h>
using namespace std;

vector<string>ReadFileLines(string path)
{
    vector<string>lines;
    fstream file_handler(path.c_str());
    if (file_handler.fail())
    {
        cout << "Error:  file failed to open!\n";
        return lines;
    }
    string line;
    while(getline(file_handler,line))
    {
        if(line.size()==0)
            continue;
        lines.push_back(line);
    }
    file_handler.close();
    return lines;
}

void WriteFileLines(string path, vector<string>lines,bool append=true)
{
    auto status = ios::in|ios::out|ios::app;

    if(!append)
        status=ios::in|ios::out|ios::trunc;

    fstream file_handler(path.c_str(),status);
    if(file_handler.fail())
    {
        cout << "Error: unable to open file\n";
        return;
    }
    for(auto &line:lines)
    {
        file_handler<<line<<"\n";
    }
    file_handler.close();
}

vector<string>SplitString(string s, string delimiter = ",")
{
    vector<string>vector_of_strings;
    int pos = 0;
    string subby;
    while((pos=(int)s.find(delimiter))!=-1)
    {
        subby=s.substr(0,pos);
        vector_of_strings.push_back(subby);
        s.erase(0,pos+delimiter.length());
    }
    vector_of_strings.push_back(s);
    return vector_of_strings;
}

int ToInt(string str)
{
    istringstream iss(str);
    int num;
    iss>>num;
    return num;
}

int UserChoice(int low, int high)
{
    int choice = 0;
    cout << "Please enter a choice between "<<low<<"-"<<high <<":  ";
    cin >> choice;
    if(choice>=low&&choice<=high)
        return choice;
    return UserChoice(low,high);
}

int OptionsMenu(vector<string>menu)
{
    cout << "Menu:\n";
    for (int i = 0; i<(int)menu.size(); ++i)
    {
        cout << i+1 << ")" << menu[i] << "\n";
    }
    return UserChoice(1,(int)menu.size());
}

struct User
{
    int user_id;
    string username;
    string password;
    string name;
    string email;
    int allows_anonymous_questions;

    vector<int>QuestionIDs_From_Me;
    map<int,vector<int>>QuestionID_Thread_Map;

    User()
    {
        user_id=allows_anonymous_questions=-1;
    }

    User(string line)
    {
        vector<string>subbies = SplitString(line);
        assert(subbies.size()==6);

        user_id = ToInt(subbies[0]);
        username=subbies[1];
        password=subbies[2];
        name=subbies[3];
        email=subbies[4];
        allows_anonymous_questions=ToInt(subbies[5]);
    }

    string ToString()
    {
        ostringstream oss;
        oss<<user_id<<","<<username<<","<<password<<","<<name<<","
           <<email<<","<<allows_anonymous_questions;
        return oss.str();
    }
};

struct Question
{
    int question_id;
    int parent_question_id;
    int from_user_id;
    int to_user_id;
    int is_anonymous;
    string question_text;
    string answer_text;

    Question()
    {
        question_id=from_user_id=to_user_id=parent_question_id=-1;
        is_anonymous=1;
    }

    Question(string line)
    {
        vector<string>subbies = SplitString(line);
        assert(subbies.size()==7);

        question_id=ToInt(subbies[0]);
        parent_question_id=ToInt(subbies[1]);
        from_user_id=ToInt(subbies[2]);
        to_user_id=ToInt(subbies[3]);
        is_anonymous=ToInt(subbies[4]);
        question_text=subbies[5];
        answer_text=subbies[6];
    }

    string ToString()
    {
        ostringstream oss;
        oss << question_id << "," << parent_question_id << ","
            << from_user_id << "," << to_user_id <<","<<is_anonymous
            <<","<<question_text<<","<<answer_text;
        return oss.str();
    }

    void PrintToQuestions()
    {
        string prefix = "";
        if (parent_question_id!=-1)
            prefix = "Thread: ";
        cout << prefix << "Question ID: (" << question_id << ")";
        if(!is_anonymous)
            cout << " from user ID (" << from_user_id << "):\n";
        cout << "Question: " << question_text << "\n";
        if(answer_text!="")
            cout << prefix << "Answer: " << answer_text << "\n";
    }

    void PrintFromQuestions()
    {
        cout << "Question ID (" << question_id << ") ";
        if(!is_anonymous)
            cout << "not an AQ";
        cout << " to User ID (" << to_user_id << ")\n";
        cout << "Question: " << question_text << "\n";
        if (answer_text!="")
            cout << "Answer: " << answer_text << "\n";
        else
            cout << "Not answered yet!\n";
    }

    void PrintFeedQuestions()
    {
        if (parent_question_id!=-1)
            cout << "Parent Question ID (" << parent_question_id << ") ";
        cout << "Question ID (" << question_id << ") ";
        if(!is_anonymous)
        {
            cout << " from User ID (" << from_user_id << ") ";
        }
        cout << "to User ID (" << to_user_id << "):\n";
        cout << "\tQuestion:  " << question_text << "\n";
        cout << "\tAnswer:  " << answer_text << "\n";
    }
};

struct UserManager
{
    User current_user;
    map<string,User>username_object_map;
    int last_id;

    UserManager()
    {
        last_id=0;
    }

    void LoadDatabase()
    {
        last_id = 0;
        username_object_map.clear();

        vector<string>lines = ReadFileLines("users.txt");
        for (auto &line:lines)
        {
            User user(line);
            username_object_map[user.username] = user;
            last_id=max(last_id,user.user_id);
        }
    }

    void AccessSystem()
    {
        int choice = OptionsMenu({"Log In","Sign Up"});
        if (choice==1)
            DoLogIn();
        else
            DoSignUp();
    }

    void DoLogIn()
    {
        LoadDatabase();
        while(true)
        {
            cout << "Please enter your username and password:  \n";
            cin >> current_user.username >> current_user.password;
            if (!username_object_map.count(current_user.username))
            {
                cout << "Username or password incorrect; please try again!\n";
                continue;
            }
            User DjangoLives = username_object_map[current_user.username];
            if (DjangoLives.password!=current_user.password)
            {
                cout << "Username or password incorrect; please try again!\n";
                continue;
            }
            current_user=DjangoLives;
            break;
        }
    }

    void DoSignUp()
    {
        while(true)
        {
            cout << "Please enter your username:   ";
            cin >> current_user.username;
            if(username_object_map.count(current_user.username))
            {
                cout << "This username has already been taken.  Please try again\n";
            }
            else
                break;
        }
        cout <<"Please enter a password:   ";
        cin >> current_user.password;
        cout << "Please enter your name:   ";
        cin >> current_user.name;
        cout << "Please enter your email:   ";
        cin >> current_user.email;
        cout << "Will you allow anonymous questions?  (0=No, 1=Yes):    ";
        cin >> current_user.allows_anonymous_questions;
        current_user.user_id = ++last_id;

        username_object_map[current_user.username]=current_user;
        UpdateDatabase(current_user);
    }

    void UpdateDatabase(User &current_user)
    {
        string line = current_user.ToString();
        vector<string>Lines(1,line);
        WriteFileLines("users.txt",Lines);
    }

    pair<int,int>To_User_Pair()
    {
        int id = 0;

        char response = 'n';
        cout << "Do you need to see the list of User IDs?   (y/n):   ";
        cin >> response;
        if (response=='y'||response=='Y')
            ListSystemUsers();

        cout << "Please enter the user ID of the person whom you'll be sending the question to\n";
        cout << "Alternatively, enter -1 to cancel: ";
        cout << "Enter user ID, or -1 to exit\n";
        cin >> id;

        if (id==-1)
            return make_pair(-1,-1);
        for (auto &pair:username_object_map)
        {
            if (id==pair.second.user_id)
                return make_pair(id,pair.second.allows_anonymous_questions);
        }
        cout << "Invalid user id!\n";
        return To_User_Pair();
    }

    void ListSystemUsers()
    {
        for (auto &pair:username_object_map)
        {
            cout << "User ID: " << pair.second.user_id
                 <<" "<<"Name: " << pair.second.username << "\n";
        }
    }
};



struct QuestionManager
{
    map<int,Question>questionid_object_map;
    map<int,vector<int>>questionid_thread_map;
    int last_id;

    QuestionManager()
    {
        last_id = 0;
    }

    void LoadDatabase()
    {
        last_id=0;
        questionid_object_map.clear();
        questionid_thread_map.clear();

        vector<string>lines = ReadFileLines("questions.txt");
        for(auto &line:lines)
        {
            Question question(line);
            last_id=max(last_id,question.question_id);

            questionid_object_map[question.question_id] = question;

            if(question.parent_question_id==-1)
                questionid_thread_map[question.question_id].push_back(question.question_id);
            else
                questionid_thread_map[question.parent_question_id].push_back(question.question_id);
        }
    }

    void FillUserQuestions(User &user)
    {
        user.QuestionIDs_From_Me.clear();  //Clear the vectors in the user struct
        user.QuestionID_Thread_Map.clear();

        for (auto &pair:questionid_thread_map) //We iterate through this in the QM
        {
            for (auto &question_id:pair.second)
            {
                Question &question = questionid_object_map[question_id];

                if(user.user_id==question.from_user_id)
                    user.QuestionIDs_From_Me.push_back(question.question_id);

                if(user.user_id==question.to_user_id)

                    if(question.parent_question_id==-1)
                        user.QuestionID_Thread_Map[question.question_id].push_back(question.question_id);
                    else
                        user.QuestionID_Thread_Map[question.parent_question_id].push_back(question.question_id);
            }
        }
    }

    void PrintQuestionsToUser(User &user)
    {
        if(user.QuestionID_Thread_Map.size()==0)
            cout << "No Questions!\n";

        for (auto &pair:user.QuestionID_Thread_Map)
        {
            for (auto &question_id:pair.second)
            {
                Question &question = questionid_object_map[question_id];
                question.PrintToQuestions();
            }
        }
        cout << "\n";
    }

    void PrintQuestionsFromUser(User &user)
    {
        if (user.QuestionIDs_From_Me.size()==0)
            cout << "No questions posed\n";

        for (auto&question_id:user.QuestionIDs_From_Me)
        {
            Question &question = questionid_object_map[question_id];
            question.PrintFromQuestions();
        }
        cout << "\n";
    }

    int ReadQuestionIDAny(User &user)
    {
        int question_id;
        cout <<"Please enter a question ID, or -1 to cancel:  ";
        cin >> question_id;
        if (question_id==-1)
            return -1;
        if(!questionid_object_map.count(question_id))
        {
            cout << "No question found with this ID.  Please try again\n";
            return ReadQuestionIDAny(user);
        }
        Question &question = questionid_object_map[question_id];
        if(question.to_user_id!=user.user_id)
        {

            cout << "This question wasn't asked to you!  Please try again!\n";
            return ReadQuestionIDAny(user);
        }
        return question_id;
    }


    void AnswerQuestion(User &user)
    {
        PrintQuestionsToUser(user);
        int question_id = ReadQuestionIDAny(user);

        if (question_id==-1)
            return;

        Question &question = questionid_object_map[question_id];

        question.PrintToQuestions();

        if (question.answer_text!="")
            cout << "Caution!  Answer will be overwritten!\n";

        cout << "Enter answer:  \n";
        getline(cin,question.answer_text);
        getline(cin,question.answer_text);  //Two are used to allow for an 'Enter' key
    }

    void DeleteQuestion(User &user)
    {
        PrintQuestionsToUser(user);

        cout << "Do you have any questions sent to you that you wish to delete?\n";
        int question_id = ReadQuestionIDAny(user);
        if(question_id==-1)
            return;
        vector<int>IDs_To_Remove;
        if(questionid_thread_map.count(question_id))
        {
            IDs_To_Remove = questionid_thread_map[question_id];
            questionid_thread_map.erase(question_id);
        }
        else
        {
            IDs_To_Remove.push_back(question_id);
            for(auto &pair:questionid_thread_map)
            {
                vector<int>&crocodile = pair.second;
                for (int i = 0; i<(int)crocodile.size(); ++i)
                {
                    if(crocodile[i]==question_id)
                    {
                        crocodile.erase(crocodile.begin()+i);
                        break;
                    }
                }
            }
        }
        for (auto id:IDs_To_Remove)
        {
            questionid_object_map.erase(question_id);
        }
    }

    void AskQuestion(User &user, pair<int,int>To_User_Pair)
    {
        Question question;
        if(!To_User_Pair.second)
        {

            cout <<"This user doesn't allow anonymous questions.\n";
            question.is_anonymous=0;
        }
        else
        {
            cout << "Do you want to make your question anonymous?  (0=Nope, 1=Yes)\n";
            cin >> question.is_anonymous;
        }

        question.parent_question_id=ReadQuestionIDThread(user);

        cout << "What is your question?\n";
        getline(cin,question.question_text);
        getline(cin,question.question_text);

        question.from_user_id = user.user_id;
        question.to_user_id = To_User_Pair.first;

        question.question_id = ++last_id;

        questionid_object_map[question.question_id] = question;

        if(question.parent_question_id==-1)
            questionid_thread_map[question.question_id].push_back(question.question_id);
        else
            questionid_thread_map[question.parent_question_id].push_back(question.question_id);
    }

    void ListFeed()
    {
        for (auto &pair:questionid_object_map)
        {
            Question &question = pair.second;
            if(question.answer_text=="")
                continue;
            question.PrintFeedQuestions();
        }
    }

    int ReadQuestionIDThread(User &user)
    {
        int question_id;
        cout << "Enter the Question ID if adding; or enter -1 for a NEW question:   \n";
        cin >> question_id;
        if(question_id==-1)
            return -1;
        if(!questionid_thread_map.count(question_id))
        {
            cout << "Please try again.\n";
            return ReadQuestionIDThread(user);
        }
        return question_id;
    }

    void UpdateDatabase()
    {
        vector<string>lines;
        for(auto&pair:questionid_object_map)
        {
            lines.push_back(pair.second.ToString());
        }
        WriteFileLines("questions.txt",lines,false);
    }
};

struct AskFM
{
    UserManager umo;
    QuestionManager qmo;

    void LoadDatabases(bool fill_user_questions=false)
    {
        umo.LoadDatabase();
        qmo.LoadDatabase();

        if (fill_user_questions)  //Waiting for LogIn or SignUp
            qmo.FillUserQuestions(umo.current_user);
    }

    void run()
    {
        LoadDatabases();
        umo.AccessSystem();
        qmo.FillUserQuestions(umo.current_user);

        vector<string>menu;
        menu.push_back("Print Questions To User");
        menu.push_back("Print Questions From User");
        menu.push_back("Answer Question");
        menu.push_back("Delete Question");
        menu.push_back("Ask Question");
        menu.push_back("Print System Users");
        menu.push_back("List Feed");
        menu.push_back("Log Out");

        while(true)
        {
            int choice = OptionsMenu(menu);
            LoadDatabases(true);

            if (choice==1)
                qmo.PrintQuestionsToUser(umo.current_user);

            else if (choice==2)
                qmo.PrintQuestionsFromUser(umo.current_user);

            else if (choice==3)
            {
                qmo.AnswerQuestion(umo.current_user);
                qmo.UpdateDatabase();
            }

            else if (choice==4)
            {
                qmo.DeleteQuestion(umo.current_user);
                qmo.FillUserQuestions(umo.current_user);
                qmo.UpdateDatabase();
            }

            else if (choice==5)
            {
                pair<int,int>user_combo = umo.To_User_Pair();
                if (user_combo.first!=-1)
                {
                    qmo.AskQuestion(umo.current_user,user_combo);
                    qmo.UpdateDatabase();
                }
            }

            else if (choice==6)
            {
                umo.ListSystemUsers();
            }

            else if (choice==7)
            {
                qmo.ListFeed();
            }

            else
                break;
        }
        run();
    }
};

int main()
{
    AskFM system;
    system.run();
    return 0;
}
