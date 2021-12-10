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
#include "utils/hashmap.h"
#include "services/change_label.h"

#define PVT_DATA ((private_data_node *) fuse_get_context()->private_data) // cmd to get the private data in one shot

#define OPTION(t, p)                           \
    { t, offsetof(mailbox_config, p), 1 }
static const struct fuse_opt option_spec[] = {
	OPTION("--hostname=%s", ip_address),
	OPTION("--port=%s", port),
	OPTION("--email=%s", email),
	OPTION("--password=%s", password),
	FUSE_OPT_END
};

/**
 * @brief prints a small prompt on console on all options and Usage of the script
 */
void show_help(){
	printf("Usage: [Mount Point] [IP Address] [PORT] [Email] [Password]\n");
}

// variable to store the mailbox config
static mailbox_config* config=NULL;


/**
 * @brief method to be executed by fuse while initializing the module
 *
 * @param conn fuse connection info (to be passed by FUSE)
 * @param cfg fuse config (to be passed by FUSE)
 * @return void*
 */
static void * mail_fs_init(struct fuse_conn_info *conn, struct fuse_config *cfg){
		// initialize private data
		private_data_node* private_data=(private_data_node*)malloc(sizeof(private_data_node));
		private_data->config=config;
		private_data->level=0;

		// initialize curl handle and set the credentials, and options
    CURL *curl;
    curl_handle_init(&curl, private_data->config);
		char* tmp;
		curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL,&tmp);
		private_data->base_full_url=strdup(tmp);
		private_data->curl=curl;
    // Note: destory everything in mail_fs_destroy
    return private_data;
}

/**
 * @brief method to use the curl handle to get the labels(folders), and emails(files) on server present at a specific path
 *
 * @param path: current path
 * @return int; 0 for No Error, otherwise -ERRCODE
 */
static int mail_fs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags){
	(void) offset;
	(void) fi;
	(void) flags;

	printf("Debug: log mail_fs_readdir: %s\n", path);

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

/**
 * @brief function to allow opening of files
 *
 * @param path: file path
 * @param fi
 * @return int
 */
static int mail_fs_open(const char *path, struct fuse_file_info *fi){
	printf("Debug: log mail_fs_open: %s\n", path);
	// all files are allowed to be opened
	return 0;
}

/**
 * @brief function to set the attributes of file and folders
 *
 * @return int: 0 if successful, -ERR if error
 */
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

	// for root dir, we don't need to perform a network call.
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0777;
		stbuf->st_nlink = 2;
		return 0;
	}
	printf("Debug: log mail_fs_getattr: %s\n", path);
	private_data_node* data=PVT_DATA;

	int res = 0;
	char root_dirname[10000];
	char objectname[10000];

	split_path_to_components(root_dirname, objectname, path);
	int object_type=fetch_object_type_by_dirname_and_objectname(data->curl,root_dirname,objectname,data->base_full_url);
	printf("Debug: FETCH_OBJECT_TYPE: %d\n\n", object_type);

	if(object_type==1){
		stbuf->st_mode = S_IFDIR | 0777;
		stbuf->st_nlink = 2;
	}else if(object_type==2){
		// Note: for numerals object we cannot make their permissions different
		// as Linux doesn't have a APPEND_ONLY permission
		stbuf->st_mode = S_IFREG | 0777;
		stbuf->st_nlink = 1;
		stbuf->st_size = 1000000;
	}else{
		res=-ENOENT;
	}
	return res;
}


/**
 * @brief function to set the buffer with the contents of the file after fetching from server
 *
 * @return int: 0 if successful, -ERR if error
 */
static int mail_fs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi){
	size_t len;
	(void) fi;
	printf("Debug: log mail_fs_read: %s\n", path);

	// getattr will ensure that here we reach only if there exist a file
	char root_dirname[10000];
	char objectname[10000];
	split_path_to_components(root_dirname, objectname, path);

	private_data_node* data=PVT_DATA;
	int msg_id=fetch_msgid_by_subject_and_label(data->curl, objectname, root_dirname);
	printf("Msg Id: %d\n", msg_id);
	if(msg_id==-1){
		// msg doesn't exist
		return -ENOENT;
	}else{
		// fetch the message content
		char* response= fetch_email_content_by_id(data->curl, msg_id);
		len=strlen(response);
		if (offset < len) {
			if (offset + size > len) size = len - offset;
			memcpy(buf, response + offset, size);
		} else{
			size = 0;
		}
		free(response);
	}
	return size;
}

/**
 * @brief function to deallocate all private data like curl handle etc
 *
 * @param private_data: instance of pvt data to be passed by FUSE
 * @return void
 */
static void mail_fs_destroy(void *private_data){
	// In docs it's mentioned to "re-using handles is a key to good performance with libcurl"
	// so we shall clean it in the end, and not after every new call
	private_data_node* pvt_data=(private_data_node*)private_data;
	curl_easy_cleanup(pvt_data->curl);
}

/**
 * @brief function to allow write the specified buffer to the file
 * @return int: 0 if successful, -ERR if error
 */
static int mail_fs_write(const char * path, const char *buf, size_t size, off_t off, struct fuse_file_info *fi){
	// Note that for a simple update, we need to first delete the old file and then create a new one with updated content
	// This is because IMAP doesn't support updates. (See References in README)
	printf("PATH: %s\n", path);
	char new_content[size+1];
	strncpy(new_content,buf,size);
	new_content[size]='\0';
	printf("NEW CONTENT: %s\n", new_content);
	private_data_node* data=PVT_DATA;

	// save the content to server
	// delete old file
	char root_dirname[10000];
	char objectname[10000];
	split_path_to_components(root_dirname, objectname, path);
	int msg_id=fetch_msgid_by_subject_and_label(data->curl, objectname, root_dirname);
	printf("Msg Id: %d\n", msg_id);
	if(msg_id==-1){
		// a file which is opened and having a msg_id as 0 shouldn't happen
		// as opening a file validates the msg_id
		printf("Logic Error\n");
		return size;
	}else{
		// check for append only permission
		if(objectname[0]>='0' && objectname[0]<='9'){
			// fetch the old content
			char* old_content=fetch_email_content_by_id(data->curl,msg_id);int i=0,j=0;
			// check if the new_content is a strict addition to the old content; If not throw permission error!
			int n1=strlen(old_content), n2=strlen(new_content);
			while(i<n1 && j<n2){
				int is_old_rn=(i+1!=n1 && old_content[i]=='\r' && old_content[i+1]=='\n');
				int is_new_rn=(j+1!=n2 && new_content[j]=='\r' && new_content[j+1]=='\n');
				if(is_new_rn && is_old_rn){
					i+=2,j+=2;
				}else if(is_new_rn && old_content[i]=='\n'){
					i++,j+=2;
				}else if(is_old_rn && new_content[j]=='\n'){
					i+=2,j++;
				}else if(old_content[i]==new_content[j]){
					i++,j++;
				}else{
					return -EACCES;
				}
			}
			if(i!=n1) return -EACCES;
		}
		// delete the old message
		delete_email_by_id_and_folder(data->curl, msg_id, root_dirname);
	}
	// create new mail and push the new content
	create_new_mail(data->curl, root_dirname, objectname, new_content);

	return size;
}


/**
 * @brief function to create a new folder
 *
 * @param path: path of the new folder
 * @param mode: creation mode (Not used in implementation)
 * @return int: 0 if Success, -ERRCODE otherwise
 */
static int mail_fs_mkdir(const char * path, mode_t mode){
	char root_dirname[10000];
	char objectname[10000];
	split_path_to_components(root_dirname, objectname, path);
	if(strcmp(root_dirname,"/")==0){
		// allowed to create directory, as we are in root dir
		private_data_node* data=PVT_DATA;
		// invalidate cache entry
		invalidate_object_if_exist(path);
		// create a new label at server
		int res=create_new_label(data->curl, path);
		if(res){
			printf("Debug: Failed to create\n");
			return -ENONET;
		}
	}else{
		// only till 1 level depth creation of directories are allowed
		return -EACCES;
	}

	return 0;
}

/**
 * @brief function to create a new file node
 *
 * @param path: path of the new folder
 * @param mode: creation mode (Not used in implementation)
 * @param rdev: (Not used in implementation)
 * @return int: 0 if Success, -ERRCODE otherwise
 */
static int mail_fs_mknod(const char *path, mode_t mode, dev_t rdev){
	// create new file
	private_data_node* data=PVT_DATA;
	char root_dirname[10000];
	char objectname[10000];
	split_path_to_components(root_dirname, objectname, path);
	// create a new mail at server
	create_new_mail(data->curl, root_dirname, objectname, "");
	// invalidate cache entry
	invalidate_object_if_exist(path);
	return 0;
}

/**
 * @brief function to remove a directory
 *
 * @param path: path of the directory
 * @return int: 0 for SUCCESS, -ERRCODE otherwise
 */
static int mail_fs_rmdir(const char * path){
	private_data_node* data=PVT_DATA;
	// delete the label
	int res=delete_label(data->curl, path);
	if(res) return -ENONET; // network error
	// invalidate path in cache
	invalidate_object_if_exist(path);
	return 0;
}




/**
 * @brief function to allow renaming of dirs and files
 *
 * @param from_path: before path of the file/folder
 * @param to_path: new path of the file/folder
 * @param flags: Not used in implementation
 * @return int: 0 for SUCCESS, -ERRCODE otherwise
 */
static int mail_fs_rename(const char * from_path, const char * to_path, unsigned int flags){
	printf("Debug: log mail_fs_rename: [%s] [%s]\n", from_path, to_path);

	private_data_node* data=PVT_DATA;

	char to_root_dirname[10000];
	char to_objectname[10000];
	split_path_to_components(to_root_dirname, to_objectname, to_path);

	char from_root_dirname[10000];
	char from_objectname[10000];
	split_path_to_components(from_root_dirname, from_objectname, from_path);

	int from_object_type=fetch_object_type_by_dirname_and_objectname(data->curl,from_root_dirname,from_objectname,data->base_full_url);
	int to_object_type=fetch_object_type_by_dirname_and_objectname(data->curl, to_root_dirname,to_objectname,data->base_full_url);
	printf("TYPE: %d %d\n", from_object_type, to_object_type);

	if(from_object_type==2 && to_object_type==1){
		// Renaming: file to dir
		// Verdict: won't happen every; libfuse handles it internally
		exit(1);
		return 0;
	}else if(from_object_type==2 && (to_object_type==0 || to_object_type==2)) {
		// **CASES**
		// object_type==2; file to file: overwrite contents
		// object_type==0; file to none: rename the subject
		int msg_id;
		msg_id=fetch_msgid_by_subject_and_label(data->curl, from_objectname, from_root_dirname);
		char* content=fetch_email_content_by_id(data->curl,msg_id);

		// delete from email
		delete_email_by_id_and_folder(data->curl, msg_id, from_root_dirname);

		// delete the "to" file
		if(to_object_type==2){
			msg_id=fetch_msgid_by_subject_and_label(data->curl, to_objectname, to_root_dirname);
			if(msg_id!=-1){
				// if [to] msg exist; delete it
				delete_email_by_id_and_folder(data->curl, msg_id, to_root_dirname);
			}
		}
		// create a new mail with updated name
		int res=create_new_mail(data->curl, to_root_dirname, to_objectname, content);
		free(content);

		// invalidate cache entries
		invalidate_object_if_exist(to_path);
		invalidate_object_if_exist(from_path);
		if(res) return -ENONET;
		return 0;
	}else if(from_object_type==1 && to_object_type==0){
		// Renaming: dir to none
		// dest rootdir name should be /
		if(strcmp(to_root_dirname, "/")) return -EACCES;

		// rename the label
		if(change_label(data->curl, from_path, to_path)) return -ENONET;
		// invalidate cache
		invalidate_object_if_exist(to_path);
		invalidate_object_if_exist(from_path);
		return 0;
	}else if(from_object_type==1 && to_object_type==1) {
		// Renaming: dir to dir
		// Verdict: not allowed
		// handled by fuse
		return -EACCES;
	}else if(from_object_type==1 && to_object_type==2) {
		// Renaming: dir to file
		// Verdict: not allowed
		// handled by fuse
		return -ENONET;
	}else if(from_object_type==0){
		// source file/dir doesn't exists: not allowed
		// libfuse handles it
		return -ENONET;
	}

	return -ENONET;
}



/**
 * @brief function to allow deletion of files
 *
 * @param path: path of the file
 * @return int: 0 for SUCCESS, -ERRCODE otherwise
 */
static int mail_fs_unlink(const char * path){
	private_data_node* data=PVT_DATA;
	char root_dirname[10000];
	char objectname[10000];
	split_path_to_components(root_dirname, objectname, path);

	if(objectname[0]>='0' && objectname[0]<='9'){
		// not allowed as the starting is a numeral
		return -EACCES;
	}
	// fetch the message id
	int msg_id=fetch_msgid_by_subject_and_label(data->curl, objectname, root_dirname);
	if(msg_id==-1){
		return -ENONET;
	}
	// delete the mail
	if(delete_email_by_id_and_folder(data->curl,msg_id,root_dirname)){
		return -ENONET;
	}
	// invalidate cache
	invalidate_object_if_exist(path);
	return 0;
}



/**
 * @brief struct defines all the functionalities the fs supports
 */
static const struct fuse_operations mail_fs_operations = {
    .init           = mail_fs_init,
    .getattr        = mail_fs_getattr,
    .readdir        = mail_fs_readdir,
    .open           = mail_fs_open,
    .read           = mail_fs_read,
		.mkdir					= mail_fs_mkdir,
		.rmdir					= mail_fs_rmdir,
		.write					= mail_fs_write,
		.mknod					= mail_fs_mknod,
		.rename					= mail_fs_rename,
		.unlink					= mail_fs_unlink,
    .destroy 				= mail_fs_destroy,
};



int main(int argc, char* argv[]){
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
	config=(mailbox_config*)malloc(sizeof(mailbox_config));

	if (fuse_opt_parse(&args, config, option_spec, NULL) == -1)
		return 1;

	// initialize the cache
	hashmap_init();

	int ret;
	ret = fuse_main(args.argc, args.argv, &mail_fs_operations, NULL);
	fuse_opt_free_args(&args);

	free(config);
	return ret;
}

