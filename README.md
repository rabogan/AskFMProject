# AskFMProject
This is an instructor-led project where we implemented AskFM.  Although all functions are working, the error handling is very basic in this version, and I want to streamline some other aspects of the code; such as not printing questions out multiple times when answering a question, and terminating functions (such as the DeleteQuestion(User &user) function) immediately when it's clear there's no use for them - rather than wait for the user to input a '-1'.

Firstly, we offer the option to Log In or Sign Up.

<img width="469" alt="Snapshot of the initial menu" src="https://user-images.githubusercontent.com/71470525/118162647-e8083980-b418-11eb-8dc3-8bbf84e3af60.png">

<img width="675" alt="SigningUp" src="https://user-images.githubusercontent.com/71470525/118162665-ec345700-b418-11eb-8247-e825e4d2a5b6.png">

The sign up process depends on a unique username.

<img width="660" alt="FailingToSignUp" src="https://user-images.githubusercontent.com/71470525/118162674-efc7de00-b418-11eb-9d73-d2f3dce39e30.png">

Whether it's the username or password that is incorrectly entered, the system will not allow you in until both are correct.

<img width="511" alt="SecurityOfLogIn" src="https://user-images.githubusercontent.com/71470525/118162716-f9e9dc80-b418-11eb-9e5e-520a70398a9f.png">

This displays the basic menu:

<img width="661" alt="MenuAfterLogIn" src="https://user-images.githubusercontent.com/71470525/118162692-f2c2ce80-b418-11eb-8a1f-41924201593e.png">

We want to be able to ASK QUESTIONS to the user of our choice.   Depending on whether they allow for anonymous questions, we can elect whether to send them anonymously.  In this, an anonymous question is sometimes shortened to 'AQ'.

<img width="462" alt="AskingAQuestion-7" src="https://user-images.githubusercontent.com/71470525/118162749-05d59e80-b419-11eb-8251-4f79e9c9ed03.png">

This shows the response to the question:

<img width="461" alt="AnswerQ" src="https://user-images.githubusercontent.com/71470525/118163454-e7bc6e00-b419-11eb-859c-cceaa49c9eb1.png">

This prints out all questions from the user:

<img width="402" alt="PrintingQsFromUser8" src="https://user-images.githubusercontent.com/71470525/118162762-0a01bc00-b419-11eb-958e-05a2a49c46aa.png">

We offer each user the chance to delete any question put to them.   They can delete any 'parent' question (and, thus, the entire thread of questions that follows it), or any individual question at all.   Thus, if one question in a thread is deem worthy of deletion, the remainder of the questions will not be affected by the delete.

<img width="398" alt="DeleteQuestion" src="https://user-images.githubusercontent.com/71470525/118311892-6d0e5400-b4e8-11eb-961d-b354406335ed.png">
