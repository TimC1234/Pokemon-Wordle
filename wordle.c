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

        // Calculate score for the guess
        //int score = check_word(guess, wordsize, status, choice);
        won = check_word(guess, wordsize, status, choice);

        printf("Guess %i: ", i + 1);

        // Print the guess
        print_word(guess, wordsize, status);

        //if they guessed it exactly right, set terminate loop
        // if (score == EXACT * wordsize) {
        //     won = 1;
        //     break;
        // }

        if (won) {
            break;
        }
    }

    // Print the game's result

    if (won == 1) {
        printf("You won!\n");
    }
    else {
        printf("The word was ");
        for (int i = 0; i < wordsize; i++) {
            printf("%c", toupper(choice[i]));
            //printf("%c", tolower(choice[i]));
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
    int score = 0;

    // compare guess to choice and score points as appropriate, storing points in status

    for (int i = 0; i < wordsize; i++) {
        guess[i] = tolower(guess[i]);
        choice[i] = tolower(choice[i]);
    }

    for (int i = 0; i < wordsize; i++) {
        // if (i > 0) {
        //     guess[i] = tolower(guess[i]);
        // }
        // else {
        //     guess[i] = toupper(guess[i]);
        // }
        
        for (int j = 0; j < wordsize; j++) {
            if (guess[i] == choice[j] && i == j) {
                score += EXACT;
                status[i] = EXACT;
                break;
            }
            else if (guess[i] == choice[j]) {
                score += CLOSE;
                status[i] = CLOSE;
            }
        }
    }
    if (strcmp(guess, choice) == 0) {
        //printf("Equal\n");
        return 1;
    }

    return 0;
}

void print_word(char* guess, int wordsize, int status[]) {
    // print word character-for-character with correct color coding, then reset terminal font to normal

    for (int i = 0; i < wordsize; i++) {
        if (status[i] == EXACT) {
            printf(GREEN"%c", toupper(guess[i]));
            //printf(GREEN"%c", tolower(guess[i]));
        }
        else if (status[i] == CLOSE) {
            printf(YELLOW"%c", toupper(guess[i]));
            //printf(YELLOW"%c", tolower(guess[i]));
        }
        else {
            printf(RED"%c", toupper(guess[i]));
            //printf(RED"%c", tolower(guess[i]));
        }
    }
    printf(RESET);
    //printf("\n");
    //printf(GREEN"This is WORDLE50"RESET"\n");
    printf("\n");
    return;
}
