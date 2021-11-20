# Operating Systems: Assignment 5


The following assignment implements a FUSE based file system that uses mounts the emails and labels stored in Google's IMAP server to the local filesystem.



## Modules and Directory Structure
```bash
├── .editorconfig
├── src
│   ├── main.c # entrypoint to
│   ├── services # contains methods to interact with the gmail server using libcurl
│   │   ├── delete_email_by_id_and_folder.h # method to delete email using unique id and folder/label name
│   │   ├── create_new_mail.h # method to create a new mail
│   │   ├── fetch_all_labels.h # method to fetch all label
│   │   ├── fetch_email_subject_by_id.h # method to fetch email subject using its unique id
│   │   ├── add_label_to_email.h # method to add label to email
│   │   ├── delete_label_from_email.h # method to delete label from email
│   │   ├── create_new_label.h # method to create new label/folder
│   │   ├── change_label.h # method to deep recursively update/change the label
│   │   ├── curl_handle_init.h # method to handle the curl handle, and reset it
│   │   ├── fetch_object_type.h # method to fetch the current object type for any resource like /foo/a.txt, /bar, /bar/helo.txt
│   │   ├── relabel_an_email.h # method to relabel and email
│   │   ├── delete_label.h # method to delete a label/folder
│   │   ├── fetch_all_emails_by_label.h # method to fetch all emails by label
│   │   ├── fetch_email_content_by_id.h # method to fetch email content using its unique id
│   │   └── fetch_msgid_by_query.h # method to fetch message id's using a standard query
│   ├── test
│   │   └── main.c # contains tests for services/
│   └── utils
│       ├── write_callback.h # generic methods to redirect the write head to the specified buffer. (libcurl specific method)
│       ├── custom_string.h # wrapper function for char* to allow dynamic behaviour
│       ├── definitions.h # contains generic definitions and configuration
│       ├── stack.h # stack operations
│       ├── string_helpers.h # contains string helper functions like reverse a string etc
│       ├── decode.h # function to decode the quote printable encoded message
│       ├── uuid_ops.h # function to generate uuid
│       └── hashmap.h # contains LRU cache implementation
└── .gitignore
```

## Installing dependencies

Before moving further we need `uuid`, `libcurl`, and `libfuse` installed on the local machine.

```
sudo apt-get install python3 python3-pip python3-setuptools python3-wheel ninja-build
sudo apt install meson cmake fuse3 libfuse3-dev libglib2.0-dev
pip3 install meson
```

## Compile

Inside the src directory run the following command to generate an executable! Ensure that curl, uuid and fuse are linked.
```bash
gcc -Wall main.c  -lcurl -luuid `pkg-config fuse3 --cflags --libs`

```

## Executing

```bash
./a.out "MOUNT_DIR_PATH" --hostname="imaps://imap.gmail.com" --port="993" --email="EMAIL" --password='PASSWORD' -s
```
> Note: Run it on single thread to utilize the caches more effectively!


## Assumptions and Important points
> Ensure that no trailing slash in IP Address.

> Every IMAP server interaction is wrapped into a separate file, as most of them requires custom parsing.

> Any attempt to create a dir after one level will lead to Permission Error!

> Note that throughout this project the term Label and Folder are interchangeably used.

## Unmounting
```bash
fusermount -u "MOUNT_DIR_PATH"
```

## Credits

1. Quoted printable parser: https://www.w3.org/Tools/Mail-Transcode/mail-transcode.c
2. We cannote update a mail, so we must delete it and then create a new one: [LINK](https://stackoverflow.com/questions/16945041/imap-editing-a-draft-or-existing-message#:~:text=There%20are%20no%20IMAP%20commands,to%20existing%20messages%20(%20STORE%20).&text=Indeed%2C%20the%20specification%20guarantees%20that,with%20a%20UID)%20is%20immutable).
3. https://github.com/billziss-gh/winfsp/issues/44
4. Why we are not reporting size in `getattr`: https://stackoverflow.com/questions/46267972/fuse-avoid-calculating-size-in-getattr
5. Unmounting cleanly issue: https://github.com/billziss-gh/cgofuse/issues/6
















