#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "../services/curl_handle_init.h"
#include "../services/create_new_label.h"
#include "../services/delete_label.h"
#include "../services/fetch_all_labels.h"
#include "../services/fetch_email_subject_by_id.h"
#include "../services/fetch_all_emails_by_label.h"
#include "../services/create_new_mail.h"
#include "../services/add_label_to_email.h"
#include "../services/fetch_msgid_by_query.h"
#include "../services/fetch_email_content_by_id.h"
#include "../services/delete_email_by_id_and_folder.h"
#include "../services/relabel_an_email.h"


int main(){
    CURL *curl;
		mailbox_config config={
			.email="CHANGE",
			.password="CHANGE",
			.ip_address="imaps://imap.gmail.com",
			.port="993"
		};
    curl_handle_init(&curl, &config);
    if(!curl){
        printf("Unable to get a curl handle.. terminating..\n");
        return 1;
    }
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 100L);

    // ** create new label
    // create_new_label(curl, "/bar");


    // ** delete label
    // delete_label(curl, "/hello");


		// ** fetch all labels
		// char** labels_ptr=NULL;
		// int number_of_labels=0;
		// fetch_all_labels(curl, &labels_ptr, &number_of_labels, "");
		// for(int i=0;i<number_of_labels;i++){
		// 	printf("Lab: %s\n", labels_ptr[i]);
		// }


		// ** fetch email subject by id
		// char* subject=fetch_email_subject_by_id(curl, 4);
		// printf("Subject: %s\n", subject);


		// ** fetch all emails by label
		// char* email_subjects[MAX_FILES_IN_A_DIR];
		// int number_of_emails=0;
		// fetch_all_emails_by_label(curl, "/", email_subjects, &number_of_emails);

		// for(int i=0;i<number_of_emails;i++){
		// 	printf("%s\n", email_subjects[i]);
		// }


		// ** create new email
		// create_new_mail(curl, "/", "health is wealth");


		// ** add labels to email
		// add_label_to_email(curl, 1, "/haha");


		// ** relabel an email
		// relabel_an_email(curl, "/yubn", "/haha", 1);


		// ** delete and email
		// relabel_an_email(curl, "/DELETED", "/", 3);
		// delete_email_by_id_and_folder(curl, 2, "/foo");


		// ** INVALID: fetch_msg_id_by_query(curl, "helloabc", "abc");


		// ** fetch msg content
		// int msg_id=fetch_msgid_by_subject_and_label(curl, "new_mail", "/haha");
		// printf("Msg Id: %d\n", msg_id);
		// assert(msg_id!=-1);
		// char* response= fetch_email_content_by_id(curl, msg_id);
		// printf("Content: [%s]\n\n", response);
		// free(response);


		// ** delete msg
		// int msg_id=fetch_msgid_by_subject_and_label(curl, "bar", "/haha");
		// printf("Msg Id: %d\n", msg_id);
		// assert(msg_id!=-1);
		// delete_email_by_id_and_folder(curl, msg_id, "/haha");


		// ** update msg content: See direct implementation


}
