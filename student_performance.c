#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Structure for Attendance Record
typedef struct Attendance {
    char date[11];  // YYYY-MM-DD format
    char status;    // 'P' for Present, 'A' for Absent, 'L' for Late, 'H' for Half-day
    struct Attendance* next;
} Attendance;

// Structure for Extracurricular Activity
typedef struct Activity {
    char activity[50];
    struct Activity* next;
} Activity;

// Structure for Student
typedef struct Student {
    int rollNo;
    char name[50];
    int semester;
    float marks[5]; // 5 subjects
    Activity* extracurricular; // Linked list of activities
    char sports[100];
    Attendance* attendanceRecords; // Linked list of attendance
    int totalPresent;
    int totalAbsent;
    int totalLate;
    int totalHalfDay;
    float attendancePercentage;
    struct Student* next;
} Student;

// Structure for Stack (for attendance undo functionality)
typedef struct AttendanceStack {
    int rollNo;
    char date[11];
    char previousStatus;
    struct AttendanceStack* next;
} AttendanceStack;

// Structure for Queue (for processing students)
typedef struct Queue {
    Student* student;
    struct Queue* next;
} Queue;

// Global pointers
Student* studentList = NULL;
AttendanceStack* undoStack = NULL;
Queue* processQueue = NULL;

// Function prototypes
// Student Management
void addStudent();
void displayAllStudents();
void addExtracurricularActivity();
void displayExtracurricular();
void displaySports();

// Marks Analysis
void findLowestMarksIndividual();
void findLowestMarksAll();
void compareStudentSemesters();
void displayLowestMarks(Student* student);
void compareIndividualStudent(int rollNo);

// Attendance System
void markAttendance();
void markBulkAttendance();
void viewStudentAttendance();
void viewAllAttendance();
void calculateAttendancePercentage(Student* student);
void lowAttendanceAlerts();
void monthlyAttendanceReport();
void undoLastAttendance();

// Queue Operations
void processQueueStudents();
void enqueue(Student* student);
Student* dequeue();

// Stack Operations (Attendance Undo)
void pushToUndoStack(int rollNo, char* date, char previousStatus);
int popFromUndoStack(int* rollNo, char* date, char* previousStatus);

// Utility Functions
float calculateAverage(Student* student);
void displayStudent(Student* student);
Student* searchStudent(int rollNo);
Activity* createActivity(char* activityName);
void addActivityToStudent(Student* student, char* activityName);
void displayStudentActivities(Student* student);
void freeMemory();
int validateDate(char* date);

// Main function
int main() {
    int choice;
    
    printf("=== STUDENT EVALUATION SYSTEM WITH ATTENDANCE ===\n");
    
    while(1) {
        printf("\n=== MAIN MENU ===\n");
        printf("1. Add Student\n");
        printf("2. Display All Students\n");
        printf("3. Add Extracurricular Activity\n");
        printf("4. Display Extracurricular Activities\n");
        printf("5. Display Sports Activities\n");
        printf("6. Find Lowest Marks (Individual Student)\n");
        printf("7. Find Lowest Marks (All Students)\n");
        printf("8. Compare Student Semesters\n");
        printf("9. Mark Attendance\n");
        printf("10. Mark Bulk Attendance\n");
        printf("11. View Student Attendance\n");
        printf("12. View All Attendance\n");
        printf("13. Low Attendance Alerts\n");
        printf("14. Monthly Attendance Report\n");
        printf("15. Undo Last Attendance\n");
        printf("16. Process Students (Queue Demo)\n");
        printf("17. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        
        switch(choice) {
            case 1:
                addStudent();
                break;
            case 2:
                displayAllStudents();
                break;
            case 3:
                addExtracurricularActivity();
                break;
            case 4:
                displayExtracurricular();
                break;
            case 5:
                displaySports();
                break;
            case 6:
                findLowestMarksIndividual();
                break;
            case 7:
                findLowestMarksAll();
                break;
            case 8:
                compareStudentSemesters();
                break;
            case 9:
                markAttendance();
                break;
            case 10:
                markBulkAttendance();
                break;
            case 11:
                viewStudentAttendance();
                break;
            case 12:
                viewAllAttendance();
                break;
            case 13:
                lowAttendanceAlerts();
                break;
            case 14:
                monthlyAttendanceReport();
                break;
            case 15:
                undoLastAttendance();
                break;
            case 16:
                processQueueStudents();
                break;
            case 17:
                freeMemory();
                printf("Thank you for using Student Evaluation System!\n");
                exit(0);
            default:
                printf("Invalid choice! Please try again.\n");
        }
    }
    
    return 0;
}

// ==================== ATTENDANCE SYSTEM FUNCTIONS ====================

// Mark attendance for individual student
void markAttendance() {
    if(studentList == NULL) {
        printf("No students found!\n");
        return;
    }
    
    int rollNo;
    char date[11];
    char status;
    
    printf("\n=== MARK INDIVIDUAL ATTENDANCE ===\n");
    printf("Enter student roll number: ");
    scanf("%d", &rollNo);
    
    Student* student = searchStudent(rollNo);
    if(student == NULL) {
        printf("Student with roll number %d not found!\n", rollNo);
        return;
    }
    
    printf("Enter date (YYYY-MM-DD): ");
    scanf("%s", date);
    
    if(!validateDate(date)) {
        printf("Invalid date format! Use YYYY-MM-DD.\n");
        return;
    }
    
    // Check if attendance already exists for this date
    Attendance* current = student->attendanceRecords;
    while(current != NULL) {
        if(strcmp(current->date, date) == 0) {
            printf("Attendance already marked for this date. Previous status: %c\n", current->status);
            printf("Do you want to update? (y/n): ");
            char choice;
            scanf(" %c", &choice);
            if(choice == 'y' || choice == 'Y') {
                // Store previous status for undo
                pushToUndoStack(rollNo, date, current->status);
                
                printf("Enter new status (P-Present, A-Absent, L-Late, H-Half-day): ");
                scanf(" %c", &status);
                status = toupper(status);
                
                if(status == 'P' || status == 'A' || status == 'L' || status == 'H') {
                    // Update counters
                    switch(current->status) {
                        case 'P': student->totalPresent--; break;
                        case 'A': student->totalAbsent--; break;
                        case 'L': student->totalLate--; break;
                        case 'H': student->totalHalfDay--; break;
                    }
                    
                    current->status = status;
                    
                    switch(status) {
                        case 'P': student->totalPresent++; break;
                        case 'A': student->totalAbsent++; break;
                        case 'L': student->totalLate++; break;
                        case 'H': student->totalHalfDay++; break;
                    }
                    
                    calculateAttendancePercentage(student);
                    printf("Attendance updated successfully!\n");
                } else {
                    printf("Invalid status! Use P, A, L, or H.\n");
                }
            }
            return;
        }
        current = current->next;
    }
    
    printf("Enter status (P-Present, A-Absent, L-Late, H-Half-day): ");
    scanf(" %c", &status);
    status = toupper(status);
    
    if(status != 'P' && status != 'A' && status != 'L' && status != 'H') {
        printf("Invalid status! Use P, A, L, or H.\n");
        return;
    }
    
    // Create new attendance record
    Attendance* newAttendance = (Attendance*)malloc(sizeof(Attendance));
    strcpy(newAttendance->date, date);
    newAttendance->status = status;
    newAttendance->next = student->attendanceRecords;
    student->attendanceRecords = newAttendance;
    
    // Update counters
    switch(status) {
        case 'P': student->totalPresent++; break;
        case 'A': student->totalAbsent++; break;
        case 'L': student->totalLate++; break;
        case 'H': student->totalHalfDay++; break;
    }
    
    calculateAttendancePercentage(student);
    printf("Attendance marked successfully for %s!\n", student->name);
}

// Mark bulk attendance for all students on a specific date
void markBulkAttendance() {
    if(studentList == NULL) {
        printf("No students found!\n");
        return;
    }
    
    char date[11];
    printf("\n=== MARK BULK ATTENDANCE ===\n");
    printf("Enter date for bulk attendance (YYYY-MM-DD): ");
    scanf("%s", date);
    
    if(!validateDate(date)) {
        printf("Invalid date format! Use YYYY-MM-DD.\n");
        return;
    }
    
    Student* current = studentList;
    while(current != NULL) {
        printf("\nStudent: %s (Roll No: %d)\n", current->name, current->rollNo);
        printf("Enter status (P-Present, A-Absent, L-Late, H-Half-day): ");
        char status;
        scanf(" %c", &status);
        status = toupper(status);
        
        if(status == 'P' || status == 'A' || status == 'L' || status == 'H') {
            // Check if attendance already exists
            Attendance* att = current->attendanceRecords;
            int exists = 0;
            while(att != NULL) {
                if(strcmp(att->date, date) == 0) {
                    exists = 1;
                    break;
                }
                att = att->next;
            }
            
            if(!exists) {
                Attendance* newAttendance = (Attendance*)malloc(sizeof(Attendance));
                strcpy(newAttendance->date, date);
                newAttendance->status = status;
                newAttendance->next = current->attendanceRecords;
                current->attendanceRecords = newAttendance;
                
                switch(status) {
                    case 'P': current->totalPresent++; break;
                    case 'A': current->totalAbsent++; break;
                    case 'L': current->totalLate++; break;
                    case 'H': current->totalHalfDay++; break;
                }
                
                calculateAttendancePercentage(current);
            }
        }
        current = current->next;
    }
    printf("Bulk attendance marked successfully!\n");
}

// View attendance for individual student
void viewStudentAttendance() {
    if(studentList == NULL) {
        printf("No students found!\n");
        return;
    }
    
    int rollNo;
    printf("Enter student roll number: ");
    scanf("%d", &rollNo);
    
    Student* student = searchStudent(rollNo);
    if(student == NULL) {
        printf("Student with roll number %d not found!\n", rollNo);
        return;
    }
    
    printf("\n=== ATTENDANCE RECORDS FOR %s ===\n", student->name);
    printf("Roll No: %d, Semester: %d\n", student->rollNo, student->semester);
    printf("Total Present: %d, Absent: %d, Late: %d, Half-day: %d\n", 
           student->totalPresent, student->totalAbsent, student->totalLate, student->totalHalfDay);
    printf("Attendance Percentage: %.2f%%\n", student->attendancePercentage);
    
    if(student->attendanceRecords == NULL) {
        printf("No attendance records found!\n");
        return;
    }
    
    printf("\nDate       | Status\n");
    printf("-----------|-------\n");
    
    Attendance* current = student->attendanceRecords;
    while(current != NULL) {
        char* statusText;
        switch(current->status) {
            case 'P': statusText = "Present"; break;
            case 'A': statusText = "Absent"; break;
            case 'L': statusText = "Late"; break;
            case 'H': statusText = "Half-day"; break;
            default: statusText = "Unknown";
        }
        printf("%s | %s\n", current->date, statusText);
        current = current->next;
    }
}

// View attendance summary for all students
void viewAllAttendance() {
    if(studentList == NULL) {
        printf("No students found!\n");
        return;
    }
    
    printf("\n=== ATTENDANCE SUMMARY - ALL STUDENTS ===\n");
    printf("Roll No | Name                | Present | Absent | Late | Half-day | Percentage\n");
    printf("--------|---------------------|---------|--------|------|----------|-----------\n");
    
    Student* current = studentList;
    while(current != NULL) {
        printf("%7d | %-19s | %7d | %6d | %4d | %8d | %9.2f%%\n",
               current->rollNo, current->name, current->totalPresent,
               current->totalAbsent, current->totalLate, current->totalHalfDay,
               current->attendancePercentage);
        current = current->next;
    }
}

// Calculate attendance percentage for a student
void calculateAttendancePercentage(Student* student) {
    int totalDays = student->totalPresent + student->totalAbsent + student->totalLate + student->totalHalfDay;
    if(totalDays == 0) {
        student->attendancePercentage = 0.0;
        return;
    }
    
    // Calculate with weights: Present=1, Late=0.75, Half-day=0.5, Absent=0
    float weightedScore = student->totalPresent * 1.0 + 
                         student->totalLate * 0.75 + 
                         student->totalHalfDay * 0.5;
    
    student->attendancePercentage = (weightedScore / totalDays) * 100;
}

// Show students with low attendance
void lowAttendanceAlerts() {
    if(studentList == NULL) {
        printf("No students found!\n");
        return;
    }
    
    float threshold;
    printf("Enter attendance threshold percentage (default 75): ");
    if(scanf("%f", &threshold) != 1) {
        threshold = 75.0;
    }
    
    printf("\n=== LOW ATTENDANCE ALERTS (< %.1f%%) ===\n", threshold);
    int found = 0;
    
    Student* current = studentList;
    while(current != NULL) {
        if(current->attendancePercentage < threshold) {
            printf("Roll No: %d, Name: %s\n", current->rollNo, current->name);
            printf("Attendance: %.2f%%, Present: %d, Absent: %d, Late: %d\n",
                   current->attendancePercentage, current->totalPresent,
                   current->totalAbsent, current->totalLate);
            
            // Check if student also has low marks
            float avgMarks = calculateAverage(current);
            if(avgMarks < 40) {
                printf("WARNING: Also has low academic performance (Avg: %.2f)\n", avgMarks);
            }
            printf("\n");
            found = 1;
        }
        current = current->next;
    }
    
    if(!found) {
        printf("No students found with attendance below %.1f%%\n", threshold);
    }
}

// Generate monthly attendance report
void monthlyAttendanceReport() {
    if(studentList == NULL) {
        printf("No students found!\n");
        return;
    }
    
    char month[8]; // YYYY-MM format
    printf("Enter month for report (YYYY-MM): ");
    scanf("%s", month);
    
    printf("\n=== MONTHLY ATTENDANCE REPORT FOR %s ===\n", month);
    printf("Roll No | Name                | Working Days | Present | Percentage\n");
    printf("--------|---------------------|--------------|---------|-----------\n");
    
    Student* currentStudent = studentList;
    while(currentStudent != NULL) {
        int monthlyPresent = 0, monthlyTotal = 0;
        
        Attendance* currentAtt = currentStudent->attendanceRecords;
        while(currentAtt != NULL) {
            if(strncmp(currentAtt->date, month, 7) == 0) { // Compare YYYY-MM
                monthlyTotal++;
                if(currentAtt->status == 'P') {
                    monthlyPresent++;
                }
            }
            currentAtt = currentAtt->next;
        }
        
        float monthlyPercentage = (monthlyTotal > 0) ? ((float)monthlyPresent / monthlyTotal) * 100 : 0;
        
        printf("%7d | %-19s | %12d | %7d | %9.2f%%\n",
               currentStudent->rollNo, currentStudent->name,
               monthlyTotal, monthlyPresent, monthlyPercentage);
        
        currentStudent = currentStudent->next;
    }
}

// Undo last attendance operation
void undoLastAttendance() {
    int rollNo;
    char date[11];
    char previousStatus;
    
    if(popFromUndoStack(&rollNo, date, &previousStatus)) {
        Student* student = searchStudent(rollNo);
        if(student != NULL) {
            Attendance* current = student->attendanceRecords;
            while(current != NULL) {
                if(strcmp(current->date, date) == 0) {
                    // Revert to previous status
                    switch(current->status) {
                        case 'P': student->totalPresent--; break;
                        case 'A': student->totalAbsent--; break;
                        case 'L': student->totalLate--; break;
                        case 'H': student->totalHalfDay--; break;
                    }
                    
                    current->status = previousStatus;
                    
                    switch(previousStatus) {
                        case 'P': student->totalPresent++; break;
                        case 'A': student->totalAbsent++; break;
                        case 'L': student->totalLate++; break;
                        case 'H': student->totalHalfDay++; break;
                    }
                    
                    calculateAttendancePercentage(student);
                    printf("Attendance undone successfully for %s on %s\n", student->name, date);
                    return;
                }
                current = current->next;
            }
        }
    } else {
        printf("No attendance operations to undo!\n");
    }
}

// ==================== STACK OPERATIONS (ATTENDANCE UNDO) ====================

void pushToUndoStack(int rollNo, char* date, char previousStatus) {
    AttendanceStack* newStack = (AttendanceStack*)malloc(sizeof(AttendanceStack));
    newStack->rollNo = rollNo;
    strcpy(newStack->date, date);
    newStack->previousStatus = previousStatus;
    newStack->next = undoStack;
    undoStack = newStack;
}

int popFromUndoStack(int* rollNo, char* date, char* previousStatus) {
    if(undoStack == NULL) {
        return 0;
    }
    
    AttendanceStack* temp = undoStack;
    *rollNo = temp->rollNo;
    strcpy(date, temp->date);
    *previousStatus = temp->previousStatus;
    undoStack = undoStack->next;
    free(temp);
    return 1;
}

// ==================== STUDENT MANAGEMENT FUNCTIONS ====================

// Create a new activity node
Activity* createActivity(char* activityName) {
    Activity* newActivity = (Activity*)malloc(sizeof(Activity));
    if(newActivity == NULL) {
        printf("Memory allocation failed for activity!\n");
        return NULL;
    }
    strcpy(newActivity->activity, activityName);
    newActivity->next = NULL;
    return newActivity;
}

// Add activity to student's extracurricular list
void addActivityToStudent(Student* student, char* activityName) {
    Activity* newActivity = createActivity(activityName);
    if(newActivity == NULL) return;
    
    if(student->extracurricular == NULL) {
        student->extracurricular = newActivity;
    } else {
        Activity* current = student->extracurricular;
        while(current->next != NULL) {
            current = current->next;
        }
        current->next = newActivity;
    }
}

// Display student's activities
void displayStudentActivities(Student* student) {
    if(student->extracurricular == NULL) {
        printf("  No extracurricular activities\n");
        return;
    }
    
    Activity* current = student->extracurricular;
    int count = 1;
    while(current != NULL) {
        printf("  %d. %s\n", count++, current->activity);
        current = current->next;
    }
}

// Add student to linked list
void addStudent() {
    Student* newStudent = (Student*)malloc(sizeof(Student));
    
    if(newStudent == NULL) {
        printf("Memory allocation failed!\n");
        return;
    }
    
    // Initialize all fields
    newStudent->extracurricular = NULL;
    newStudent->attendanceRecords = NULL;
    newStudent->totalPresent = 0;
    newStudent->totalAbsent = 0;
    newStudent->totalLate = 0;
    newStudent->totalHalfDay = 0;
    newStudent->attendancePercentage = 0.0;
    
    printf("\n=== ADD NEW STUDENT ===\n");
    printf("Enter Roll No: ");
    scanf("%d", &newStudent->rollNo);
    
    // Check if roll number already exists
    if(searchStudent(newStudent->rollNo) != NULL) {
        printf("Student with Roll No %d already exists!\n", newStudent->rollNo);
        free(newStudent);
        return;
    }
    
    printf("Enter Name: ");
    getchar();
    fgets(newStudent->name, 50, stdin);
    newStudent->name[strcspn(newStudent->name, "\n")] = 0;
    
    printf("Enter Semester: ");
    scanf("%d", &newStudent->semester);
    
    printf("Enter marks for 5 subjects:\n");
    for(int i = 0; i < 5; i++) {
        printf("Subject %d: ", i+1);
        scanf("%f", &newStudent->marks[i]);
        
        if(newStudent->marks[i] < 0 || newStudent->marks[i] > 100) {
            printf("Invalid marks! Marks should be between 0 and 100.\n");
            i--;
        }
    }
    
    // Add extracurricular activities
    int activityCount;
    printf("How many extracurricular activities? ");
    scanf("%d", &activityCount);
    getchar();
    
    for(int i = 0; i < activityCount; i++) {
        char activityName[50];
        printf("Enter activity %d: ", i+1);
        fgets(activityName, 50, stdin);
        activityName[strcspn(activityName, "\n")] = 0;
        addActivityToStudent(newStudent, activityName);
    }
    
    printf("Enter Sports Activities: ");
    fgets(newStudent->sports, 100, stdin);
    newStudent->sports[strcspn(newStudent->sports, "\n")] = 0;
    
    // Add to linked list
    newStudent->next = studentList;
    studentList = newStudent;
    
    printf("Student added successfully!\n");
}

// Display all students
void displayAllStudents() {
    if(studentList == NULL) {
        printf("No students found!\n");
        return;
    }
    
    printf("\n=== ALL STUDENTS ===\n");
    Student* current = studentList;
    int count = 1;
    
    while(current != NULL) {
        printf("\n--- Student %d ---\n", count++);
        displayStudent(current);
        current = current->next;
    }
}

// Display student details
void displayStudent(Student* student) {
    printf("Roll No: %d\n", student->rollNo);
    printf("Name: %s\n", student->name);
    printf("Semester: %d\n", student->semester);
    printf("Marks: ");
    for(int i = 0; i < 5; i++) {
        printf("%.2f ", student->marks[i]);
    }
    printf("\nAverage: %.2f\n", calculateAverage(student));
    printf("Attendance: %.2f%% (P:%d A:%d L:%d H:%d)\n", 
           student->attendancePercentage, student->totalPresent,
           student->totalAbsent, student->totalLate, student->totalHalfDay);
    printf("Extracurricular Activities:\n");
    displayStudentActivities(student);
    printf("Sports: %s\n", student->sports);
}

// Add extracurricular activity to existing student
void addExtracurricularActivity() {
    if(studentList == NULL) {
        printf("No students found!\n");
        return;
    }
    
    int rollNo;
    printf("Enter student roll number: ");
    scanf("%d", &rollNo);
    
    Student* student = searchStudent(rollNo);
    if(student == NULL) {
        printf("Student with roll number %d not found!\n", rollNo);
        return;
    }
    
    printf("Adding extracurricular activity for %s (Roll No: %d)\n", student->name, student->rollNo);
    
    char activityName[50];
    printf("Enter new activity: ");
    getchar();
    fgets(activityName, 50, stdin);
    activityName[strcspn(activityName, "\n")] = 0;
    
    addActivityToStudent(student, activityName);
    printf("Activity added successfully!\n");
}

// Display extracurricular activities
void displayExtracurricular() {
    if(studentList == NULL) {
        printf("No students found!\n");
        return;
    }
    
    printf("\n=== EXTRACURRICULAR ACTIVITIES ===\n");
    Student* current = studentList;
    int found = 0;
    
    while(current != NULL) {
        if(current->extracurricular != NULL) {
            printf("\nRoll No: %d, Name: %s\n", current->rollNo, current->name);
            printf("Activities:\n");
            displayStudentActivities(current);
            found = 1;
        }
        current = current->next;
    }
    
    if(!found) {
        printf("No extracurricular activities found!\n");
    }
}

// Display sports activities
void displaySports() {
    if(studentList == NULL) {
        printf("No students found!\n");
        return;
    }
    
    printf("\n=== SPORTS ACTIVITIES ===\n");
    Student* current = studentList;
    int found = 0;
    
    while(current != NULL) {
        if(strlen(current->sports) > 0 && 
           strcasecmp(current->sports, "none") != 0 && 
           strcasecmp(current->sports, "no") != 0) {
            printf("Roll No: %d, Name: %s\n", current->rollNo, current->name);
            printf("Sports: %s\n\n", current->sports);
            found = 1;
        }
        current = current->next;
    }
    
    if(!found) {
        printf("No sports activities found!\n");
    }
}

// ==================== MARKS ANALYSIS FUNCTIONS ====================

// Display lowest marks for a student
void displayLowestMarks(Student* student) {
    if(student == NULL) return;
    
    float min1 = 100, min2 = 100;
    int subject1 = -1, subject2 = -1;
    
    // Find two lowest marks
    for(int i = 0; i < 5; i++) {
        if(student->marks[i] < min1) {
            min2 = min1;
            subject2 = subject1;
            min1 = student->marks[i];
            subject1 = i;
        } else if(student->marks[i] < min2) {
            min2 = student->marks[i];
            subject2 = i;
        }
    }
    
    printf("Lowest marks for %s (Roll No: %d):\n", student->name, student->rollNo);
    printf("1. Subject %d: %.2f\n", subject1 + 1, min1);
    printf("2. Subject %d: %.2f\n", subject2 + 1, min2);
    printf("Average: %.2f\n", calculateAverage(student));
}

// Find lowest marks for individual student
void findLowestMarksIndividual() {
    if(studentList == NULL) {
        printf("No students found!\n");
        return;
    }
    
    int rollNo;
    printf("Enter student roll number: ");
    scanf("%d", &rollNo);
    
    Student* student = searchStudent(rollNo);
    if(student == NULL) {
        printf("Student with roll number %d not found!\n", rollNo);
        return;
    }
    
    printf("\n=== LOWEST MARKS ANALYSIS ===\n");
    displayLowestMarks(student);
}

// Find lowest marks for all students
void findLowestMarksAll() {
    if(studentList == NULL) {
        printf("No students found!\n");
        return;
    }
    
    printf("\n=== LOWEST MARKS - ALL STUDENTS ===\n");
    Student* current = studentList;
    
    while(current != NULL) {
        displayLowestMarks(current);
        printf("\n");
        current = current->next;
    }
}

// Compare individual student's performance
void compareIndividualStudent(int rollNo) {
    Student* student = searchStudent(rollNo);
    if(student == NULL) {
        printf("Student not found!\n");
        return;
    }
    
    printf("\n=== PERFORMANCE ANALYSIS FOR %s ===\n", student->name);
    printf("Roll No: %d\n", student->rollNo);
    printf("Semester: %d\n", student->semester);
    
    float avg = calculateAverage(student);
    printf("Current Semester Average: %.2f\n", avg);
    
    // Calculate subject-wise performance
    printf("\nSubject-wise Performance:\n");
    for(int i = 0; i < 5; i++) {
        printf("Subject %d: %.2f - ", i+1, student->marks[i]);
        if(student->marks[i] >= 85) {
            printf("Excellent\n");
        } else if(student->marks[i] >= 70) {
            printf("Good\n");
        } else if(student->marks[i] >= 50) {
            printf("Average\n");
        } else {
            printf("Needs Improvement\n");
        }
    }
    
    // Overall performance
    printf("\nOverall Performance: ");
    if(avg >= 80) {
        printf("Outstanding\n");
    } else if(avg >= 65) {
        printf("Good\n");
    } else if(avg >= 50) {
        printf("Satisfactory\n");
    } else if(avg >= 40) {
        printf("Pass\n");
    } else {
        printf("Fail - Needs Immediate Attention\n");
    }
    
    // Display activities and attendance
    printf("\nExtracurricular Activities:\n");
    displayStudentActivities(student);
    printf("Sports: %s\n", student->sports);
    printf("Attendance: %.2f%%\n", student->attendancePercentage);
}

// Compare semesters for individual student
void compareStudentSemesters() {
    if(studentList == NULL) {
        printf("No students found!\n");
        return;
    }
    
    int rollNo;
    printf("Enter student roll number to analyze: ");
    scanf("%d", &rollNo);
    
    compareIndividualStudent(rollNo);
}

// ==================== QUEUE OPERATIONS ====================

// Process students using queue
void processQueueStudents() {
    if(studentList == NULL) {
        printf("No students found!\n");
        return;
    }
    
    // Enqueue all students
    Student* current = studentList;
    while(current != NULL) {
        enqueue(current);
        current = current->next;
    }
    
    printf("\n=== PROCESSING STUDENTS (QUEUE DEMO) ===\n");
    printf("Processing order (FIFO):\n");
    
    Student* student;
    int count = 1;
    while((student = dequeue()) != NULL) {
        printf("\n--- Processing Student %d ---\n", count++);
        displayStudent(student);
        
        // Additional processing
        float avg = calculateAverage(student);
        printf("Status: ");
        if(avg >= 75) {
            printf("Excellent\n");
        } else if(avg >= 60) {
            printf("Good\n");
        } else if(avg >= 40) {
            printf("Average\n");
        } else {
            printf("Needs Improvement\n");
        }
        
        // Attendance status
        printf("Attendance Status: ");
        if(student->attendancePercentage >= 90) {
            printf("Excellent Attendance\n");
        } else if(student->attendancePercentage >= 75) {
            printf("Good Attendance\n");
        } else if(student->attendancePercentage >= 60) {
            printf("Average Attendance\n");
        } else {
            printf("Poor Attendance - Needs Attention\n");
        }
    }
}

void enqueue(Student* student) {
    Queue* newQueue = (Queue*)malloc(sizeof(Queue));
    newQueue->student = student;
    newQueue->next = NULL;
    
    if(processQueue == NULL) {
        processQueue = newQueue;
    } else {
        Queue* current = processQueue;
        while(current->next != NULL) {
            current = current->next;
        }
        current->next = newQueue;
    }
}

Student* dequeue() {
    if(processQueue == NULL) {
        return NULL;
    }
    
    Queue* temp = processQueue;
    Student* student = temp->student;
    processQueue = processQueue->next;
    free(temp);
    return student;
}

// ==================== UTILITY FUNCTIONS ====================

// Calculate average marks
float calculateAverage(Student* student) {
    float total = 0;
    for(int i = 0; i < 5; i++) {
        total += student->marks[i];
    }
    return total / 5;
}

// Search student by roll number
Student* searchStudent(int rollNo) {
    Student* current = studentList;
    while(current != NULL) {
        if(current->rollNo == rollNo) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Validate date format (YYYY-MM-DD)
int validateDate(char* date) {
    if(strlen(date) != 10) return 0;
    if(date[4] != '-' || date[7] != '-') return 0;
    
    for(int i = 0; i < 10; i++) {
        if(i == 4 || i == 7) continue;
        if(!isdigit(date[i])) return 0;
    }
    
    return 1;
}

// Free allocated memory
void freeMemory() {
    // Free student list
    Student* currentStudent = studentList;
    while(currentStudent != NULL) {
        Student* nextStudent = currentStudent->next;
        
        // Free extracurricular activities
        Activity* currentActivity = currentStudent->extracurricular;
        while(currentActivity != NULL) {
            Activity* nextActivity = currentActivity->next;
            free(currentActivity);
            currentActivity = nextActivity;
        }
        
        // Free attendance records
        Attendance* currentAtt = currentStudent->attendanceRecords;
        while(currentAtt != NULL) {
            Attendance* nextAtt = currentAtt->next;
            free(currentAtt);
            currentAtt = nextAtt;
        }
        
        free(currentStudent);
        currentStudent = nextStudent;
    }
    
    // Free undo stack
    AttendanceStack* currentStack = undoStack;
    while(currentStack != NULL) {
        AttendanceStack* nextStack = currentStack->next;
        free(currentStack);
        currentStack = nextStack;
    }
    
    // Free queue
    Student* student;
    while((student = dequeue()) != NULL);
}