#include <tsh.h>

using namespace std;

/**
 * @brief
 * Helper function to print the PS1 pormpt.
 *
 * Linux has multiple promt levels
 * (https://wiki.archlinux.org/title/Bash/Prompt_customization): PS0 is
 * displayed after each command, before any output. PS1 is the primary prompt
 * which is displayed before each command. PS2 is the secondary prompt displayed
 * when a command needs more input (e.g. a multi-line command). PS3 is not very
 * commonly used
 */
void display_prompt() {cout << "$ ";}

/**
 * @brief Cleans up allocated resources to prevent memory leaks.
 *
 * This function deletes all elements in the provided list of Process objects,
 * clears the list, and frees the memory allocated for the input line.
 *
 * @param process_list A pointer to a list of Process pointers to be cleaned up.
 * @param input_line A pointer to the dynamically allocated memory for user
 * input. This memory is freed to avoid memory leaks.
 */
void cleanup(list<Process *> &process_list, char *input_line) {
  for (Process *p : process_list) {
    delete p;
  }
  process_list.clear();
  free(input_line);
  input_line = nullptr;
}

/**
 * @brief Main loop for the shell, facilitating user interaction and command
 * execution.
 *
 * This function initializes a list to store Process objects, reads user input
 * in a loop, and executes the entered commands. The loop continues until the
 * user decides to quit.
 *
 * @note The function integrates several essential components:
 *   1. Displaying the shell prompt to the user.
 *   2. Reading user input using the read_input function.
 *   3. Parsing the input into a list of Process objects using parse_input.
 *   4. Executing the commands using run_commands.
 *   5. Cleaning up allocated resources to prevent memory leaks with the cleanup
 * function.
 *   6. Breaking out of the loop if the user enters the quit command.
 *   7. Continuously prompting the user for new commands until an exit condition
 * is met.
 *
 * @warning
 *  Initialize Necessary Variables:
 *      Declare variables for storing the list of Process objects, the input
 * line, and a flag for quitting. Read User Input in a Loop: Utilize a while
 * loop to continuously read user input until a termination condition is met.
 * Check for a valid input line and skip empty lines. Clean Up Resources: After
 * executing the commands, clean up allocated resources using the cleanup
 *      function to avoid memory leaks.
 *  Check for Quit Command:
 *      Determine if the user entered the quit command. If so, set the exit flag
 * to true and break out of the loop. Repeat the Process: If the user did not
 * quit, continue prompting for input by displaying the prompt again.
 *  Considerations:
 *      Handle edge cases such as empty input lines or unexpected errors
 * gracefully. Ensure proper error handling and informative messages for the
 * user.
 */
void run() {
  list<Process *> process_list;
  char *input_line;
  bool is_quit = false;
  while (!is_quit){
    display_prompt();
    char* input = read_input();
    parse_input(input, process_list);
    is_quit = run_commands(process_list);
    cleanup(process_list, input_line);
  }
}

/**
 * @brief Reads input from the standard input (stdin) in chunks and dynamically
 *        allocates memory to store the entire input.
 *
 * This function reads input from the standard input in chunks of size MAX_LINE.
 * It dynamically allocates memory to store the entire input, resizing the
 * memory as needed. The input is stored as a null-terminated string. The
 * function continues reading until a newline character is encountered or an
 * error occurs.
 *
 * @return A pointer to the dynamically allocated memory containing the input
 * string. The caller is responsible for freeing this memory when it is no
 * longer needed. If an error occurs or EOF is reached during input, the
 * function returns NULL.
 *
 * @warning Ensure that the memory allocated by this function is freed using
 * free() to avoid memory leaks.
 */
char *read_input() {
  char *input = NULL;
  char tempbuf[MAX_LINE];
  //size_t inputlen = 0, templen = 0;
  std::cout << "Enter a line: ";
  std::cin.getline(tempbuf, MAX_LINE);
  input = strdup(tempbuf);
  return input;
}

/**
 * Parses the given command string and populates a list of Process objects.
 *
 * This function takes a command string and a reference to a list of Process
 * pointers. It tokenizes the command based on the delimiters "|;" and creates a
 * new Process object for each token. The created Process objects are added to
 * the provided process_list. Additionally, it sets pipe flags for each Process
 * based on the presence of pipe delimiters '|' in the original command string.
 *
 * @param cmd The command string to be parsed.
 * @param process_list A reference to a list of Process pointers where the
 * created Process objects will be stored.
 *
 * Hints for students:
 * - The 'delimiters' variable holds the characters used to tokenize the command
 * string.
 * - 'pipe_in_val' is a flag indicating whether the current Process should take
 * input from a previous Process (1 if true, 0 if false).
 * - 'cmd_copy' is a duplicate of the original command string, and it's used to
 *   preserve the original string while tokenizing.
 * - 'currProcess' is a pointer to the current Process being created and added
 * to the list.
 * - The function uses strtok to tokenize the command based on delimiters and
 * iterates through the resulting tokens.
 * - For each token, a new Process object is created with relevant information,
 * and the pipe flags are set based on the presence of '|' in the original
 * command.
 * - The created Process objects are added to the process_list.
 * - After parsing, the memory allocated for 'cmd_copy' is freed.
 * - Finally, the split_string() method is called for each Process in the
 * process_list.
 */
void parse_input(char *cmd, list<Process *> &process_list) {
  const char *delimiters = "|;";
  int pipe_in_val = 0;
  char *cmd_copy = strdup(cmd);
  //Process *currProcess = nullptr;
  char* token = strtok(cmd_copy, delimiters);
  char* next;
  while(token != NULL){
    // check next for |
    next = strtok(NULL, delimiters);
    int pipe_out_val = (*strtok(NULL, "") == '|' && next) ? 1 : 0;
    
    // create new process
    Process* newProc = new Process(token, pipe_in_val, pipe_out_val); 

    // add to list
    process_list.push_back(newProc);

    // iterate to next token
    token = next;
    pipe_in_val = pipe_out_val;
  }

  // free cmd_cpy
  free(cmd_copy);

  //split_string() method is called for each Process in the * process_list.
  for (Process* p : process_list){
    p->split_string();
  }
}

/**
 * Check if the given command represents a quit request.
 *
 * This function compares the first token of the provided command with the
 * string "quit" to determine if the command is a quit request.
 *
 * Parameters:
 *   - p: A pointer to a Process structure representing the command.
 *
 * Returns:
 *   - true if the command is a quit request (the first token is "quit").
 *   - false otherwise.
 */
bool isQuit(Process *p) {
  if (!p || !(p->cmd)){return false;}
  char* cpy_cmd = strdup((p->cmd));
  char* cmd = strtok(p->cmd, "");
  bool isQuit = strcmp(cmd, "quit") == 0;
  free(cpy_cmd);
  return isQuit;
}

/**
 * @brief Execute a list of commands using processes and pipes.
 *
 * This function takes a list of processes and executes them sequentially,
 * connecting their input and output through pipes if needed. It handles forking
 * processes, creating pipes, and waiting for child processes to finish.
 *
 * @param command_list A list of Process pointers representing the commands to
 * execute. Each Process object contains information about the command, such as
 *                     command tokens, pipe settings, and file descriptors.
 *
 * @return A boolean indicating whether a quit command was encountered during
 * execution. If true, the execution was terminated prematurely due to a quit
 * command; otherwise, false.
 *
 * @details
 * The function iterates through the provided list of processes and performs the
 * following steps:
 * 1. Check if a quit command is encountered. If yes, terminate execution.
 * 2. Create pipes and fork a child process for each command.
 * 3. In the parent process, close unused pipes, wait for child processes to
 * finish if necessary, and continue to the next command.
 * 4. In the child process, set up pipes for input and output, execute the
 * command using execvp, and handle errors if the command is invalid.
 * 5. Cleanup final process and wait for all child processes to finish.
 *
 * @note
 * - The function uses Process objects, which contain information about the
 * command and pipe settings.
 * - It handles sequential execution of commands, considering pipe connections
 * between them.
 * - The function exits with an error message if execvp fails to execute the
 * command.
 * - Make sure to properly manage file descriptors, close unused pipes, and wait
 * for child processes.
 * - The function returns true if a quit command is encountered during
 * execution; otherwise, false.
 *
 * @warning
 * - Ensure that the Process class is properly implemented and contains
 * necessary information about the command, such as command tokens and pipe
 * settings.
 * - The function relies on proper implementation of the isQuit function for
 * detecting quit commands.
 * - Students should understand the basics of forking, pipes, and process
 * execution in Unix-like systems.
 */
bool run_commands(list<Process *> &command_list) {
  bool is_quit = false;
  //int i = 0;
  //int j = 0;
  //int size = command_list.size();
  //pid_t pids[size];
  Process *prev = nullptr;
  int prev_fd[2];
  for (Process* p : command_list){
    // check quit
    if (isQuit(p)){
      is_quit = true;
      break;
    }

    // check if new pipe is needed
    // fork
    int pid = fork();
    if (pid == -1){
      exit(1);
    }

    int fd[2];
    if(pipe(fd) == -1) exit(1);
      // if child
    if (pid == 0) {
        // set up pipes for input and output
        // prev pipes out
        if (p->pipe_in) dup2(prev_fd[1], STDIN_FILENO);

        if (prev) {
          //if (prev->pipe_out) dup2(prev_fd[0], STDOUT_FILENO);
          close(prev_fd[0]);
          close(prev_fd[1]);
        }
        // if it pipes out
        if(p->pipe_out) dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);

        // execute the command using execvp
        execvp(p->cmdTokens[0], p->cmdTokens);
        // handle errors if the command is invalid.
        exit(1);
    } else if (pid > 0){
      // if parent
        // close unused pipes
        if (prev) {
          close(prev_fd[0]);
          close(prev_fd[1]);
        }

        // wait for child processes to finish if necessary,
        wait(&pid);
        // continue to the next command.
    }
    prev = p;
  }
  return is_quit;
}

/**
 * @brief Constructor for Process class.
 *
 * Initializes a Process object with the provided command string, input pipe,
 * and output pipe.
 *
 * @param _cmd The command string to be executed.
 * @param _pipe_in The input pipe descriptor.
 * @param _pipe_out The output pipe descriptor.
 */
Process::Process(char *_cmd, int _pipe_in, int _pipe_out) {
  cmd = strdup(_cmd);
  pipe_in = _pipe_in;
  pipe_out = _pipe_out;
}

/**
 * @brief Destructor for Process class.
 *
 * Frees the dynamically allocated memory for the command string.
 */
Process::~Process() { free(cmd); }

/**
 * @brief Tokenizes the command string into an array of strings.
 *
 * Splits the command string based on space (' ') delimiter and stores the
 * resulting tokens in the cmdTokens array. The tokens can be accessed using
 * cmdTokens[index].
 *
 * @note Ensure that the cmdTokens array has sufficient space to store the
 * tokens.
 *
 * @warning This method uses strtok function which modifies the input string.
 * Ensure that the original command string is not needed after calling this
 * method.
 */
void Process::split_string() {
}
