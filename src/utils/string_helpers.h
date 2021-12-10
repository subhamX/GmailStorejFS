#ifndef STRING_HELPERS_H
#define STRING_HELPERS_H



/**
 * @brief function to convert integer to string
 * > ensure that enough size is allocated to res; although checks are in place
 * > sz is size of the [res] string
 *
 * @param res: pointer to the char array to get the results
 * @param sz
 * @param n
 */
void int_to_string(char* res, int sz, int n){
	assert(sz); // ensure that sz is actually some valid non zero number

	int i=0;
	while(n){
		res[i++]=(n%10)+'0';
		n/=10;
		assert(i!=sz);
	}
	res[i]='\0';

	// reverse [0...i-1]
	for(int j=0;j<(i)/2;j++){
		char tmp=res[j];
		res[j]=res[i-1-j];
		res[i-1-j]=tmp;
	}
}

/**
 * @brief function to reverse the string
 *
 * @param str: input string to reverse inplace
 * @param n: size of the string
 */
void reverse_str(char* str, int n){
	// reverse
	for(int k=0;k<n/2;k++){
		char tmp=str[k];
		str[k]=str[n-1-k];
		str[n-1-k]=tmp;
	}
}





// objectname because it can be dirname or objectname
/**
 * @brief function to split the path to rootname and objectname
 *
 * Cases:
 * if dir then objectname is '\0'
 * /bar; where bar is a directory will return {/, bar}
 * /hello.txt; where hello.txt is a file will return {/, hello.txt}
 * /bar/hello.txt; where hello.txt is a file will return {/bar, hello.txt}
 * /; will return error; this should be handled there itself
 *
 * @param root_dirname: pointer to char array to store the root_dirname extracted from path
 * @param objectname: pointer to char array to store the objectname extracted from path
 * @param path
 */
void split_path_to_components(char* root_dirname, char* objectname, const char* path){
	objectname[0]='\0';
	root_dirname[0]='/';
	assert(strcmp(path,"/")!=0); // shouldn't be same

	int n=strlen(path);
	int i=n-1;
	int object_ptr=0;
	while(i>=0 && path[i]!='/'){
		objectname[object_ptr++]=path[i--];
	}
	objectname[object_ptr]='\0';
	// reverse string
	reverse_str(objectname,object_ptr);

	if(i==0){
		root_dirname[0]='/';
		root_dirname[1]='\0';
	}else{
		assert(path[i]=='/');
		i--;
		int root_ptr=0;
		while(i>=0){
			root_dirname[root_ptr++]=path[i--];
		}
		root_dirname[root_ptr]='\0';
		// reverse string
		reverse_str(root_dirname,root_ptr);
	}
}


#endif
