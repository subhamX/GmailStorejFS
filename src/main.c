#define FUSE_USE_VERSION 31


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fuse.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <assert.h>
#include <curl/curl.h>
#include <unistd.h>
#include "mail/mail_client.h"
#include "utils/definitions.h"
#include "services/curl_handle_init.h"
#include "services/fetch_all_labels.h"
#include "services/fetch_all_emails_by_label.h"
#include "services/fetch_email_content_by_id.h"
#include "services/fetch_object_type.h"
#include "utils/string_helpers.h"
#include "services/create_new_label.h"
#include "services/delete_label.h"
#include "services/relabel_an_email.h"
#include "services/delete_email_by_id_and_folder.h"
#include "services/create_new_mail.h"

// if I am at "x" then I shall keep track of all

// TODO: I have data when I call readdir; now how to pass it to getattr

#define PVT_DATA ((private_data_node *) fuse_get_context()->private_data)

#define OPTION(t, p)                           \
    { t, offsetof(mailbox_config, p), 1 }
static const struct fuse_opt option_spec[] = {
	OPTION("--hostname=%s", ip_address),
	OPTION("--port=%s", port),
	OPTION("--email=%s", email),
	OPTION("--password=%s", password),
	FUSE_OPT_END
};

void show_help(){
	printf("Usage: [Mount Point] [IP Address] [PORT] [Email] [Password]\n");
}

// variable to store the mailbox config
static mailbox_config* config=NULL;

static void * mail_fs_init(struct fuse_conn_info *conn, struct fuse_config *cfg){
    // connect to the mail client with the credentials
    // and return the pointer

    // initialize the logging ptr which has an open file descripter
    // every ops will be logged there
    // ! Ensure that the log file isn't in the mount folder
    (void) conn;
    // cfg->kernel_cache = 1;
		// check
		// cfg->attr_timeout=100000;

		private_data_node* private_data=(private_data_node*)malloc(sizeof(private_data_node));
		private_data->config=config;
		private_data->level=0;

    CURL *curl;
    curl_handle_init(&curl, private_data->config);
		char* tmp;
		curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL,&tmp);
		private_data->base_full_url=strdup(tmp);
		private_data->curl=curl;
    // TODO: destory everything in mail_fs_destroy
    return private_data;
}


static int mail_fs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags){
	(void) offset;
	(void) fi;
	(void) flags;

	printf("\033[1;33m");
	printf("Debug: log mail_fs_readdir: %s\n", path);
	printf("\033[0m");

	private_data_node* data=PVT_DATA;
	char* labels_ptr[MAX_DIRS_IN_A_DIR];
	int number_of_labels=0;

	if (strcmp(path, "/")==0){
		// lookup for both dirs and files
		fetch_all_labels(data->curl, labels_ptr, &number_of_labels, data->base_full_url);
	}

	// fetch all emails
	char* email_subjects[MAX_FILES_IN_A_DIR];
	int number_of_emails=0;
	fetch_all_emails_by_label(data->curl, path, email_subjects, &number_of_emails);

	printf("Number of labels: %d\n", number_of_labels);
	for(int i=0;i<number_of_labels;i++){
		filler(buf, labels_ptr[i], NULL, 0, 0);
	}

	printf("Number of emails: %d\n", number_of_emails);
	for(int i=0;i<number_of_emails;i++){
		filler(buf, email_subjects[i], NULL, 0, 0);
	}

	filler(buf, ".", NULL, 0, 0);
	filler(buf, "..", NULL, 0, 0);


	return 0;
}


static int mail_fs_open(const char *path, struct fuse_file_info *fi){
	// TODO: shall I check ?
	printf("\033[0;35m");
	printf("Debug: log mail_fs_open: %s\n", path);
	printf("\033[0m");
	// if ((fi->flags & O_ACCMODE) != O_RDONLY)
	// 	return -EACCES;
	return 0;
}


static int mail_fs_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi){
	(void) fi;
	// TODO: remove it later. keep it active for now
	if(0==strcmp(path, "/.git") || 0==strcmp(path, "/.rbenv-version") || 0==strcmp(path, "/Gemfile") || 0==strcmp(path, "/.rvm") || 0==strcmp(path, "/__rvm_cleanup_download") || 0==strcmp(path, "/HEAD") || 0==strcmp(path, "/.git") || 0==strcmp(path, "/.git") || 0==strcmp(path, "/.rvmrc") || 0==strcmp(path, "/.versions.conf") || 0==strcmp(path, "/.ruby-version") || 0==strcmp(path, "/.rbfu-version")){
		return -ENOENT;
	}
	memset(stbuf, 0, sizeof(struct stat));

	stbuf->st_uid = getuid(); // The owner of the file/directory is the user who mounted the filesystem
	stbuf->st_gid = getgid(); // The group of the file/directory is the same as the group of the user who mounted the filesystem
	stbuf->st_atime = time( NULL ); // The last "a"ccess of the file/directory is right now
	stbuf->st_mtime = time( NULL ); // The last "m"odification of the file/directory is right now

	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0777;
		stbuf->st_nlink = 2;
		return 0;
	}
	printf("\033[1;31m");
	printf("Debug: log mail_fs_getattr: %s\n", path);
	printf("\033[0m");
	private_data_node* data=PVT_DATA;

	int res = 0;

	int object_type=fetch_object_type(data->curl,path,data->base_full_url);
	printf("Debug: FETCH_OBJECT_TYPE: %d\n\n", object_type);

	if(object_type==1){
		stbuf->st_mode = S_IFDIR | 0777;
		stbuf->st_nlink = 2;
	}else if(object_type==2){
		stbuf->st_mode = S_IFREG | 0777;
		stbuf->st_nlink = 1;
		stbuf->st_size = 1000;
	}else{
		res=-ENOENT;
	}

	return res;
}


static int mail_fs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi){
	size_t len;
	(void) fi;
	printf("\033[1;31m");
	printf("Debug: log mail_fs_read: %s\n", path);
	printf("\033[0m");

	// getattr will ensure that here we reach only if there exist a file
	char root_dirname[10000];
	char objectname[10000];
	split_path_to_components(root_dirname, objectname, path);
	printf("Debug: root_dirname: %s\n", root_dirname);
	printf("Debug: objectname: %s\n", objectname);

	private_data_node* data=PVT_DATA;
	int msg_id=fetch_msgid_by_subject_and_label(data->curl, objectname, root_dirname);
	printf("Msg Id: %d\n", msg_id);
	if(msg_id==-1){
		// msg doesn't exist
		return -ENOENT;
	}else{
		char* response= fetch_email_content_by_id(data->curl, msg_id);
		len=strlen(response);
		if (offset < len) {
			if (offset + size > len) size = len - offset;
			memcpy(buf, response + offset, size);
		} else{
			size = 0;
		}
	}
	return size;
}


// static int mail_fs_destroy(){

//     // TODO: Always cleanup
//     // In docs it's mentioned to "re-using handles is a key to good performance with libcurl"
//     // so we shall clean it in the end
//     // curl_easy_cleanup(curl);
// }
static int mail_fs_write(const char * path, const char *buf, size_t size, off_t off, struct fuse_file_info *fi){

	printf("PATH: %s\n", path);
	printf("NEW CONTENT: %s\n", buf);
	private_data_node* data=PVT_DATA;

	// save the content to server
	// delete old
	char root_dirname[10000];
	char objectname[10000];
	split_path_to_components(root_dirname, objectname, path);
	printf("Debug: root_dirname: %s\n", root_dirname);
	printf("Debug: objectname: %s\n", objectname);
	int msg_id=fetch_msgid_by_subject_and_label(data->curl, objectname, root_dirname);
	printf("Msg Id: %d\n", msg_id);
	if(msg_id==-1){
		printf("unusual\n");
	}else{
		assert(msg_id!=-1);
		delete_email_by_id_and_folder(data->curl, msg_id, root_dirname);
	}
	// create new
	int res=create_new_mail(data->curl, root_dirname, objectname, buf);

	return size;
}


// create a new folder
static int mail_fs_mkdir(const char * path, mode_t mode){
	printf("\033[1;31m");
	printf("Debug: log mail_fs_mkdir: %s\n", path);
	printf("\033[0m");

	char root_dirname[10000];
	char objectname[10000];
	split_path_to_components(root_dirname, objectname, path);
	if(strcmp(root_dirname,"/")==0){
		private_data_node* data=PVT_DATA;
		// allowed to create directory
		int res=create_new_label(data->curl, path);
		if(res){
			printf("Debug: Failed to create\n");
			return -ENONET;
		}
	}else{
		// only 1 level is allowed
		return -EACCES;
	}

	return 0;
}

static int mail_fs_mknod(const char *path, mode_t mode, dev_t rdev){
	// create new file
	private_data_node* data=PVT_DATA;
	char root_dirname[10000];
	char objectname[10000];
	split_path_to_components(root_dirname, objectname, path);
	create_new_mail(data->curl, root_dirname, objectname, "");
	return 0;
}


static int mail_fs_rmdir(const char * path){
	private_data_node* data=PVT_DATA;
	int res=delete_label(data->curl, path);
	if(res) return -ENONET; // network error
	return 0;
}



static const struct fuse_operations mail_fs_operations = {
    // .destroy 				= mail_fs_destroy
		// .rename					= mail_fs_rename,
    .init           = mail_fs_init,
    .getattr        = mail_fs_getattr,
    .readdir        = mail_fs_readdir,
    .open           = mail_fs_open,
    .read           = mail_fs_read,
		.mkdir					= mail_fs_mkdir,
		.rmdir					= mail_fs_rmdir,
		.write					= mail_fs_write,
		.mknod					= mail_fs_mknod,
};



int main(int argc, char* argv[]){
		// if(argc!=6){
		// 	show_help();
		// 	return 1;
		// }

    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
		config=(mailbox_config*)malloc(sizeof(mailbox_config));

		if (fuse_opt_parse(&args, config, option_spec, NULL) == -1)
			return 1;


		// config->ip_address=strdup(argv[2]);
		// config->port=strdup(argv[3]);
		// config->email=strdup(argv[4]);
		// config->password=strdup(argv[5]);

    int ret;
    ret = fuse_main(args.argc, args.argv, &mail_fs_operations, NULL);
		fuse_opt_free_args(&args);

    // TODO: deallocating memory
		free(config);

	return ret;
}

