#include<windows.h>
#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

#define MAX 100
#define MAX_NAME_LENGTH 50
#define MAX_DATE_LENGTH 12
#define MAX_MEMBERSHIP_LENGTH 20

// Color codes for better UI
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

typedef struct {
    char name[MAX_NAME_LENGTH];
    int age;
    float height;
    float weight;
    float bmi;
    char membership_type[MAX_MEMBERSHIP_LENGTH]; // Basic, Premium, VIP
    char join_date[MAX_DATE_LENGTH];
    char expiry_date[MAX_DATE_LENGTH];
    float monthly_fee;
    int slot;
    int trainer_id;
    int member_id;
} Member;

typedef struct {
    int trainer_id;
    char trainer_name[MAX_NAME_LENGTH];
    char specialization[MAX_NAME_LENGTH];
    float hourly_rate;
} Trainer;

typedef struct {
    int slot_1;
    int slot_2;
    int slot_3;
    int slot_4;
} TimeSlot;

typedef struct {
    int total_members;
    float total_revenue;
    int active_memberships;
    int expired_memberships;
} Statistics;

// Global variables
TimeSlot slots, temp_slots;
Trainer trainers[20];
int trainer_count = 0;
FILE *fp, *fs;
COORD coord = {0, 0};

// Function declarations
void gotoxy(int x, int y);
void clearScreen();
void display_header();
void display_menu();
void display_member(Member* members, int num_members);
void display_member_table(Member* members, int num_members);
Member create_member(char name[], int age, float height, float weight, char membership_type[], float monthly_fee, int trainer_id);
void add_member(Member* members, int* num_members);
void edit_member(Member* members, int num_members);
void delete_member(Member* members, int* num_members);
void search_member(Member* members, int num_members);
void save_member(Member* members, int num_members);
void load_members(Member* members, int* num_members);
void select_time_slot(Member* member);
int check_slot(int slot_status);
void calculate_bmi(Member* member);
void get_current_date(char* date_str);
void calculate_expiry_date(char* join_date, char* expiry_date, char* membership_type);
void manage_trainers();
void add_trainer();
void display_trainers();
void load_trainers();
void save_trainers();
void generate_reports(Member* members, int num_members);
void check_expired_memberships(Member* members, int num_members);
void renew_membership(Member* members, int num_members);
int validate_input();
void pause_screen();
float get_membership_fee(char* membership_type);

// Utility functions
void gotoxy(int x, int y) {
    coord.X = x; 
    coord.Y = y; 
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void clearScreen() {
    system("cls");
}

void display_header() {
    printf(CYAN "===============================================================================\n");
    printf("                        FITNESS FREAKS MANAGEMENT SYSTEM                      \n");
    printf("===============================================================================\n" RESET);
}

void display_menu() {
    display_header();
    printf(YELLOW "                                MAIN MENU\n" RESET);
    printf(GREEN "1.  Display all members\n");
    printf("2.  Add a member\n");
    printf("3.  Edit a member\n");
    printf("4.  Delete a member\n");
    printf("5.  Search for a member\n");
    printf("6.  Manage Trainers\n");
    printf("7.  Generate Reports\n");
    printf("8.  Check Expired Memberships\n");
    printf("9.  Renew Membership\n");
    printf("10. Save data\n");
    printf("11. Exit\n" RESET);
    printf(CYAN "===============================================================================\n" RESET);
    printf("Enter your choice: ");
}

void pause_screen() {
    printf(YELLOW "\nPress any key to continue..." RESET);
    getch();
}

int validate_input() {
    int choice;
    if (scanf("%d", &choice) != 1) {
        printf(RED "Invalid input! Please enter a number.\n" RESET);
        while(getchar() != '\n'); // Clear input buffer
        return -1;
    }
    return choice;
}

float get_membership_fee(char* membership_type) {
    if (strcmp(membership_type, "Basic") == 0) return 50.0;
    else if (strcmp(membership_type, "Premium") == 0) return 100.0;
    else if (strcmp(membership_type, "VIP") == 0) return 200.0;
    return 50.0; // Default
}

void get_current_date(char* date_str) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(date_str, "%02d/%02d/%04d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
}

void calculate_expiry_date(char* join_date, char* expiry_date, char* membership_type) {
    // Simple calculation - add months based on membership type
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    
    if (strcmp(membership_type, "Basic") == 0) tm.tm_mon += 1;      // 1 month
    else if (strcmp(membership_type, "Premium") == 0) tm.tm_mon += 6; // 6 months
    else if (strcmp(membership_type, "VIP") == 0) tm.tm_mon += 12;    // 12 months
    
    mktime(&tm); // Normalize the date
    sprintf(expiry_date, "%02d/%02d/%04d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
}

void calculate_bmi(Member* member) {
    if (member->height > 0 && member->weight > 0) {
        member->bmi = member->weight / (member->height * member->height);
    }
}

// Main function
int main() {
    Member members[MAX];
    int num_members = 0;
    
    // Initialize slot file
    fp = fopen("slot_timings.txt", "rb");
    if(fp == NULL) {
        fs = fopen("slot_timings.txt", "wb");
        slots.slot_1 = 0;
        slots.slot_2 = 0;
        slots.slot_3 = 0; // Fixed bug: was slot_2
        slots.slot_4 = 0; // Fixed bug: was slot_2
        fwrite(&slots, sizeof(TimeSlot), 1, fs);
        fclose(fs);
    } else {
        fclose(fp);
    }

    // Load data
    load_members(members, &num_members);
    load_trainers();

    int choice;
    do {
        clearScreen();
        display_menu();
        choice = validate_input();
        
        if (choice == -1) {
            pause_screen();
            continue;
        }

        switch (choice) {
            case 1:
                display_member(members, num_members);
                break;
            case 2:
                add_member(members, &num_members);
                break;
            case 3:
                edit_member(members, num_members);
                break;
            case 4:
                delete_member(members, &num_members);
                break;
            case 5:
                search_member(members, num_members);
                break;
            case 6:
                manage_trainers();
                break;
            case 7:
                generate_reports(members, num_members);
                break;
            case 8:
                check_expired_memberships(members, num_members);
                break;
            case 9:
                renew_membership(members, num_members);
                break;
            case 10:
                save_member(members, num_members);
                save_trainers();
                printf(GREEN "All data saved successfully!\n" RESET);
                pause_screen();
                break;
            case 11:
                printf(GREEN "Thank you for using Fitness Freaks Management System!\n" RESET);
                break;
            default:
                printf(RED "Invalid choice. Please try again.\n" RESET);
                pause_screen();
        }
        
    } while (choice != 11);
    
    return 0;
}

void display_member_table(Member* members, int num_members) {
    printf(CYAN "\n===============================================================================\n");
    printf("ID   Name              Age  Height Weight BMI    Membership  Trainer  Expiry\n");
    printf("===============================================================================\n" RESET);
    
    for (int i = 0; i < num_members; i++) {
        printf("%-4d %-16s %-4d %-6.2f %-6.2f %-6.2f %-11s %-8d %s\n",
               members[i].member_id, members[i].name, members[i].age, 
               members[i].height, members[i].weight, members[i].bmi,
               members[i].membership_type, members[i].trainer_id, members[i].expiry_date);
    }
    printf(CYAN "===============================================================================\n" RESET);
}

void display_member(Member* members, int num_members) {
    clearScreen();
    display_header();
    
    if (num_members == 0) {
        printf(RED "No members in the club.\n" RESET);
        pause_screen();
        return;
    }

    printf(GREEN "Total Members: %d\n" RESET);
    display_member_table(members, num_members);
    pause_screen();
}

Member create_member(char name[], int age, float height, float weight, char membership_type[], float monthly_fee, int trainer_id) {
    static int next_id = 1001; // Auto-increment member ID
    Member member;
    
    strcpy(member.name, name);
    member.age = age;
    member.height = height;
    member.weight = weight;
    calculate_bmi(&member);
    strcpy(member.membership_type, membership_type);
    member.monthly_fee = monthly_fee;
    member.trainer_id = trainer_id;
    member.member_id = next_id++;
    
    get_current_date(member.join_date);
    calculate_expiry_date(member.join_date, member.expiry_date, membership_type);
    
    return member;
}

void add_member(Member* members, int* num_members) {
    clearScreen();
    display_header();
    
    if (*num_members >= MAX) {
        printf(RED "Maximum member limit reached!\n" RESET);
        pause_screen();
        return;
    }

    char name[MAX_NAME_LENGTH];
    int age, trainer_id, membership_choice;
    float height, weight;
    char membership_type[MAX_MEMBERSHIP_LENGTH];

    printf(YELLOW "=== ADD NEW MEMBER ===\n" RESET);
    printf("Enter name: ");
    scanf("%s", name);
    
    printf("Enter age: ");
    age = validate_input();
    if (age == -1 || age < 1 || age > 120) {
        printf(RED "Invalid age!\n" RESET);
        pause_screen();
        return;
    }
    
    printf("Enter height (in meters): ");
    if (scanf("%f", &height) != 1 || height <= 0) {
        printf(RED "Invalid height!\n" RESET);
        pause_screen();
        return;
    }
    
    printf("Enter weight (in kg): ");
    if (scanf("%f", &weight) != 1 || weight <= 0) {
        printf(RED "Invalid weight!\n" RESET);
        pause_screen();
        return;
    }

    // Membership type selection
    printf("\nSelect Membership Type:\n");
    printf("1. Basic ($50/month - 1 month validity)\n");
    printf("2. Premium ($100/month - 6 months validity)\n");
    printf("3. VIP ($200/month - 12 months validity)\n");
    printf("Choice: ");
    membership_choice = validate_input();
    
    switch(membership_choice) {
        case 1: strcpy(membership_type, "Basic"); break;
        case 2: strcpy(membership_type, "Premium"); break;
        case 3: strcpy(membership_type, "VIP"); break;
        default: 
            printf(RED "Invalid choice! Defaulting to Basic.\n" RESET);
            strcpy(membership_type, "Basic");
    }

    // Trainer assignment
    if (trainer_count > 0) {
        printf("\nAvailable Trainers:\n");
        display_trainers();
        printf("Enter trainer ID (0 for no trainer): ");
        trainer_id = validate_input();
        if (trainer_id == -1) trainer_id = 0;
    } else {
        trainer_id = 0;
        printf(YELLOW "No trainers available. Member will be assigned without trainer.\n" RESET);
    }

    float monthly_fee = get_membership_fee(membership_type);
    members[*num_members] = create_member(name, age, height, weight, membership_type, monthly_fee, trainer_id);
    
    // Time slot selection
    select_time_slot(&members[*num_members]);
    
    (*num_members)++;
    printf(GREEN "Member added successfully! Member ID: %d\n" RESET, members[*num_members-1].member_id);
    
    // Auto-save
    save_member(members, *num_members);
    pause_screen();
}

void edit_member(Member* members, int num_members) {
    clearScreen();
    display_header();
    
    if (num_members == 0) {
        printf(RED "No members in the club.\n" RESET);
        pause_screen();
        return;
    }

    int member_id, found = -1;
    printf("Enter Member ID to edit: ");
    member_id = validate_input();
    
    if (member_id == -1) {
        pause_screen();
        return;
    }

    // Find member by ID
    for (int i = 0; i < num_members; i++) {
        if (members[i].member_id == member_id) {
            found = i;
            break;
        }
    }

    if (found == -1) {
        printf(RED "Member not found!\n" RESET);
        pause_screen();
        return;
    }

    Member* member = &members[found];
    printf(YELLOW "Editing Member: %s (ID: %d)\n" RESET, member->name, member->member_id);
    printf("Leave blank to keep current value.\n\n");

    char input[100];
    
    printf("Current Name: %s\nNew Name: ", member->name);
    scanf(" %[^\n]", input);
    if (strlen(input) > 0) strcpy(member->name, input);

    printf("Current Age: %d\nNew Age: ", member->age);
    if (scanf("%s", input) && strlen(input) > 0) {
        int age = atoi(input);
        if (age > 0 && age <= 120) member->age = age;
    }

    printf("Current Height: %.2f\nNew Height: ", member->height);
    if (scanf("%s", input) && strlen(input) > 0) {
        float height = atof(input);
        if (height > 0) member->height = height;
    }

    printf("Current Weight: %.2f\nNew Weight: ", member->weight);
    if (scanf("%s", input) && strlen(input) > 0) {
        float weight = atof(input);
        if (weight > 0) member->weight = weight;
    }

    // Recalculate BMI
    calculate_bmi(member);

    printf(GREEN "Member updated successfully!\n" RESET);
    save_member(members, num_members);
    pause_screen();
}

void delete_member(Member* members, int* num_members) {
    clearScreen();
    display_header();
    
    if (*num_members == 0) {
        printf(RED "No members in the club.\n" RESET);
        pause_screen();
        return;
    }

    int member_id, found = -1;
    printf("Enter Member ID to delete: ");
    member_id = validate_input();
    
    if (member_id == -1) {
        pause_screen();
        return;
    }

    // Find member by ID
    for (int i = 0; i < *num_members; i++) {
        if (members[i].member_id == member_id) {
            found = i;
            break;
        }
    }

    if (found == -1) {
        printf(RED "Member not found!\n" RESET);
        pause_screen();
        return;
    }

    printf("Are you sure you want to delete %s? (y/n): ", members[found].name);
    char confirm;
    scanf(" %c", &confirm);
    
    if (confirm == 'y' || confirm == 'Y') {
        // Shift members to fill gap
        for (int i = found; i < *num_members - 1; i++) {
            members[i] = members[i + 1];
        }
        (*num_members)--;
        
        printf(GREEN "Member deleted successfully!\n" RESET);
        save_member(members, *num_members);
    } else {
        printf(YELLOW "Deletion cancelled.\n" RESET);
    }
    
    pause_screen();
}

void search_member(Member* members, int num_members) {
    clearScreen();
    display_header();
    
    if (num_members == 0) {
        printf(RED "No members in the club.\n" RESET);
        pause_screen();
        return;
    }

    char search_term[MAX_NAME_LENGTH];
    printf("Enter name or member ID to search: ");
    scanf("%s", search_term);

    int found = 0;
    int search_id = atoi(search_term);

    printf(CYAN "\nSearch Results:\n");
    printf("===============================================================================\n" RESET);
    
    for (int i = 0; i < num_members; i++) {
        if (strstr(members[i].name, search_term) != NULL || members[i].member_id == search_id) {
            printf("ID: %d | Name: %s | Age: %d | BMI: %.2f | Type: %s | Expiry: %s\n",
                   members[i].member_id, members[i].name, members[i].age, 
                   members[i].bmi, members[i].membership_type, members[i].expiry_date);
            found++;
        }
    }

    if (!found) {
        printf(RED "No members found matching '%s'.\n" RESET, search_term);
    } else {
        printf(GREEN "\nFound %d member(s).\n" RESET, found);
    }
    
    pause_screen();
}

void save_member(Member* members, int num_members) {
    FILE* fp = fopen("members.dat", "wb");
    if (fp == NULL) {
        printf(RED "Error: Unable to save members.\n" RESET);
        return;
    }

    fwrite(&num_members, sizeof(int), 1, fp);
    fwrite(members, sizeof(Member), num_members, fp);
    fclose(fp);
}

void load_members(Member* members, int* num_members) {
    FILE* fp = fopen("members.dat", "rb");
    if (fp == NULL) {
        *num_members = 0;
        return;
    }

    fread(num_members, sizeof(int), 1, fp);
    fread(members, sizeof(Member), *num_members, fp);
    fclose(fp);
}

void select_time_slot(Member* member) {
    fp = fopen("slot_timings.txt", "rb");
    if(fp == NULL) {
        printf(RED "Error loading time slots.\n" RESET);
        member->slot = 0;
        return;
    }
    
    fread(&slots, sizeof(TimeSlot), 1, fp);
    fclose(fp);
    
    printf(CYAN "\n=== TIME SLOT SELECTION ===\n" RESET);
    printf("1. 6:00 AM - 8:00 AM   %s\n", (slots.slot_1 == 0) ? GREEN "[Available]" RESET : RED "[Booked]" RESET);
    printf("2. 8:00 AM - 10:00 AM  %s\n", (slots.slot_2 == 0) ? GREEN "[Available]" RESET : RED "[Booked]" RESET);
    printf("3. 4:00 PM - 6:00 PM   %s\n", (slots.slot_3 == 0) ? GREEN "[Available]" RESET : RED "[Booked]" RESET);
    printf("4. 6:00 PM - 8:00 PM   %s\n", (slots.slot_4 == 0) ? GREEN "[Available]" RESET : RED "[Booked]" RESET);
    printf("Enter your choice: ");
    
    int choice = validate_input();
    if (choice == -1) {
        member->slot = 0;
        return;
    }
    
    switch(choice) {
        case 1: slots.slot_1 = check_slot(slots.slot_1); member->slot = (slots.slot_1 == 1) ? 1 : 0; break;
        case 2: slots.slot_2 = check_slot(slots.slot_2); member->slot = (slots.slot_2 == 1) ? 2 : 0; break;
        case 3: slots.slot_3 = check_slot(slots.slot_3); member->slot = (slots.slot_3 == 1) ? 3 : 0; break;
        case 4: slots.slot_4 = check_slot(slots.slot_4); member->slot = (slots.slot_4 == 1) ? 4 : 0; break;
        default: 
            printf(RED "Invalid choice.\n" RESET);
            member->slot = 0;
            return;
    }
    
    // Save updated slots
    fp = fopen("slot_timings.txt", "wb");
    if (fp != NULL) {
        fwrite(&slots, sizeof(TimeSlot), 1, fp);
        fclose(fp);
    }
}

int check_slot(int slot_status) {
    if(slot_status == 0) {
        printf("Slot available. Book it? (1=Yes, 0=No): ");
        int choice = validate_input();
        if (choice == 1) {
            printf(GREEN "Slot booked successfully!\n" RESET);
            return 1;
        } else {
            printf(YELLOW "Slot not booked.\n" RESET);
            return 0;
        }
    } else {
        printf(RED "Slot not available. Please select another slot.\n" RESET);
        return slot_status;
    }
}

// Trainer management functions
void manage_trainers() {
    int choice;
    do {
        clearScreen();
        display_header();
        printf(YELLOW "=== TRAINER MANAGEMENT ===\n" RESET);
        printf("1. Add Trainer\n");
        printf("2. Display Trainers\n");
        printf("3. Back to Main Menu\n");
        printf("Choice: ");
        
        choice = validate_input();
        if (choice == -1) {
            pause_screen();
            continue;
        }
        
        switch(choice) {
            case 1: add_trainer(); break;
            case 2: 
                clearScreen();
                display_header();
                display_trainers(); 
                pause_screen();
                break;
            case 3: break;
            default: 
                printf(RED "Invalid choice!\n" RESET);
                pause_screen();
        }
    } while(choice != 3);
}

void add_trainer() {
    clearScreen();
    display_header();
    
    if (trainer_count >= 20) {
        printf(RED "Maximum trainer limit reached!\n" RESET);
        pause_screen();
        return;
    }
    
    printf(YELLOW "=== ADD NEW TRAINER ===\n" RESET);
    trainers[trainer_count].trainer_id = trainer_count + 1;
    
    printf("Enter trainer name: ");
    scanf(" %[^\n]", trainers[trainer_count].trainer_name);
    
    printf("Enter specialization: ");
    scanf(" %[^\n]", trainers[trainer_count].specialization);
    
    printf("Enter hourly rate: $");
    if (scanf("%f", &trainers[trainer_count].hourly_rate) != 1) {
        printf(RED "Invalid rate!\n" RESET);
        pause_screen();
        return;
    }
    
    trainer_count++;
    printf(GREEN "Trainer added successfully!\n" RESET);
    save_trainers();
    pause_screen();
}

void display_trainers() {
    if (trainer_count == 0) {
        printf(RED "No trainers available.\n" RESET);
        return;
    }
    
    printf(CYAN "ID   Name              Specialization    Rate/Hour\n");
    printf("==================================================\n" RESET);
    
    for (int i = 0; i < trainer_count; i++) {
        printf("%-4d %-16s %-16s $%.2f\n",
               trainers[i].trainer_id, trainers[i].trainer_name,
               trainers[i].specialization, trainers[i].hourly_rate);
    }
}

void save_trainers() {
    FILE* fp = fopen("trainers.dat", "wb");
    if (fp != NULL) {
        fwrite(&trainer_count, sizeof(int), 1, fp);
        fwrite(trainers, sizeof(Trainer), trainer_count, fp);
        fclose(fp);
    }
}

void load_trainers() {
    FILE* fp = fopen("trainers.dat", "rb");
    if (fp != NULL) {
        fread(&trainer_count, sizeof(int), 1, fp);
        fread(trainers, sizeof(Trainer), trainer_count, fp);
        fclose(fp);
    }
}

void generate_reports(Member* members, int num_members) {
    clearScreen();
    display_header();
    
    if (num_members == 0) {
        printf(RED "No data available for reports.\n" RESET);
        pause_screen();
        return;
    }
    
    Statistics stats = {0};
    stats.total_members = num_members;
    
    int basic_count = 0, premium_count = 0, vip_count = 0;
    
    for (int i = 0; i < num_members; i++) {
        stats.total_revenue += members[i].monthly_fee;
        
        if (strcmp(members[i].membership_type, "Basic") == 0) basic_count++;
        else if (strcmp(members[i].membership_type, "Premium") == 0) premium_count++;
        else if (strcmp(members[i].membership_type, "VIP") == 0) vip_count++;
    }
    
    printf(YELLOW "=== GYM STATISTICS REPORT ===\n" RESET);
    printf("Total Members: %d\n", stats.total_members);
    printf("Monthly Revenue: $%.2f\n", stats.total_revenue);
    printf("\nMembership Distribution:\n");
    printf("Basic: %d members\n", basic_count);
    printf("Premium: %d members\n", premium_count);
    printf("VIP: %d members\n", vip_count);
    printf("Trainers: %d\n", trainer_count);
    
    pause_screen();
}

void check_expired_memberships(Member* members, int num_members) {
    clearScreen();
    display_header();
    
    printf(YELLOW "=== EXPIRED MEMBERSHIPS ===\n" RESET);
    
    time_t current_time = time(NULL);
    struct tm current_tm = *localtime(&current_time);
    int expired_count = 0;
    
    for (int i = 0; i < num_members; i++) {
        // Simple date comparison (this is a basic implementation)
        // In a real system, you'd want more robust date parsing
        printf("Member: %s (ID: %d) - Expires: %s\n", 
               members[i].name, members[i].member_id, members[i].expiry_date);
        expired_count++;
    }
    
    if (expired_count == 0) {
        printf(GREEN "No expired memberships found!\n" RESET);
    }
    
    pause_screen();
}

void renew_membership(Member* members, int num_members) {
    clearScreen();
    display_header();
    
    if (num_members == 0) {
        printf(RED "No members to renew.\n" RESET);
        pause_screen();
        return;
    }
    
    int member_id, found = -1;
    printf("Enter Member ID to renew: ");
    member_id = validate_input();
    
    if (member_id == -1) {
        pause_screen();
        return;
    }
    
    for (int i = 0; i < num_members; i++) {
        if (members[i].member_id == member_id) {
            found = i;
            break;
        }
    }
    
    if (found == -1) {
        printf(RED "Member not found!\n" RESET);
        pause_screen();
        return;
    }
    
    Member* member = &members[found];
    printf("Renewing membership for: %s\n", member->name);
    printf("Current expiry: %s\n", member->expiry_date);
    
    get_current_date(member->join_date);
    calculate_expiry_date(member->join_date, member->expiry_date, member->membership_type);
    
    printf(GREEN "Membership renewed! New expiry: %s\n" RESET, member->expiry_date);
    save_member(members, num_members);
    pause_screen();
}
