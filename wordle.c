#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// each of our text files contains 1000 words
#define LISTSIZE 1000

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

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: ./wordle wordsize\n");
        return 1;
    }
    // ensure proper usage

    int wordsize = atoi(argv[1]);

    // ensure argv[1] is either 5, 6, 7, or 8 and store that value in wordsize instead
    //wordsize = 'argv[1]';

    if (wordsize < 5 || wordsize > 8) {
        printf("Error: wordsize must be either 5, 6, 7, or 8\n");
        return 1;
    }

    // open correct file, each file has exactly LISTSIZE words
    char wl_filename[6];
    sprintf(wl_filename, "%i.txt", wordsize);
    FILE *wordlist = fopen(wl_filename, "r");
    if (wordlist == NULL) {
        printf("Error opening file %s.\n", wl_filename);
        return 1;
    }

    // load word file into an array of size LISTSIZE
    char options[LISTSIZE][wordsize + 1];

    for (int i = 0; i < LISTSIZE; i++) {
        fscanf(wordlist, "%s", options[i]);
    }

    // pseudorandomly select a word for this game
    srand(time(NULL));
    char *choice = options[rand() % LISTSIZE];

    // allow one more guess than the length of the word
    int guesses = wordsize + 1;
    int won = 0;

    // print greeting, using ANSI color codes to demonstrate
    printf(GREEN"This is WORDLE50"RESET"\n");
    printf("You have %i tries to guess the %i-letter word I'm thinking of\n", guesses, wordsize);

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
        int score = check_word(guess, wordsize, status, choice);

        printf("Guess %i: ", i + 1);

        // Print the guess
        print_word(guess, wordsize, status);

        // if they guessed it exactly right, set terminate loop
        if (score == EXACT * wordsize) {
            won = 1;
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
            printf("%c", choice[i]);
        }
        printf("\n");
    }

    return 0;
}

char* get_guess(int wordsize) {
    char guess[5] = "\n";
    char *attempt = malloc(sizeof(char) * 6);
    
    // ensure users actually provide a guess that is the correct length
    
    int len = 0;
    while (1) {

        scanf("%5s", guess);
        for (int i = 0; i < 6; i++) {
            attempt[i] = guess[i];
        }
        len = strlen(attempt);
        if (len == wordsize) {
            return attempt;
        }
    }
    
    return attempt;
}

int check_word(char* guess, int wordsize, int status[], char* choice) {
    int score = 0;

    // compare guess to choice and score points as appropriate, storing points in status

    for (int i = 0; i < wordsize; i++) {
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

    return score;
}

void print_word(char* guess, int wordsize, int status[]) {
    // print word character-for-character with correct color coding, then reset terminal font to normal

    for (int i = 0; i < wordsize; i++) {
        if (status[i] == EXACT) {
            printf(GREEN"%c", guess[i]);
        }
        else if (status[i] == CLOSE) {
            printf(YELLOW"%c", guess[i]);
        }
        else {
            printf(RED"%c", guess[i]);
        }
    }
    printf(RESET);
    //printf("\n");
    //printf(GREEN"This is WORDLE50"RESET"\n");
    printf("\n");
    return;
}
