#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

// values for colors and score (EXACT == right letter, right place; CLOSE == right letter, wrong place; WRONG == wrong letter)
#define EXACT 2
#define CLOSE 1
#define WRONG 0

// ANSI color codes for boxed in letters
#define GREEN   "\e[38;2;255;255;255;1m\e[48;2;106;170;100;1m"
#define YELLOW  "\e[38;2;255;255;255;1m\e[48;2;201;180;88;1m"
#define RED     "\e[38;2;255;255;255;1m\e[48;2;220;20;60;1m"
#define RESET   "\e[0;39m"

// user-defined function prototypes

char *get_guess(int wordsize);
int check_word(char* guess, int wordsize, int status[], char* choice);
void print_word(char* guess, int wordsize, int status[]); 

// The number of pokemon names in each file
int sizes[] = {39, 116, 183, 152, 100};
int numNames = 0;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: ./wordle wordsize\n");
        return 1;
    }
    // ensure proper usage

    int wordsize = atoi(argv[1]);

    // ensure argv[1] is either 5, 6, 7, 8 or 9 and store that value in wordsize instead

    if (wordsize < 5 || wordsize > 9) {
        printf("Error: wordsize must be either 5, 6, 7, 8 or 9\n");
        return 1;
    }

    char wl_filename[18];
    
    sprintf(wl_filename, "%i_Letter_Poke.txt", wordsize);

    FILE *wordlist = fopen(wl_filename, "r");
    if (wordlist == NULL) {
        printf("Error opening file %s.\n", wl_filename);
        return 1;
    }

    numNames = sizes[wordsize - 5];

    // load word file into an array of size numNames
    char options[numNames][wordsize + 1];

    for (int i = 0; i < numNames; i++) {
        fscanf(wordlist, "%s", options[i]);
    }

    // pseudorandomly select a word for this game
    srand(time(NULL));
    char *choice = options[rand() % numNames];

    // allow one more guess than the length of the word
    //int guesses = wordsize + 1;
    int guesses = 6;
    int won = 0;

    // print greeting, using ANSI color codes to demonstrate
    printf(GREEN"This is Pokemon Wordle!"RESET"\n");
    printf("You have %i tries to guess the %i-letter Pokemon name I'm thinking of\n", guesses, wordsize);

    // main game loop, one iteration for each guess
    for (int i = 0; i < guesses; i++) {
        // obtain user's guess
        char *guess = get_guess(wordsize);

        // array to hold guess status, initially set to zero
        int status[wordsize];

        // set all elements of status array initially to 0, aka WRONG
        for (int j = 0; j < wordsize; j++) {
            status[j] = 0;
        }

        // Check if the user has guessed the correct name
        won = check_word(guess, wordsize, status, choice);

        printf("Guess %i: ", i + 1);

        // Print the guess
        print_word(guess, wordsize, status);

        if (won) {
            break;
        }
    }

    // Print the game's result

    if (won) {
        printf("You won!\n");
    }
    else {
        printf("The Pokemon's name was ");
        for (int i = 0; i < wordsize; i++) {
            printf("%c", toupper(choice[i]));
        }
        printf("\n");
    }

    return 0;
}

char* get_guess(int wordsize) {
    char buffer[256]; // Temporary buffer to store the input, large enough to handle longer inputs
    char *attempt = malloc(sizeof(char) * (wordsize + 1)); // Allocate space for the guess, including null terminator

    if (!attempt) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    while (1) {
        printf("Enter your %d-letter guess: ", wordsize);
        // Read the entire line of input
        if (!fgets(buffer, sizeof(buffer), stdin)) {
            printf("Error reading input. Try again.\n");
            continue;
        }

        // Remove the newline character from the input, if present
        buffer[strcspn(buffer, "\n")] = '\0';

        // Check the length of the input
        int len = strlen(buffer);
        if (len < wordsize) {
            printf("Guess is too short. Please enter exactly %d characters.\n", wordsize);
        } else {
            // Copy only the first 'wordsize' characters into 'attempt'
            strncpy(attempt, buffer, wordsize);
            attempt[wordsize] = '\0'; // Null-terminate the string
            return attempt;
        }
    }
}

int check_word(char* guess, int wordsize, int status[], char* choice) {
    
    // set first letter of choice to lower case
    choice[0] = tolower(choice[0]);

    // copy over choice 
    char choice_cpy[wordsize + 1];
    strcpy(choice_cpy, choice);
    
    // Mark EXACT matches and store in status
    for (int i = 0; i < wordsize; i++) {
        
        // Fixed case sensitivity by setting all the letters to be lower case
        guess[i] = tolower(guess[i]);

        if (guess[i] == choice_cpy[i]) {
            status[i] = EXACT;
            choice_cpy[i] = '-'; // Denote the letter from the solution has been matched up already
        }
    }

    // Mark close matches and store in status
    for (int i = 0; i < wordsize; i++) {
        // skip letters that have already been matched up
        if (status[i] == EXACT) {
            continue;
        }
        for (int j = 0; j < wordsize; j++) {
            // also skip letters that have been matched up
            if (status[j] == EXACT) {
                continue;
            }
            // check if a letter belongs in the name but is in the wrong position
            else if (guess[i] == choice_cpy[j]) {
                status[i] = CLOSE;
                choice_cpy[j] = '-'; // mark down location of close match
                break;
            }
        }
    }
    // final check to see if the guess was an exact match
    if (strcmp(guess, choice) == 0) {
        return 1;
    }

    return 0;
}

void print_word(char* guess, int wordsize, int status[]) {
    // print word character-for-character with correct color coding, then reset terminal font to normal

    for (int i = 0; i < wordsize; i++) {
        if (status[i] == EXACT) {
            printf(GREEN"%c", toupper(guess[i]));
        }
        else if (status[i] == CLOSE) {
            printf(YELLOW"%c", toupper(guess[i]));
        }
        else {
            printf(RED"%c", toupper(guess[i]));
        }
    }
    printf(RESET);
    //printf(GREEN"This is WORDLE50"RESET"\n");
    printf("\n");
}
