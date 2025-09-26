# Minishell Project in C language
I was tasked on my Operating Systems class to implement a minishell that supports aliases, pipelines and basic shell commands

## Table of Contents
- [Features](#features)
- [Usage](#usage)
- [Test Cases](#test_cases)
- [Further Improvements](#further_improvements)

## Features
I followed myprofessor's instruction on which features I must include for this project:
- Common:
  -shell:
 Using 'exec()' family, the shell runs in an infinite loop, reading one command line at a time.
 Parent process is waiting for all child processes to finish.
 No background execution of processes.
  -input:
 Each commanf consists of at most 20 words (tokens)
 No empty or unknown commands.
 No shell metacharacters support.
- External commands:
  -cd commands:
  cd ~ changes to $HOME
  Does not handle the case of 'cd' with no argument
  
  
