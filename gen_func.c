#include <stdio.h>
#include <ctype.h>

// displays ascii art from .txt files
void display_ascii(const char *txt_file, int delay) { // const to ensure no modifications
    FILE *ptr; // ptr to access file functions

    ptr = fopen(txt_file, "r"); // open, read only

    if(ptr == NULL) {
        perror("Error opening file"); // perror tells what type of error exists...
    }

    char c;
    while((c = fgetc(ptr)) != EOF) { // while loop to not include EOF (unrepresented char) in printing
        usleep(delay);
        printf("%c", c);
    }
    printf("\n");
    fclose(ptr);
}

// strlwr and strupr doesn't exist on mac - here are makeup funcs
void strlwr(char *string) {
    char *i;
    for(i = string; *i != '\0'; i++){
        *i = tolower(*i);
    }
}

void strupr(char *string) {
    char *i;
    for(i = string; *i != '\0'; i++) {
        *i = toupper(*i);
    }
}
// ---

// accepting yes/no only for yes or no questions
int validans_yesorno(char *answer) {
    int yes = strcmp(answer, "yes");
    int no = strcmp(answer, "no");
    
    if(yes == 0) {
        return 1;
    }
    
    else if(no == 0) {
        return 2;
    }

    return 0;
}

int nochar_ans(char *answer) {
    char *i;
    int dots = 0; // count of decimal points encountered
    for (i = answer; *i != '\0'; i++) {
        if (!isdigit(*i)) {
            if (*i == '.') {
                dots++;
                // Allow only one decimal point
                if (dots > 1) {
                    return 0;
                }
            } else {
                return 0; // if there's a non-digit and non-dot character, return false
            }
        }
    }
    return 1; // if all characters are either digits or dots, return true
}

// taking string w/ whitespaces and strcspn to replace NL with NULL
void str_noNL(char *str, int size) {
    fgets(str, size, stdin);
    str[strcspn(str, "\n")] = '\0';
}

// valid password of length 8, at least one uppercase, one lowercase, and one punctuation
int valid_pass(char *password, int pw_len) {
    char *i;
    int upr = 0, lwr = 0, punc = 0; 
    if(pw_len < 8) {
        return 0;
    } else {
        for(i = password; *i != '\0'; i++) {
            if(isupper(*i)) {
                upr++;
            }
            else if(islower(*i)) {
                lwr++;
            }
            else if(ispunct(*i)) {
                punc++;
            }
        }

        if(upr > 0 && lwr > 0 && punc > 0) {
            return 1;
        }
        return 0;
    }
}

// accepts male/female answer only
int valid_M_or_F(char *M_or_F) {
    int male = strcmp(M_or_F, "MALE");
    int female = strcmp(M_or_F, "FEMALE");
    
    if(male == 0) {
        return 1;
    }
    else if (female == 0) {
        return 2;
    }

    return 0;
}

// checks if input is within valid range set
int valid_ParamRange(double input, double lower, double upper) {
    if (input < lower || input > upper) {
        return 0;
    }
    return 1;
}