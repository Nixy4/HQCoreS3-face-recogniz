#pragma once 

#define FR_USER_MAX 10

typedef struct
{
  char name[20];
  char gender[64];
  int age;
  char employee_id[20];
  char position[20];
  int face_id;
} fr_user_t;

#ifdef __cplusplus
extern "C"
{
#endif

void fr_user_init();
void fr_user_fmt();
void fr_user_load();
void fr_user_save();
void fr_user_print();

void fr_user_set(int index, const fr_user_t* data);
int fr_user_register(const fr_user_t* data);
void fr_user_modify(int index, const fr_user_t* data);
void fr_user_delete(int index);
int fr_user_get_cnt();


const char* fr_user_get_name(int index);
const char* fr_user_get_gender(int index);
int fr_user_get_age(int index);
const char* fr_user_get_employee_id(int index);
const char* fr_user_get_position(int index);
int fr_user_get_face_id(int index);
fr_user_t* fr_user_get_ptr(int index);

#ifdef __cplusplus
}
#endif