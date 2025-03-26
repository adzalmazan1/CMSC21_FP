#include <math.h> // round func
#include <stdio.h>
#include <stdlib.h> // system clear
#include <string.h> 
#include <unistd.h> // usleep func

#include "gen_func.c" // a reusable C file

#define MAX_MEMBERS 5
#define MAX_ROWLN 1010 // for csv lines
#define MAX_STRLN 101 // for strings, 100 with null

struct jogger_info {
    char name[MAX_STRLN];
    char password[MAX_STRLN];
    char gender[MAX_STRLN];
    int age;
    double height;
    double weight;
    double pace;
    double mile_goal;
// --- break --- // 
    int entry;
    double distance;
    double duration;
    double calories;
};

// global struct array
struct jogger_info jogger[MAX_MEMBERS];
int curr_jogger; // global

// screens in order
void splash_screen();

void home_screen();
    
    void jog_squad_members();
        void log_in();
            const char* cell_elem(char *csv_row, int row_num); // used by disp_jogger
            int disp_joggerUN(const char *csv_file);
            
            void access_jogger(struct jogger_info j[MAX_MEMBERS], int jogger_index);
                void view_profile(struct jogger_info j);
                int recommendations(double tdee); //
                void log_jogging(struct jogger_info j[MAX_MEMBERS], int jogger_index);

        void sign_up();
            void store_signup(const char *csv_file, struct jogger_info j); // if user signs up, put to EOF of existing excel file
            int csvrow_read_signup(const char *csv_file, struct jogger_info j[MAX_MEMBERS]); // read + store in struct array
            void overwriterow_signup(struct jogger_info j[MAX_MEMBERS], const char *csv_file, int target_row); // used for best pace 
            void csvrow_read_jogentry(const char *csv_file, struct jogger_info j[MAX_MEMBERS]); // read + store in struct array
            void clear_csv(const char *csv_file); // clear everything using 'truncating'
            void store_jogentry(const char *csv_file, struct jogger_info j[MAX_MEMBERS]);
            
    // view stats
    void view_stats();
        void indiv_stats();
        void team_stats(const char *csv_signup, const char *csv_jogentry);
    
    void hall_of_achievements();
        void distance_achieved(struct jogger_info j[MAX_MEMBERS]);
        void duration_achieved(struct jogger_info j[MAX_MEMBERS]);
        void calories_achieved(struct jogger_info j[MAX_MEMBERS]);

int interface_opt(char *interface_opt, int upper);
int interface_opt_home(char *interface_opt, int upper);

// hernia
void delete (const char *csv_file);
void delete_row(const char *csv_file, int jogger_index);

// int recommendations(double tdee);

void update_info(struct jogger_info j[MAX_MEMBERS], int jogger_index);

int main() {
    splash_screen();
    home_screen();
    return 0;
}

// status: done
void splash_screen() {
    system("clear");
    
    display_ascii("ascii_art/jog_squad.txt", 500);
    usleep(2000000); // 2 seconds
}

// status: done
void home_screen() {
    system("clear");

    display_ascii("ascii_art/home_js.txt", 0);
    printf(
    "\nEnter # to proceed"
    "\n\n1. Jog Squad Members"
    "\n2. View Stats"
    "\n3. Hall of Achievements"
    "\n4. Exit Jog Squad\n\n");

    char home_opt[MAX_STRLN];
    int opt = interface_opt_home(home_opt, 4);

    switch (opt) {
    
    case 1:
        jog_squad_members();
        break;

    case 2:
        view_stats();
        break;

    case 3:
        hall_of_achievements();
        break;

    case 4:
        printf("Exiting Jog Squad...\n\n");
        usleep(2000000);
        system("clear");
        return;
    
    default:
        break;
    }
}

void jog_squad_members() {
    system("clear");

    char answer1[MAX_STRLN];
    struct jogger_info j[MAX_MEMBERS];
    int limit_checker = csvrow_read_signup("csv/sign_up.csv", j); // returns index from prev excel read
    
    display_ascii("ascii_art/1_js.txt", 0);
    
    printf(
    "\nAlready a member of the squad? :)"
    "\n\nIf so, enter 'yes' to log-in"
    "\nOtherwise, enter 'no' to sign-up\n\n");

    do {
        printf("Enter answer here: ");
        
        str_noNL(answer1, sizeof(answer1));
        strlwr(answer1);
        
        if(!validans_yesorno(answer1)) {
            printf("Invalid answer.\n");
        }
    } while(!validans_yesorno(answer1));
    
    if(validans_yesorno(answer1) == 1) { // 1 for yes
        log_in();
    }
    else if(validans_yesorno(answer1) == 2 && limit_checker < MAX_MEMBERS) { // 2 for no
        sign_up();
    }
    else if(limit_checker == MAX_MEMBERS) {
        char answer2[MAX_STRLN];
        do {
            printf("Sign-up limit reached.\nEnter 'yes' to delete a user. Otherwise, 'no': ");
            str_noNL(answer2, sizeof(answer2));
            strlwr(answer2);
            if(!validans_yesorno(answer2)) {
                printf("Invalid answer.\n");
            }
        } while(!validans_yesorno(answer2));
        
        if(validans_yesorno(answer2) == 1) {
            delete("csv/sign_up.csv"); 
            jog_squad_members();
        }
        else {
            jog_squad_members();
        }
    }
}

// status: done
void log_in() {
    system("clear"); // file is already read in disp_UN, no need for another declaration

    display_ascii("ascii_art/1_login.txt", 0);

    printf("\nEnter # to proceed. Press b/B to return. Press h/H to go back to Home Page.\n\n");
    int log_upper = disp_joggerUN("csv/sign_up.csv") - 1;
    
    char log_opt[MAX_STRLN];
    int opt = interface_opt(log_opt, log_upper);
    curr_jogger = opt - 1;

    if(opt == -1){
        home_screen();
    }
    else if(opt == -2){
        jog_squad_members();
    }
    else{
        
        csvrow_read_signup("csv/sign_up.csv", jogger); // password purposes
        char password[MAX_STRLN];
        do {
            printf("Enter your password: ");
            str_noNL(password, sizeof(password));

            if (strcmp(password, jogger[opt-1].password) != 0) {
                printf("Incorrect password. Try again.\n");
            }

        } while (strcmp(password, jogger[opt-1].password) != 0);

        access_jogger(jogger, curr_jogger); // this is where most of the magic happens
    }
}

// status
void sign_up() {
    system("clear");

    struct jogger_info jogger;

    display_ascii("ascii_art/1_signup.txt", 0);
    printf(
    "\nTo successfully sign-up, enter following data asked :)\n\n");

    // lacking: duplicate usernames fix
    printf("Enter username: ");
    str_noNL(jogger.name, sizeof(jogger.name));

    do {
        printf("Enter password: ");
        str_noNL(jogger.password, sizeof(jogger.password)); // like in final exam

        if(!valid_pass(jogger.password, sizeof(jogger.password))) {
            printf("Password must be at least 8 characters long"
            " w/ at least one of each type (uppercase, lowercase, and punctuation).\n");
        }
    } while(!valid_pass(jogger.password, sizeof(jogger.password)));

    do {
        printf("Enter whether male/female: ");
        str_noNL(jogger.gender, sizeof(jogger.gender));
        strupr(jogger.gender);
        
        if(!valid_M_or_F(jogger.gender)) {
            printf("Invalid input.\n");
        }
    } while(!valid_M_or_F(jogger.gender));
    
    // no char and invalid //
    int nca_age, i_age;
    do {
        char age[MAX_STRLN];
        printf("Enter age: ");
        str_noNL(age, sizeof(age));
        
        nca_age = !nochar_ans(age);
        jogger.age = atoi(age);
        i_age = !valid_ParamRange(jogger.age, 10, 100);

        if(nca_age || i_age) {
            printf("The valid age range is within 10-100.\n");
        }
    } while(nca_age || i_age);

    // can i make a func for this?
    int nca_height, i_height;
    do {
        char height[MAX_STRLN];
        printf("Enter height(cm): ");
        str_noNL(height, sizeof(height));
        
        nca_height = !nochar_ans(height);
        jogger.height = atof(height);
        i_height = !valid_ParamRange(jogger.height, 100, 250);

        if(nca_height || i_height) {
            printf("The valid height range is within 100-250.\n");
        }
    } while(nca_height || i_height);

    int nca_weight, i_weight;
    do {
        char weight[MAX_STRLN];
        printf("Enter weight(kg): ");
        str_noNL(weight, sizeof(weight));
        
        nca_weight = !nochar_ans(weight);
        jogger.weight = atof(weight);
        i_weight = !valid_ParamRange(jogger.weight, 20, 300);

        if(nca_weight || i_weight) {
            printf("The valid weight range is within 20-300.\n");
        }
    } while(nca_weight || i_weight);
    
    char ans[MAX_STRLN];
    do {
        printf(
        "\nDo you have an initial jogging experience?"
        "\nType yes or no only: ");
        str_noNL(ans, sizeof(ans));
        strlwr(ans);

        if(!validans_yesorno(ans)) {
            printf("Invalid answer.\n");
            continue;
        }

        double distance = 0.0, duration = 0.0; // distance and duration
        if(validans_yesorno(ans) == 1) { // yes
        
            int nca_dist, i_dist;
            do {
                char dist[MAX_STRLN];
                printf("\nEnter typical distance jogged in a session(mi): ");
                str_noNL(dist, sizeof(dist));

                nca_dist = !nochar_ans(dist);
                distance = atof(dist);
                i_dist = !valid_ParamRange(distance, 0.1, 100);

                if(nca_dist || i_dist) {
                    printf("The valid distance range is within 0.1-100.\n");
                }
            } while(nca_dist || i_dist);
            
            int nca_dur, i_dur;
            do {
                char dur[MAX_STRLN];
                printf("Enter typical duration jogged in a session(min): ");
                str_noNL(dur, sizeof(dur));

                nca_dur = !nochar_ans(dur);
                duration = atof(dur);
                i_dur = !valid_ParamRange(duration, 1, 300);

                if(nca_dur || i_dur) {
                    printf("The valid duration range is within 1-300.\n");
                }
            } while(nca_dur || i_dur);

            jogger.pace = duration / distance;
        } else if (validans_yesorno(ans) == 2) { // no
            jogger.pace = 0.00;
        }
    } while(!validans_yesorno(ans));

    // after all the signing up, data is stored in excel with this func
    store_signup("csv/sign_up.csv", jogger);

    printf("\nNew jog squad member alert!\n\n");

    // after signing up
    usleep(6000000); 
	system("clear");

    // returned to page where login and sigup options are presented
    jog_squad_members();
}

// !!!IMPLEMENTED FROM STACK OVERFLOW THREAD!!! 
// https://stackoverflow.com/questions/12911299/read-csv-file-in-c

// status: done
const char* cell_elem(char *csv_row, int row_num) {
    const char* token;

    // strtok - used to split a string to smaller strings based on a delimiter 
    // declaration - strtok(string, delimiter)

    /* returns a pointer to the first token and it replaces the 
    delimiter in original string with ('\0') --> officially a string for later use */

    // above basically meant it returns the string, replaces the end will null char...

    // "body execution before the increment/decrement section"
    // assuming all the cells are field (condition section)

    // NULL - pointing to a valid memory location
    for(token = strtok(csv_row, ","); token != NULL; row_num--) {
        if (row_num == 1) {
            return token;
        }
        token = strtok(NULL, ","); // NULL here means, continue 'tokenizing' from where you left off
    }

    return NULL;
}

// TODO 
/*
- hall of achievements
*/

// displays jogger usernames in login field
int disp_joggerUN(const char *csv_file) {
    FILE *ptr;
    ptr = fopen(csv_file, "r");

    if(ptr == NULL) {
        perror("Error opening file");
        return -1;
    }

    char row[MAX_ROWLN];
    
    int num = 1;
    // ignore header, do nothing
    fgets(row, MAX_ROWLN, ptr);
    while (fgets(row, MAX_ROWLN, ptr) != NULL) 
    {   
        // temp is a pointer because it's a string
        char *temp = strdup(row); // a duplicate of row is made
        printf("%d. %s\n", num, cell_elem(temp, 1));
        num++; // in the end, num will be +1 the list#
        free(temp); // malloc stuff
    }

    fclose(ptr);
    printf("\n");
    return num; // returns size + 1 of array (doing - 1 to get orig index)
}

// read from sign up excel, store in an array
// meaning, if you call it again even if there's changes, original array is overwritten
int csvrow_read_signup(const char *csv_file, struct jogger_info j[MAX_MEMBERS]) {
    FILE *ptr;
    ptr = fopen(csv_file, "r");

    char row[MAX_ROWLN];
    int index = 0;
    if (ptr == NULL) {
        perror("Error opening file");
        return -1;
    }

    fgets(row, MAX_ROWLN, ptr); // header?
    while (fgets(row, MAX_ROWLN, ptr) != NULL) {
        sscanf(row, "%100[^,],%100[^,],%100[^,],%d,%lf,%lf,%lf",
               j[index].name, j[index].password, j[index].gender,
               &j[index].age, &j[index].height, &j[index].weight, &j[index].pace);
        index++;
    }
    fclose(ptr);
    return index;
}

// one by one storing
// storing sign up data from array
void store_signup(const char *csv_file, struct jogger_info j) {
    FILE *ptr;

    ptr = fopen(csv_file, "a+"); // opened for reading and writing

    if(ptr == NULL) {
        perror("Error opening file"); // the file you are trying to open does not exist
        return;
    }

    fprintf(ptr, "%s,%s,%s,%d,%.2lf,%.2lf,%.2lf\n", j.name, j.password, j.gender, j.age, j.height, j.weight, j.pace); // print in the csv file
    
    fclose(ptr);
}

// read excel, store in array
// array is updated
// clear everything
// print the header
// updated values

void csvrow_read_jogentry(const char *csv_file, struct jogger_info j[MAX_MEMBERS]) {
    FILE *ptr;
    ptr = fopen(csv_file, "r");

    char row[MAX_ROWLN];
    int index = 0;
    if (ptr == NULL) {
        perror("Error opening file");
        return;
    }

    fgets(row, MAX_ROWLN, ptr);
    while (fgets(row, MAX_ROWLN, ptr) != NULL) {
        sscanf(row, "%d,%lf,%lf,%lf",
               &j[index].entry, &j[index].distance, &j[index].duration,
               &j[index].calories);
        index++;
    }
    fclose(ptr);
}

// 'truncating file' ... 'deletion'
void clear_csv(const char *csv_file) {
    FILE *ptr = fopen(csv_file, "w");
    if (ptr == NULL) {
        perror("Error opening file");
        return;
    }
    fclose(ptr);
}

void store_jogentry(const char *csv_file, struct jogger_info j[MAX_MEMBERS]) {
    
    int curr_arr_size = csvrow_read_signup("csv/sign_up.csv", j); // check size from sign up data

    FILE *ptr = fopen(csv_file, "w");
    if (ptr == NULL) {
        perror("Error opening file");
        return;
    }

    fprintf(ptr, "Entry #,Total Distance,Total Duration,Total Calories\n"); // Write header
    for (int i = 0; i < curr_arr_size; i++) { // wrong limit
        fprintf(ptr, "%d,%.2lf,%.2lf,%.2lf\n",
                j[i].entry, j[i].distance, j[i].duration, j[i].calories);
    }
    fclose(ptr);
}

// --- --- //

void access_jogger(struct jogger_info j[MAX_MEMBERS], int jogger_index) {
    system("clear");
    display_ascii("ascii_art/1_helloJogger.txt", 0);

    printf(
    "\nEnter # to proceed"
    "\n\n1. View jogger profile"
    "\n2. Log a jogging entry"
    "\n3. Update jogger age, height, and weight"
    "\n4. Log-out\n\n");

    char jogger_opt[MAX_STRLN];
    int opt = interface_opt(jogger_opt, 4);

    switch (opt)
    {
    case 1:
        view_profile(j[jogger_index]);
        break;
    case 2: 
        log_jogging(j, jogger_index);
        break;
    case 3:
        update_info(j, jogger_index);
    case 4:
        printf("Logging out...\n\n");
        usleep(2000000);
        system("clear");
        home_screen();
    default:
        break;
    }
}

void update_info(struct jogger_info j[MAX_MEMBERS], int jogger_index) {
    system("clear");
    
    csvrow_read_signup("csv/sign_up.csv", j); // check size from sign up data
    
    display_ascii("ascii_art/1_updateinfo.txt", 0);
    printf("\nTo successfully update info, enter following data asked :)\n\n");

    int age2;
    double height2 = 0, weight2 = 0;

	//  receiving age input from the user
		int nca_age, i_age;
		do {
			char mi[MAX_STRLN];
			printf("Enter age: ");
			str_noNL(mi, sizeof(mi));
        
			nca_age = !nochar_ans(mi);
			age2 = atoi(mi);
			i_age = !valid_ParamRange(age2, 10, 100);

			if (nca_age || i_age) {
				printf("The valid age range is within 10-100.\n");
			}
		} while (nca_age || i_age);

		//  receiving height input from the user
		int nca_height, i_height;
		do {
			char mins[MAX_STRLN];
			printf("Enter height: ");
			str_noNL(mins, sizeof(mins));
        
			nca_height = !nochar_ans(mins);
			height2 = atof(mins);
			i_height = !valid_ParamRange(height2, 100, 250);

			if (nca_height || i_height) {
				printf("The valid height range is within 100-250.\n");
			}
		} while (nca_height || i_height);
		
		//  receiving weight input from the user
		int nca_weight, i_weight;
		do {
			char mins[MAX_STRLN];
			printf("Enter weight: ");
			str_noNL(mins, sizeof(mins));
        
			nca_weight = !nochar_ans(mins);
			weight2 = atof(mins);
			i_weight = !valid_ParamRange(weight2, 20, 300);

			if (nca_weight || i_weight) {
				printf("The valid weight range is within 20-300.\n");
			}
		} while (nca_weight || i_weight);

		j[jogger_index].age = age2;
		j[jogger_index].height = height2;
		j[jogger_index].weight = weight2;
		
        overwriterow_signup(j, "csv/sign_up.csv", jogger_index);

        printf("\nSuccessfully updated jogger info!\n\n");

		usleep(3000000);
		system("clear");
        access_jogger(jogger, curr_jogger);
}

void view_profile(struct jogger_info j) {
    system("clear");

    display_ascii("ascii_art/1_joggerprofile.txt", 0);

    printf("%-15s\t %s\n%-15s\t %s\n%-15s\t %d\n%-15s\t %.2lf cm\n%-15s\t %.2lf kg\n%-15s\t %.2lf min/mi\n", 
    "\nName: ", j.name, "Gender: ", j.gender, "Age: ",  j.age, "Height: ", j.height, "Weight: ",  j.weight, "Best Pace: ", j.pace);
    
    double BMR;
    if (strcmp(j.gender, "MALE") == 0) {
        BMR = (10*j.weight) + (6.25*j.height) - (5*j.age) + 5;
    }
    else {
		BMR = (10*j.weight) + (6.25*j.height) - (5*j.age) - 161;
    }

    double TDEE = BMR * 1.55;
    printf("%-15s\t %.0lf", "TDEE: ", TDEE);

	printf("\n\n\t\t\t\t\t\tRECOMMENDED MEAL PLAN: ");
	recommendations(TDEE);

    printf("\nPress b/B to return. Press h/H to go back to Home Page.\n\n"); 
    
    char profile_opt[MAX_STRLN];
    int opt = interface_opt(profile_opt, -1);

    switch(opt) {
        case -2:
            access_jogger(jogger, curr_jogger);
            break;

        case -1:
            home_screen();
            break;

        default:
            break;
    }
}

int recommendations(double tdee){
    FILE *file;
    char line[MAX_ROWLN];
    char *token;
    const char *delimiter = ",";
    char fields[10][1000];

	if(tdee >= 1100 && tdee <= 1200){
		file = fopen("csv/1.2k_calplan.csv", "r");
	}else if(tdee > 1200 && tdee <= 1500){
		file = fopen("csv/1.5k_calplan.csv", "r");
	}else{
		file = fopen("csv/2k_calplan.csv", "r");
    }
    
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    int col = 0;

    printf("\n\t----------------------------------------------------------------------------------------------------\n\n\n");

    while (fgets(line, sizeof(line), file)) {
        col = 0;

        token = strtok(line, delimiter);
        while (token != NULL && col < 10) {
            strcpy(fields[col], token);
            col++;
            token = strtok(NULL, delimiter);
        }

        for (int i = 0; i < col; i++) {
            printf("\t\t%-10s\t", fields[i]);
        }
        printf("\n");
    }
    printf("\n\t----------------------------------------------------------------------------------------------------\n\n");

    fclose(file);

    return 0;
}

void log_jogging(struct jogger_info j[MAX_MEMBERS], int jogger_index) {
    system("clear");

    display_ascii("ascii_art/1_logentry.txt", 0);

    csvrow_read_signup("csv/sign_up.csv", j); 
    csvrow_read_jogentry("csv/jog_entry.csv", j);

    printf("\nEnter data for this session\n\n");

    double miles = 0.0, minutes = 0.0;
    int nca_miles, i_miles;
    do {
        char mi[MAX_STRLN];
        printf("Total miles jogged: ");
        str_noNL(mi, sizeof(mi));
        
        nca_miles = !nochar_ans(mi);
        miles = atof(mi);
        i_miles = !valid_ParamRange(miles, 0.1, 100);

        if (nca_miles || i_miles) {
            printf("The valid distance range is within 0.1-100.\n");
        }
    } while (nca_miles || i_miles);

    int nca_minutes, i_minutes;
    do {
        char mins[MAX_STRLN];
        printf("Total minutes ran: ");
        str_noNL(mins, sizeof(mins));
        
        nca_minutes = !nochar_ans(mins);
        minutes = atof(mins);
        i_minutes = !valid_ParamRange(minutes, 1, 300);

        if (nca_minutes || i_minutes) {
            printf("The valid duration range is within 1-300.\n");
        }
    } while (nca_minutes || i_minutes);

    j[jogger_index].entry += 1;
    j[jogger_index].distance += miles;
    j[jogger_index].duration += minutes;
    j[jogger_index].calories +=  (7 * 3.5 * j[jogger_index].weight / 200) * minutes;
    
    double jogger_cal = round((7 * 3.5 * j[jogger_index].weight / 200) * minutes);
    j[jogger_index].calories +=  jogger_cal;

    printf("\nSuccessfully logged jogging entry!\n");

struct calories_burned {
	double calories;
	char food[MAX_STRLN];
	char serving[MAX_STRLN];
};

    struct calories_burned cal[] = {
        {5, "Lettuce", "1 cup"},
        {17, "Cucumber", "4 ounces"},
        {35, "Eggplant", "1 cup"},
        {50, "Carrots", "1 cup"},
        {53, "Orange", "4 ounces"},
        {56, "Cooked Shrimp", "2 ounces"},
        {59, "Carrots", "1 cup"},
        {75, "White Bread", "1 slice"},
        {78, "Egg", "1 large"},
        {86, "Tofu", "4 ounces"},
        {100, "Grapes", "1 cup"},
        {137, "Cooked Pork", "2 ounces"},
        {151, "Banana", "6 ounces"},
        {155, "Dark Chocolate", "1 ounce"},
        {206, "Rice", "1 cup"},
        {481, "Caeser Salad", "3 cups"},
        {-1, " ", " "}
    };

    // minor formatting issue
    for(int i = 0; cal[i].calories != -1; i++) {
        if (jogger_cal == cal[i].calories) {
            printf("\nYou burned calories equivalent to %s of %s :O\n", cal[i].serving, cal[i].food);
        }
    }

    double session_pace = minutes / miles;

    // over writing in sign up info the best pace CELL SPECIFIC
    if(session_pace > j[jogger_index].pace) {
        j[jogger_index].pace = session_pace;
        printf("\nYou have a new all-time best pace of %.2lf min/mi :O\n", session_pace);
    }

    printf("\n");

    // TABLE CODE HERE
    overwriterow_signup(j, "csv/sign_up.csv", jogger_index);
    clear_csv("csv/jog_entry.csv");
    store_jogentry("csv/jog_entry.csv", j);

    usleep(6000000); 
    system("clear");
    access_jogger(jogger, curr_jogger);
}

void indiv_stats() {
    system("clear");

    display_ascii("ascii_art/2_indivstats.txt", 0);

    printf("\nEnter # whose individual statistics you want to see.\n\n");
    
    int indiv_upper = disp_joggerUN("csv/sign_up.csv") - 1;
    
    char indiv_opt[MAX_STRLN];
    int opt = interface_opt(indiv_opt, indiv_upper);	
    int jogger_index = opt - 1;

    struct jogger_info jogger[MAX_MEMBERS];
    csvrow_read_signup("csv/sign_up.csv", jogger); // array storing
    csvrow_read_jogentry("csv/jog_entry.csv", jogger); // array storing

    // there is a "clear" here
    system("clear");
    display_ascii("ascii_art/2_stats.txt", 0);

    printf("\t\t%s's S T A T S\n\n", jogger[jogger_index].name);
    printf("\t%-20s\t %d\n", "Total Entries: ", jogger[jogger_index].entry);
    printf("\t%-20s\t %.2lf\n", "Total Distance (mi): ", jogger[jogger_index].distance);

    printf("\t%-20s\t %.2lf\n", "Total Duration (min): ", jogger[jogger_index].duration);
    printf("\t%-20s\t %.0lf\n", "Total Calories burned: ", jogger[jogger_index].calories);
    printf("\t%-20s\t %.2lf min/mi", "Current Best Pace: ", jogger[jogger_index].pace);
    printf("\n\n");
    
    printf("\nPress b/B to return. Press h/H to go back to Home Page.\n\n"); 

    int inside_opt = interface_opt(indiv_opt, -1);

    switch(inside_opt) {
        case -1:
            home_screen();
            break;
        case -2:
            view_stats();

        default:
            break;
    }
}

void team_stats(const char *csv_signup, const char *csv_jogentry) {	
    system("clear");

    display_ascii("ascii_art/2_teamstats.txt", 0);

    FILE *signup_file = fopen(csv_signup, "r");
    FILE *jogentry_file = fopen(csv_jogentry, "r");
    
    if (jogentry_file == NULL || signup_file == NULL) {
        perror("Error opening file/s");
        return;
    }

    char name_line[MAX_ROWLN];
    char line[MAX_ROWLN];

    // skip headers
    fgets(line, MAX_ROWLN, jogentry_file);
    fgets(name_line, MAX_ROWLN, signup_file);

    printf("\n");
    printf("-----------------------------------------------------------------------\n");
    printf("%-10s\t%-10s\t%-10s\t%-10s\n", "Name", "Total Distance", "Total Time", "Total Calories Burned");
    printf("-----------------------------------------------------------------------\n");

    while (fgets(line, sizeof(line), jogentry_file) && fgets(name_line, sizeof(name_line), signup_file)) {
        name_line[strcspn(name_line, "\n")] = '\0';
        line[strcspn(line, "\n")] = '\0';

        // skipping the first column (date)
        strtok(line, ","); // skip the first token
        char *distance = strtok(NULL, ",");
        char *time = strtok(NULL, ",");
        char *speed = strtok(NULL, ",");
        char *name = strtok(name_line, ",");

        if (distance != NULL && time != NULL && speed != NULL && name != NULL) {
            printf("%-10s\t%-10s\t%-10s\t%-10s\n", name, distance, time, speed);
        }
    }

    printf("-----------------------------------------------------------------------\n");

    printf("\nPress b/B to return. Press h/H to go back to Home Page.\n\n"); 

    fclose(jogentry_file);
    fclose(signup_file);

    char team_opt[MAX_STRLN];
    int opt = interface_opt(team_opt, -1);

    switch(opt) {
        case -1:
            home_screen();
            break;
        case -2:
            view_stats();
            break;
        default:
            break;
    }

}

void view_stats() {
    system("clear"); 
    display_ascii("ascii_art/2_js.txt", 0);
    
    printf(
	"\nEnter # to proceed. Press b/B to Log-in or Sign-up. Press h/H to go back to Home Page."
	"\n\n1. View Individual Stats"
	"\n2. View Team Stats\n\n"
	);
	
	char stats_opt[MAX_STRLN];
    int opt = interface_opt(stats_opt, 2);

    switch(opt) {

    case -2:
        jog_squad_members();
        break;

    case -1:
        home_screen();
        break;

    case 1:
		indiv_stats();
		break;
	
	case 2:
		team_stats("csv/sign_up.csv", "csv/jog_entry.csv");
		break;

	default:
		break;

    }
}

void overwriterow_signup(struct jogger_info j[MAX_MEMBERS], const char *csv_file, int target_row) {
    FILE *ptr;
    char row[MAX_ROWLN];
    int current_row = 0;
    long target_position = -1;

    ptr = fopen(csv_file, "r+");
    if (ptr == NULL) {
        perror("Error opening file");
        return;
    }

    // Read through the file to find the target row position
    while (fgets(row, MAX_ROWLN, ptr) != NULL) {
        if (current_row == target_row) {
            // Position the file pointer at the beginning of the target row
            target_position = ftell(ptr);
            break;
        }
        current_row++;
    }

    // fseek to go to the target row position
    fseek(ptr, target_position, SEEK_SET);

    int index = target_row; // Assuming target_row directly maps to the array index
    // Write new data for the target row
    fprintf(ptr, "%s,%s,%s,%d,%.2lf,%.2lf,%.2lf\n",
            j[index].name, j[index].password, j[index].gender,
            j[index].age, j[index].height, j[index].weight, j[index].pace);

    fclose(ptr);
}

// downside: cant handle longer strings...
int interface_opt(char *interface_opt, int upper) {
    int nca_opt, i_opt;
    do {
        printf("Enter choice: ");
        str_noNL(interface_opt, sizeof(interface_opt));
        nca_opt = !nochar_ans(interface_opt);
        i_opt = atoi(interface_opt);
  
        int ch_b = strcmp(interface_opt, "b");
        int ch_B = strcmp(interface_opt, "B");
        int ch_h = strcmp(interface_opt, "h");
        int ch_H = strcmp(interface_opt, "H");

        if(nca_opt || i_opt < 1 || i_opt > upper) {
            if(ch_h == 0 || ch_H == 0 ){
                return -1;
            }
            else if(ch_b == 0 || ch_B == 0 ){
                return -2;
            }
            printf("Invalid choice.\n");
        }    
        
    } while(nca_opt || i_opt < 1 || i_opt > upper);
    return i_opt;
}

// home specific
int interface_opt_home(char *interface_opt, int upper) {
    int nca_opt, i_opt;
    do {
        printf("Enter choice: ");
        str_noNL(interface_opt, sizeof(interface_opt));

        nca_opt = !nochar_ans(interface_opt);
        i_opt = atoi(interface_opt);

        if(nca_opt || i_opt < 1 || i_opt > upper) {
            printf("Invalid choice.\n");
        }
    } while(nca_opt || i_opt < 1 || i_opt > upper);
    return i_opt;
}

void delete_row(const char *csv_file, int jogger_index) {
  char filename[MAX_STRLN];
  char temp_filename[MAX_STRLN];
  char buffer[MAX_STRLN];

  strcpy(filename, csv_file);
  strcpy(temp_filename, "temp.csv");

  FILE *csv_fp = fopen(filename, "r");
  FILE *temp_fp = fopen(temp_filename, "w");

  if (csv_fp == NULL || temp_fp == NULL) {
    perror("Error opening file");
    return;
  }

  int keep_reading = 1;
  int current_line = 1;

  do {
    fgets(buffer, MAX_STRLN, csv_fp);
    
    if (feof(csv_fp))
        keep_reading = 0;
    else if (current_line != jogger_index)
        fputs(buffer, temp_fp);
        current_line++;
  } while (keep_reading);

  fclose(csv_fp);
  fclose(temp_fp);

  if (remove(filename) != 0) {
    printf("Error deleting original file\n");
    return;
  }

  if (rename(temp_filename, filename) != 0) {
    printf("Error renaming temporary file\n");
    return;
  }
}

void delete (const char *csv_file) {
    system("clear");

    display_ascii("ascii_art/1_delete.txt", 0);

    printf("\nEnter # to proceed with user deletion.\n\n");
    int upper = disp_joggerUN("csv/sign_up.csv") - 1; // array size in a way

    char delete_opt[MAX_STRLN];
    int nca_opt, i_opt;

    do {
    printf("Enter choice: ");
    str_noNL(delete_opt, sizeof(delete_opt));

    nca_opt = !nochar_ans(delete_opt);
    i_opt = atoi(delete_opt);

    if(nca_opt || i_opt < 1 || i_opt > upper) {
        printf("Invalid choice.\n");
    }

    } while(nca_opt || i_opt < 1 || i_opt > upper);

    delete_row("csv/sign_up.csv", i_opt + 1);
    delete_row("csv/jog_entry.csv", i_opt + 1);

    printf("\nSlots to sign-up have freed. Proceed to sign-up.\n\n");
    usleep(3000000);
}


void hall_of_achievements() {
    system("clear");

    display_ascii("ascii_art/3_js.txt", 0);

    struct jogger_info j[MAX_MEMBERS];

    csvrow_read_signup("csv/sign_up.csv", j);
    csvrow_read_jogentry("csv/jog_entry.csv", j);

    printf(
        "\nEnter # to proceed. Press b/B to Log-in or Sign-up. Press h/H to go back to Home Page."
        "\n\n1. Distance Achievements"
        "\n2. Consistency Achievements"
        "\n3. Calories Burned Achievements\n\n");

    char hall_opt[MAX_STRLN];
    int opt = interface_opt(hall_opt, 3);

    switch (opt) {

    case -2:
        jog_squad_members();

    case -1:
        home_screen();

    case 1:
        distance_achieved(j);
        break;

    case 2:
        duration_achieved(j);
        break;

    case 3:
        calories_achieved(j);
        break;

    default:
        break;
    }
}

void distance_achieved(struct jogger_info j[MAX_MEMBERS]) {
    system("clear");

    display_ascii("ascii_art/3_distancehall.txt", 0);

    struct distance_achieved {
        double goal;
        char achievement[MAX_STRLN];
    };

    struct distance_achieved achievements[] = {
        {1, "First Mile"},
        {5, "5M Runner"},
        {10, "10M Challenger"},
        {13.1, "Half-Marathoner"},
        {-1, " "}};

    printf("\n+-----------------+-----------------+-----------------+-----------------+\n");
    
    for (int i = 0; achievements[i].goal != -1; i++) {
        printf("| %-15s ", achievements[i].achievement);
    }
    printf("|\n");

    printf("+-----------------+-----------------+-----------------+-----------------+\n");

    for (int i = 0; i < MAX_MEMBERS; i++) {
        if (j[i].distance > 0) { 
            for (int k = 0; achievements[k].goal != -1; k++) {
                if (j[i].distance >= achievements[k].goal) {
                    printf("| %-15s ", j[i].name);
                } else {
                    printf("| %-15s ", ""); 
                }
            }
            printf("|\n");
        }
    }
    printf("+-----------------+-----------------+-----------------+-----------------+\n");
    
    printf("\nPress b/B to return. Press h/H to go back to Home Page.\n\n"); 
    
    char disthall_opt[MAX_STRLN];
    int opt = interface_opt(disthall_opt, -1);

    switch(opt) {
        case -2:
            hall_of_achievements();
            break;

        case -1:
            home_screen();
            break;

        default:
            break;
    }
    
}


void duration_achieved(struct jogger_info j[MAX_MEMBERS]){
	system("clear");

    display_ascii("ascii_art/3_consistencyhall.txt", 0);	

    struct duration_achieved{
        int sessions;
        char achievement[MAX_STRLN];
    };

    struct duration_achieved achievements[] = {
        {10, "Quick Jog"},
        {20, "Steady Runner"},
        {30, "30min. Hustler"},
        {60, "Hour Hero"},
        {-1, " ",}
    };

printf("\n+-----------------+-----------------+-----------------+-----------------+\n");
    
    for (int i = 0; achievements[i].sessions != -1; i++) {
        printf("| %-15s ", achievements[i].achievement);
    }
    printf("|\n");

printf("+-----------------+-----------------+-----------------+-----------------+\n");

    for (int i = 0; i < MAX_MEMBERS; i++) {
        if (j[i].duration > 0) { 
            for (int k = 0; achievements[k].sessions != -1; k++) {
                if (j[i].duration >= achievements[k].sessions) {
                    printf("| %-15s ", j[i].name);
                } else {
                    printf("| %-15s ", ""); 
                }
            }
            printf("|\n");
        }
    }

printf("+-----------------+-----------------+-----------------+-----------------+\n");

    printf("\nPress b/B to return. Press h/H to go back to Home Page.\n\n"); 
    
    char consthall_opt[MAX_STRLN];
    int opt = interface_opt(consthall_opt, -1);

    switch(opt) {
        case -2:
            hall_of_achievements();
            break;

        case -1:
            home_screen();
            break;

        default:
            break;
    }
}
    
void calories_achieved(struct jogger_info j[MAX_MEMBERS]){
	system("clear");

    display_ascii("ascii_art/3_calburnedhall.txt", 0);

    struct calories_achieved{
        int burned;
        char achievement[MAX_STRLN];
    };

    struct calories_achieved achievements[] = {
        {300, "Calorie Crusher"},
        {500, "Energy Blaster"},
        {1000, "Calorie Blitz"},
        {2000, "Fuel Burner"},
        {-1, " ",}
    };

printf("\n+-----------------+-----------------+-----------------+-----------------+\n");

    for (int i = 0; achievements[i].burned != -1; i++) {
        printf("| %-15s ", achievements[i].achievement);
    }
    printf("|\n");

printf("+-----------------+-----------------+-----------------+-----------------+\n");

    for (int i = 0; i < MAX_MEMBERS; i++) {
        if (j[i].calories > 0) {
            for (int k = 0; achievements[k].burned != -1; k++) {
                if (j[i].calories >= achievements[k].burned) {
                    printf("| %-15s ", j[i].name);
                } else {
                    printf("| %-15s ", ""); 
                }
            }
            printf("|\n");
        }
    }

printf("+-----------------+-----------------+-----------------+-----------------+\n");

    printf("\nPress b/B to return. Press h/H to go back to Home Page.\n\n"); 
    
    char calburnedhall_opt[MAX_STRLN];
    int opt = interface_opt(calburnedhall_opt, -1);

    switch(opt) {
        case -2:
            hall_of_achievements();
            break;

        case -1:
            home_screen();
            break;

        default:
            break;
    }
    
}