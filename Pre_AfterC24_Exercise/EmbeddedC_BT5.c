#include <stdio.h>
#include <string.h>

typedef unsigned char uint8_t;

struct infor {
    char name[33];  
    char mssv[9];   
    uint8_t course_c;
};

void infor_input(struct infor* stu_infor) {
    printf("Enter name: ");
    fgets(stu_infor->name, sizeof(stu_infor->name), stdin);
    stu_infor->name[strcspn(stu_infor->name, "\n")] = '\0';  
    
    printf("Enter ID student: ");
    fgets(stu_infor->mssv, sizeof(stu_infor->mssv), stdin);
    stu_infor->mssv[strcspn(stu_infor->mssv, "\n")] = '\0';  
    
    printf("Enter ID subject : ");
    scanf("%hhu", &stu_infor->course_c);
}

void infor_print(struct infor* stu_infor) {
    printf("Information of student:\n");
    printf("Name: %s\n", stu_infor->name);
    printf("ID student: %s\n", stu_infor->mssv);
    printf("ID subject: %d\n", stu_infor->course_c);
}

int main() {
    struct infor student_info;
    infor_input(&student_info);
    infor_print(&student_info);
    return 0;
}
