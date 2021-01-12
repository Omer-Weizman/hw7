#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grades.h"
#include "linked-list.h"

#define TRUE 1
#define FALSE 0

typedef struct list list_ADT;
typedef struct node node_ADT;

typedef struct course {
    char* course_name;
    int course_grade;
} course;

typedef struct student {
    int student_id;
    char *student_name;
    list_ADT *courses;
} student;

struct grades {
    list_ADT *students;
};

/* User functions for the list */

/* brief copy a single student element */
int student_copy(void *element, void **output) {
    if(element == NULL) {
        return -1;
    }
    student *stud_ptr = (student*)element;
    student *new_stud = init_student(stud_ptr->student_id,
                                    stud_ptr->student_name);
    *output = (void*)new_stud;
    return 0;
}

/* brief destroy a single student element */
void student_destroy(void *element) {
    if(element == NULL) {
        return;
    }
    student *stud_ptr = (student*)element;
    list_destroy(stud_ptr->courses);
    free(stud_ptr->student_name);
    free(stud_ptr);
}

/* brief copy a single course element */
int course_copy(void *element, void **output) {
    if(element == NULL) {
        return -1;
    }
    course *course_ptr = (course*)element;
    course *new_course = init_course(course_ptr->course_name,
                                    course_ptr->course_grade);
    *output = (void*)new_course;
    return 0;
}

/* brief destroy a single course element */
void course_destroy(void *element) {
    if(element == NULL) {
        return;
    }
    course *course_ptr = (course*)element;
        free(course_ptr->course_name);
        free(course_ptr);
}

/* brief initillize a single student element */
student* init_student(int id, const char *name) {
    student *new_student = (student*)malloc(sizeof(student));
    if (new_student == NULL) {
        return NULL;
    }
    new_student->student_id = id;
    new_student->student_name = (char*)malloc(strlen(name) + 1);
    if (new_student->student_name == NULL) {
        free(new_student);
        return NULL;
    }
    strcpy(new_student->student_name, name);
    new_student->courses = list_init(course_copy, course_destroy);
    if (new_student->courses == NULL) {
        free(new_student->student_name);
        free(new_student);
        return NULL;
    }
    return new_student;
}

/* brief initillize a single course element */
course* init_course(const char *name, int grade) {
    course *new_course = (course*)malloc(sizeof(course));
    if (new_course == NULL) {
        return NULL;
    }
    new_course->course_grade = grade;
    new_course->course_name = malloc (strlen(name)+1);
    if (new_course->course_name == NULL) {
        free(new_course);
        return NULL;
    }
    strcpy(new_course->course_name, name);
    return new_course;
}

/**
 * @brief Initializes the "grades" data-structure.
 * @returns A pointer to the data-structure, of NULL in case of an error
 */
struct grades* grades_init() {
	struct grades *new_grades = malloc(sizeof(struct grades));
	if (new_grades == NULL) {
		return NULL;
	}
    new_grades->students = list_init(student_copy, student_destroy);
    if (new_grades->students == NULL) {
        return NULL;
    }
	return new_grades;
}

/**
 * @brief Destroys "grades", de-allocate all memory!
 */
void grades_destroy(struct grades *grades) {
    if (grades == NULL) {
        return;
    }
    list_destroy(grades->students);
    free(grades);
}

/**
 * @brief Adds a student with "name" and "id" to "grades"
 * @returns 0 on success
 * @note Failes if "grades" is invalid, or a student with
 * the same "id" already exists in "grades"
 */
int grades_add_student(struct grades *grades, const char *name, int id) {
    if(grades == NULL || name == NULL) {
        return -1;
    }
    /* Loop on grades->students */
    node_ADT *node_ptr = list_begin(grades->students);

    student *stud_ptr = NULL;
    while (node_ptr != NULL) {
        stud_ptr  = (student*)list_get(node_ptr);
        if (stud_ptr->student_id == id) {
            return -1;
        }
        node_ptr = list_next(node_ptr);
    }
    /* Add new student */
    char tmp_name[strlen(name) + 1];
    strcpy(tmp_name, name);
    student new_student;
    new_student.student_name = tmp_name;
    new_student.student_id = id;
    new_student.courses = list_init(course_copy, course_destroy);
    if (list_push_back(grades->students, (void*)&new_student) != 0) {
        list_destroy(new_student.courses);
        return -1;
    }
    list_destroy(new_student.courses);
    return 0;
}

/**
 * @brief Adds a course with "name" and "grade" to the student with "id"
 * @return 0 on success
 * @note Failes if "grades" is invalid, if a student with "id" does not exist
 * in "grades", if the student already has a course with "name", or if "grade"
 * is not between 0 to 100.
 */
int grades_add_grade(struct grades *grades,
                     const char *name,
                     int id,
                     int grade) {
    if (grades == NULL) {
        return -1;
    }
    if (grade < 0 || grade > 100) {
        return -1;
    }
    bool is_found = FALSE;
    /* Loop on grades->students->courses */
    node_ADT *node_ptr = list_begin(grades->students);

    student *stud_ptr = NULL;
    while (node_ptr != NULL) {
        stud_ptr  = (student*)list_get(node_ptr);
        if (stud_ptr->student_id == id) {
            is_found = TRUE;
            break;
        }
        node_ptr = list_next(node_ptr);
    }
    if (!is_found){
        return -1;
    }
    node_ADT *node_course_ptr = list_begin(stud_ptr->courses);
    course *course_ptr = NULL;
    while (node_course_ptr != NULL) {
        course_ptr = list_get(node_course_ptr);
        if (strcmp(course_ptr->course_name, name) == 0) {
            return -1;
        }
        node_course_ptr = list_next(node_course_ptr);
    }
    /* Add new course */
    char tmp_name[strlen(name) + 1];
    strcpy(tmp_name, name);
    course new_course;
    new_course.course_name = tmp_name;
    new_course.course_grade = grade;
    
    if (list_push_back(stud_ptr->courses, (void*)&new_course) != 0) {
        return -1;
    }
    return 0;
}
    
/**
 * @brief Calcs the average of the student with "id" in "grades".
 * @param[out] out This method sets the variable pointed by "out" to the
 * student's name. Needs to allocate memory. The user is responsible for
 * freeing the memory.
 * @returns The average, or -1 on error
 * @note Fails if "grades" is invalid, or if a student with "id" does not exist
 * in "grades".
 * @note If the student has no courses, the average is 0.
 * @note On error, sets "out" to NULL.
 */
float grades_calc_avg(struct grades *grades, int id, char **out) {
    if (grades == NULL) {
        return -1;
    }
    bool is_found = FALSE;
    /* Loop on grades->students */
    node_ADT *node_ptr = list_begin(grades->students);
    
    student *stud_ptr = NULL;
    while (node_ptr != NULL) {
        stud_ptr  = (student*)list_get(node_ptr);
        if (stud_ptr->student_id == id) {
            is_found = TRUE;
            break;
        }
        node_ptr = list_next(node_ptr);
    }
    if(!is_found) {
        return -1;
    }
    char *student_name_tmp = malloc(strlen(stud_ptr->student_name) + 1);
    strcpy(student_name_tmp, stud_ptr->student_name);
    *out = student_name_tmp;
    float sum = 0.0;
    int count = 0;
    node_ADT *node_course_ptr = list_begin(stud_ptr->courses);
    course *course_ptr = NULL;
    while (node_course_ptr != NULL) {
        course_ptr = list_get(node_course_ptr);
        count++;
        sum += course_ptr->course_grade;
        node_course_ptr = list_next(node_course_ptr);
    }
    float avg = (count == 0) ? 0.0 : sum / count;
    return avg;
}

/**
 * @brief Prints the courses of the student with "id" in the following format:
 * STUDENT-NAME STUDENT-ID: COURSE-1-NAME COURSE-1-GRADE, [...]
 * @returns 0 on success
 * @note Fails if "grades" is invalid, or if a student with "id" does not exist
 * in "grades".
 * @note The courses should be printed according to the order
 * in which they were inserted into "grades"
 */
int grades_print_student(struct grades *grades, int id) {
    if (grades == NULL) {
        return -1;
    }
    bool is_found = FALSE;
    /* Loop on grades->students */
    node_ADT *node_ptr = list_begin(grades->students);
    student *stud_ptr = NULL;
    while (node_ptr != NULL) {
        stud_ptr  = (student*)list_get(node_ptr);
        if (stud_ptr->student_id == id) { ////////////
            is_found = TRUE;
            break;
        }
        node_ptr = list_next(node_ptr);
    }
    if(!is_found) {
        return -1;
    }
    printf("%s %d:", stud_ptr->student_name, stud_ptr->student_id);
    node_ADT *node_ptr_course = list_begin(stud_ptr->courses);
    
    /* Loop on student->courses */
    while (node_ptr_course != NULL) {
        course *course_ptr = (course*)list_get(node_ptr_course);
        printf(" %s %d", course_ptr->course_name, course_ptr->course_grade);
        node_ptr_course = list_next(node_ptr_course);
        if(node_ptr_course != NULL) {
            printf(",");
        }
    }
    printf("\n");
    return 0;
}

/**
 * @brief Prints all students in "grade", in the following format:
 * STUDENT-1-NAME STUDENT-1-ID: COURSE-1-NAME COURSE-1-GRADE, [...]
 * STUDENT-2-NAME STUDENT-2-ID: COURSE-1-NAME COURSE-1-GRADE, [...]
 * @returns 0 on success
 * @note Fails if "grades" is invalid
 * @note The students should be printed according to the order
 * in which they were inserted into "grades"
 * @note The courses should be printed according to the order
 * in which they were inserted into "grades"
 */
int grades_print_all(struct grades *grades) {
    if (grades == NULL) {
        return -1;
    }
    /* Loop on grades->students */
    node_ADT *node_ptr = list_begin(grades->students);
    student *stud_ptr = NULL;
    while (node_ptr != NULL) {
        stud_ptr  = (student*)list_get(node_ptr);
        printf("%s %d:", stud_ptr->student_name, stud_ptr->student_id);
        node_ADT *node_ptr_course = list_begin(stud_ptr->courses);

        /* Loop on student->courses */
        while (node_ptr_course != NULL) {
            course *course_ptr = (course*)list_get(node_ptr_course);
            printf(" %s %d", course_ptr->course_name, course_ptr->course_grade);
            node_ptr_course = list_next(node_ptr_course);
            if(node_ptr_course != NULL) {
                printf(",");
            }
        }
        printf("\n");
        node_ptr = list_next(node_ptr);
    }
    return 0;
}