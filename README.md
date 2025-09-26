# Minishell Project in C language
I was tasked on my Operating Systems class to implement a minishell that supports aliases, pipelines and basic shell commands

## Table of Contents
- [Features](#features)
- [Usage](#usage)
- [Test Cases](#test_cases)
- [Further Improvements](#further_improvements)

## Features
I followed my professor's instruction on which features I must include for this project:
- Common:
  1. -shell:
 Using 'exec()' family, the shell runs in an infinite loop, reading one command line at a time.
 Parent process is waiting for all child processes to finish.
 No background execution of processes.
  2. -input:
 Each command consists of at most 20 words (tokens)
 No empty or unknown commands.
 No shell metacharacters support.
- External commands:
  1. -cd commands:
      cd ~ changes to $HOME
      Does not handle the case of 'cd' with no argument
     
      <img width="545" height="406" alt="image" src="https://github.com/user-attachments/assets/5cfda97e-1c9a-4b8c-a776-8ff1c1d0d603" />

  3. -alias commands:
     The maximum number of unique alias names is 20.
     If the same alias name is defined multiple times, the most recent one takes effect
     There is at most one aliased token per line
     
     <img width="662" height="735" alt="image" src="https://github.com/user-attachments/assets/9f7bce40-0c0d-4f85-8eb4-aa028186c38b" />

  4. -pipe commands:
     The parent must wait for all child processes in the pipeline.
     
     <img width="601" height="784" alt="image" src="https://github.com/user-attachments/assets/ab5972e1-a211-486b-ae2d-67bd54e56e83" />


## Usage
The program uses #include <sys/wait.h>, which means the program must run on Linux/MacOS system.
Program is run simply by compiling the program first with gcc, then calling the program with input redirection to the test case:
gcc minishell.c -o minishell
./minishell < t01.in

<img width="1318" height="186" alt="image" src="https://github.com/user-attachments/assets/2e90a47e-4665-4e91-ad4c-b8e1752d6fbf" />

Since I am using a Windows system, I have been compiling and running my program in WSL


## Test Cases
I was presented with 20 test cases ranging from simple shell commands to complex pipeline + alias commands. I have provided some of those test cases in this repository
You can check the correctness of the minishell by running the commands in the real shell
Minishell on test 19: 

<img width="1379" height="299" alt="image" src="https://github.com/user-attachments/assets/557b36a9-32d3-4bdd-bf53-947be606bfda" />

Real shell on test 19:

<img width="1249" height="282" alt="image" src="https://github.com/user-attachments/assets/c3f92cb0-cba6-4417-8a9b-d950dbfb2ba0" />

## Further Improvements
The current version of the minishell possesses the basic functionalities, however, there are still things we can add to make it more sophisticated.
One of the possible improvements is implementing unalias and enhancing the cd command responses. Furthermore, horizontal expansion is also possible, for example, considering more than 20 tokens in each command, including shell metacharacters and other features.
Another important upgrade to the program is improving its robustness, such as ability to filter through any leading or trailing spaces or tabs, double quotes, as well as incorrect spelling.

     
  
  
