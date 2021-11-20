#ifndef CANARY_MAIL_H
#define CANARY_MAIL_H



typedef struct label{

} label;


typedef struct email{
    char* subject;
    char* body;
} email;




// note this fun dynamically creates the email* array
// you should deallocate it
email* get_all_emails(char** current_directory_ptr, int* size){
    // return all emails who have the label as current_directory_ptr[0...size-1]
    return NULL;
}

label* get_all_labels();


// note that we assume that every 
email* get_email_by_subject(){
    return NULL;

}

#endif
