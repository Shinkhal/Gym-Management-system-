#include<windows.h>

#include<stdio.h>

#include<conio.h>

#include <stdlib.h>

#include<string.h>                 


#define MAX 1
#define insert_num 1
#define del_num -1
#define MAX_NAME_LENGTH 50
#define MAX_LINE_LENGTH 100

typedef struct {
    char name[MAX_NAME_LENGTH];
    int age;
    float height;
    float weight;
    float bmi;
    int slot;
} Member;

typedef struct slot
{
    int slot_1;
    int slot_2;
    int slot_3;
    int slot_4;
} slot;

slot s, p;
int i;
FILE *fp, *fs;


COORD coord = {0, 0};

void gotoxy (int x, int y)

{

coord.X = x; coord.Y = y; 

SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

}


// Function declarations
void display_member(Member* members, int num_members);
Member create_member(char name[], int age, float height, float weight, float bmi);
void add_member(Member* members, int* num_members);
void edit_member(Member* members, int num_members);
void delete_member(Member* members, int* num_members);
void search_member(Member* members, int num_members);
void save_member(Member* members, int num_members);
void load_members(Member* members, int* num_members);
void print_members(Member* members, int* num_members);
void pack_details();
void print_slot();
void clearScreen();
void select_time_slot();

int check_slot(int);
// Main function
int main() {
    Member members[100];
    int num_members = 0;
    fp = fopen("slot_timings.txt", "rb");
    if(fp==NULL)
    {
        fs = fopen("slot_timings.txt", "wb");
        p.slot_1 = 0;
        p.slot_2 = 0;
        p.slot_2 = 0;
        p.slot_2 = 0;
        fwrite(&p, sizeof(slot), 1, fs);
    }
    fclose(fp);
    fclose(fs);

    // Load members from file
    load_members(members, &num_members);

	clearScreen();

    int choice;
    do {
    	gotoxy(40,1);
       printf(" \t\tWELCOME TO FITNESS FREAKS\n ");
       gotoxy(40,3);
		printf("1. Display all members\n");
		gotoxy(40,5);
        printf("2. Add a member\n");
        gotoxy(40,7);
        printf("3. Edit a member\n");
        gotoxy(40,9);
        printf("4. Delete a member\n");
        gotoxy(40,11);
        printf("5. Search for a member\n");
        gotoxy(40,13);
        printf("6. Save members to file\n");
        gotoxy(40,15);
        printf("7. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

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
                save_member(members, num_members);
                break;
            case 7:
                printf("Exiting program...\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
        
    } while (choice != 7);
clearScreen();
    return 0;
}


// Function for displaying all members
void display_member(Member* members, int num_members) {
	clearScreen();
	
    if (num_members == 0) {
        printf("No members in the club.\n");
        return;
    }

    printf("List of members:\n");
    for (int i = 0; i < num_members; i++) {
        printf("%d. Name: %s, Age: %d, Height: %.2f, Weight: %.2f, BMI: %.2f\n",
               i+1, members[i].name, members[i].age, members[i].height, members[i].weight, members[i].bmi);
    }
    char choice;
    printf("Go to main menu? (y/n): ");
    scanf(" %c", &choice);

    if (choice == 'y' || choice == 'Y') {
    	clearScreen();
        return;
    } else if (choice == 'n' || choice == 'N') {
        exit(0);
    } else {
        printf("Invalid input. Exiting program...\n");
        exit(0);
    }
}





// Function for creating a member
Member create_member(char name[], int age, float height, float weight, float bmi) {
	
    Member member;
    strcpy(member.name, name);
    member.age = age;
    member.height = height;
    member.weight = weight;
    member.bmi = bmi;
    
    return member;
    
    
}

// Function for adding a member

void add_member(Member* members, int* num_members) {
    char name[MAX_NAME_LENGTH];
    int age;
    float height, weight,bmi;
     FILE *fp, *fs;
    int resume = 1;

    
    clearScreen();

    printf("Enter name of member: ");
    scanf("%s", name);
    printf("Enter age of member: ");
    scanf("%d", &age);
    printf("Enter height of member (in meters): ");
    scanf("%f", &height);
    printf("Enter weight of member (in kilograms): ");
    scanf("%f", &weight);
    
    bmi=weight/(height*height);
   
    select_time_slot();

    members[*num_members] = create_member(name, age, height, weight, bmi);
    (*num_members)++;

    printf("Member added successfully.\n");

    // Save members to file
    save_member(members, *num_members);
    
    char choice;
    printf("Go to main menu? (y/n): ");
    scanf(" %c", &choice);

    if (choice == 'y' || choice == 'Y') {
    	clearScreen();
        return;
    } else if (choice == 'n' || choice == 'N') {
        exit(0);
    } else {
        printf("Invalid input. Exiting program...\n");
        exit(0);
    }
    
}

// Function for editing a member
void edit_member(Member* members, int num_members) {
	clearScreen();
    if (num_members == 0) {
        printf("No members in the club.\n");
        return;
    }

    int index;
    printf("Enter index of member to edit: ");
    scanf("%d", &index);

    if (index < 1 || index > num_members) {
        printf("Invalid index. Please try again.\n");
        return;
    }

    Member member = members[index-1];
    char name[MAX_NAME_LENGTH];
    int age;
    float height, weight,bmi;

    printf("Enter new name of member (%s): ", member.name);
    scanf("%s", name);
    printf("Enter new age of member (%d): ", member.age);
    scanf("%d", &age);
    printf("Enter new height of member (in meters) (%.2f): ", member.height);
    scanf("%f", &height);
    printf("Enter new weight of member (in kilograms) (%.2f): ", member.weight);
    scanf("%f", &weight);

    if (strcmp(name, "") != 0) {
        strcpy(member.name, name);
    }
    if (age > 0) {
        member.age = age;
    }
    if (height > 0) {
        member.height = height;
    }
    if (weight > 0) {
        member.weight = weight;
    }

    members[index-1] = member;

    printf("Member edited successfully.\n");

    // Save members to file
    save_member(members, num_members);
        char choice;
    printf("Go to main menu? (y/n): ");
    scanf(" %c", &choice);

    if (choice == 'y' || choice == 'Y') {
    	clearScreen();
        return;
    } else if (choice == 'n' || choice == 'N') {
        exit(0);
    } else {
        printf("Invalid input. Exiting program...\n");
        exit(0);
    }
    

}

// Function for deleting a member
void delete_member(Member* members, int* num_members) {
	clearScreen();
    if (*num_members == 0) {
        printf("No members in the club.\n");
        return;
    }

    int index;
    printf("Enter index of member to delete: ");
    scanf("%d", &index);

    if (index < 1 || index > *num_members) {
        printf("Invalid index. Please try again.\n");
        return;
    }

    // Shift members to the left to fill gap
    for (int i = index-1; i < *num_members-1; i++) {
        members[i] = members[i+1];
    }
    (*num_members)--;

    printf("Member deleted successfully.\n");

    // Save members to file
    save_member(members, *num_members);
        char choice;
    printf("Go to main menu? (y/n): ");
    scanf(" %c", &choice);

    if (choice == 'y' || choice == 'Y') {
    	clearScreen();
        return;
    } else if (choice == 'n' || choice == 'N') {
        exit(0);
    } else {
        printf("Invalid input. Exiting program...\n");
        exit(0);
    }
    
}
// Function for printing a member 
void print_member(Member member) {
	clearScreen();
    printf("Name: %s\n", member.name);
    printf("Age: %d\n", member.age);
    printf("Height %.2f\n", member.height);
    printf("weight %.2f\n", member.weight);
    printf("BMI %.2f\n", member.bmi);
    printf("\n");
    
    
    
}

// Function for searching for a member
void search_member(Member* members, int num_members) {
	clearScreen();
    if (num_members == 0) {
        printf("No members in the club.\n");
        return;
    }

    char name[MAX_NAME_LENGTH];
    printf("Enter name of member to search for: ");
    scanf("%s", name);

    int found = 0;
    for (int i = 0; i < num_members; i++) {
        if (strcmp(name, members[i].name) == 0) {
            print_member(members[i]);
            found = 1;
        }
    }

    if (!found) {
        printf("No members found with name %s.\n", name);
    }
        char choice;
    printf("Go to main menu? (y/n): ");
    scanf(" %c", &choice);

    if (choice == 'y' || choice == 'Y') {
    	clearScreen();
        return;
    } else if (choice == 'n' || choice == 'N') {
        exit(0);
    } else {
        printf("Invalid input. Exiting program...\n");
        exit(0);
    }
}

// Function for saving members to file
void save_member(Member* members, int num_members) {
	clearScreen();
    FILE* fp = fopen("members.txt", "w");
    if (fp == NULL) {
        printf("Error: Unable to save members.\n");
        return;
    }

    fprintf(fp, "%d\n", num_members);

    for (int i = 0; i < num_members; i++) {
        fprintf(fp, "%s,%d,%.2f,%.2f,%.2f\n", members[i].name, members[i].age, members[i].height, members[i].weight, members[i].bmi);
    }

    fclose(fp);
    printf("Members saved to file.\n");


}

// Function for loading members from file
void load_members(Member* members, int* num_members) {
    FILE* fp = fopen("members.txt", "r");
    if (fp == NULL) {
        printf("Error: Unable to load members.\n");
        return;
    }

    fscanf(fp, "%d\n", num_members);

    for (int i = 0; i < *num_members; i++) {
        char name[MAX_NAME_LENGTH];
        int age;
        float height, weight, bmi;

        fscanf(fp, "%[^,],%d,%f,%f,%f\n", name, &age, &height, &weight, &bmi);

        members[i] = create_member(name, age, height, weight, bmi);
    }

    fclose(fp);
    printf("Members loaded from file.\n");
}   
void select_time_slot() {
	
    fp = fopen("slot_timings.txt", "rb");
    if(fp==NULL)
    {
        fs = fopen("slot_timings.txt", "wb");
        p.slot_1 = 0;
        p.slot_2 = 0;
        p.slot_3 = 0;
        p.slot_4 = 0;
        fwrite(&p, sizeof(slot), 1, fs);
        fclose(fs);
    }
    else
    {
        fs = fopen("temp.txt", "wb");
        fread(&s, sizeof(slot), 1, fp);
        fclose(fp);
        printf("Please select a time slot:\n");
        printf("1. 6:00 AM - 8:00 AM");
        if(s.slot_1 == 0) printf(" (available)\n"); else printf(" (booked)\n");
        printf("2. 8:00 AM - 10:00 AM");
        if(s.slot_2 == 0) printf(" (available)\n"); else printf(" (booked)\n");
        printf("3. 4:00 PM - 6:00 PM");
        if(s.slot_3 == 0) printf(" (available)\n"); else printf(" (booked)\n");
        printf("4. 6:00 PM - 8:00 PM");
        if(s.slot_4 == 0) printf(" (available)\n"); else printf(" (booked)\n");
        printf("Enter your choice: ");
        scanf("%d", &i);
        switch(i)
        {
            case 1: s.slot_1 = check_slot(s.slot_1);
                    break;
            case 2: s.slot_2 = check_slot(s.slot_2);
                    break;
            case 3: s.slot_3 = check_slot(s.slot_3);
                    break;
            case 4: s.slot_4 = check_slot(s.slot_4);
                    break;
            default: printf("Invalid choice\n");
                     break;
        }
        fwrite(&s, sizeof(slot), 1, fs);
        fclose(fs);
        remove("slot_timings.txt");
        rename("temp.txt", "slot_timings.txt");
    }
}

int check_slot(int a)
{
    if(a == 0)
    {
        printf("Slot available. Do you want to book it? (1/0): ");
        scanf("%d", &a);
        if(a == 1)
        {
            printf("Slot booked successfully.\n");
            return 1;
        }
        else
        {
            printf("Slot not booked.\n");
            return 0;
        }
    }
    else
    {
        printf("Slot not available. Please select another slot.\n");
        return a;
    }
}
void clearScreen() {
    system("clear||cls");
}
