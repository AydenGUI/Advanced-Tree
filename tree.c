#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

int directory_items(char *dirname);
int side_by_side(char *dir1, char *dir2);
int print_tree(char *dirname, char *prefix, int depth,int unhide_hidden,int show_perms,int show_size);
int directory_items(char *dirname) {
    int sum_items = 0;
    DIR *mydir;
    struct dirent *myfile = NULL;
    char fname[120];
    mydir = opendir(dirname);
    while ((myfile = readdir(mydir)) != NULL) {
//        strcpy(fname, myfile->d_name);
//        if (fname[0] != '.')
        sum_items++;
    }
    closedir(mydir);
    return sum_items;
} //directory_items

int side_by_side(char *dir1, char *dir2) {
    FILE *fp1, *fp2;
    char line1[91], line2[91]; //I set the lengths to 91 because max console char length is set a 182
    char spacing[20];
    char *get1, *get2;
    int boo = 0;
    fp1 = fopen(dir1, "r");
    fp2 = fopen(dir2, "r");

    if (!fp1 || !fp2) {
        perror("Error opening file");
        return 1;
    }

    get1 = "aaa";
    get2 = "bbb";
//    strcpy(get1, " ");
    //to make it not NULL

    //while ((get1 = fgets(line1, 91, fp1)) != NULL && (get2 = fgets(line2, 91, fp2)) != NULL) {
    while (get1 != NULL || get2 != NULL) {
        //((get1 = fgets(line1, 91, fp1)) != NULL && (get2 = fgets(line2, 91, fp2)) != NULL) {
        if (get1 != NULL)
            get1 = fgets(line1, 91, fp1);
        if (get2 != NULL)
            get2 = fgets(line2, 91, fp2);



        if (get1 == NULL && get2 == NULL)
            break;
        if (get1 == NULL) {
            //printf("Line1 done: %s\n", line1);
            strcpy(line1, "");
            strcpy(spacing, "");
        } else if (line1[strlen(line1) - 1] == '\n')
            line1[strlen(line1) - 1] = ' ';
        //removes \n from line1

        if (get2 == NULL) {
            strcpy(line2, "\n");
            //makes sure line2 has \n
        }


        //removes the \n from end of line1
        int num;
        char ptr[200], *str;
        strcpy(ptr, line1);
        //Fixes issues with non keyboard characters and ANSI colors
        //Both mess up spacing so %-80s needs additional spacing to fix it
        while (strstr(ptr, "\033[3") != NULL || (strstr(ptr, "│   ├") != NULL || strstr(ptr, "  └") != NULL))
        {

            if ((str = strstr(ptr, "│   │")) != NULL || (str = strstr(ptr, "│   │")) != NULL) {
                strcat(spacing, "  ");
                strcpy(ptr, ptr + num + 3);
                num = str- ptr;
                if (boo == 1) {
                    strcat(spacing, "  ");
                }
            }

                if ((str = strstr(ptr, "│   ├")) !=NULL || (str = strstr(ptr, "  └")) != NULL) {
                    if (strstr(ptr, "  └") != NULL && strstr(ptr, "│") == NULL && ptr[0] == ' '){
                        //printf("YESY");
                    }
                    else {
                        strcat(spacing, "  ");
                    }
                        num = str - ptr;
                        strcpy(ptr, ptr + num + 5);

                }
                if ((str = strstr(ptr, "\033[32")) != NULL) {
                strcat(spacing, "     ");
                num = str - ptr;
                strcpy(ptr, ptr + num + 4);
                }
            if ((str = strstr(ptr, "\033[31")) != NULL) {
                strcat(spacing, "     ");
                num = str - ptr;
                strcpy(ptr, ptr + num + 4);
                //printf("\n\nNUM: %d\nPOINT: %s\n\n",num, ptr);
            }
            if ((str = strstr(ptr, "\033[34")) != NULL) {
            strcat(spacing, "     ");
            num = str - ptr;
            strcpy(ptr, ptr + num + 4);
            //          printf("\n\nNUM: %d\nPOINT: %s\n\n",num, ptr);
            }



        } //while
            //color messes with the charlength and thus allignement of lines
        //here we check if the color exists and thus add appropriate spacing with char *spacing

        //      line1[strlen(line1)] = ' ';
        printf("%-80s%s : %s", line1, spacing,  line2);

        if (strstr(line1, "\033[34") != NULL) {
            boo = 1;
        } else boo = 0;

        if (get1 != NULL)
            strcpy(spacing, "          ");

    }

    // Close the files
    fclose(fp1);
    fclose(fp2);
    return 0;
} // side_by_side

int print_tree(char *dirname, char *prefix,int depth,int unhide_hidden,int show_perms,int show_size) {
    struct dirent *myfile = NULL;
    DIR *mydir;
    struct stat mystat;
    char fname[300], preprefix[strlen(prefix) + 4], suffix[14];
    char full_path[260];
    char *color = "";//"\033[0m";
    int err, sum_items = 0;
    if (depth == 0)
        return 0;
    //This checks if you are at maximum depth and if you are it doesn't print anything
    if (dirname[strlen(dirname) -1] != '/')
        strcat(dirname, "/");
    //This prevents errors caused by directory names lacking ending /'s


    err = lstat(dirname, &mystat);
    //Theses errors check the validity of the directory search
    //They end this iteration of the method if the directory fails one of these checks
    if (err == -1) {
        if (ENOENT == errno) {
            fprintf(stderr, "Error: %s is not a valid directory.\n", dirname);
            return -1;
        }
        if (EACCES == errno) {
            fprintf(stderr, "Permission Error: Can't access directory.\n");
            return -1;
        } else
            fprintf(stderr, "Error: Error attempting to find directory %s.\n", dirname);
        return -1;
    }

    mydir = opendir(dirname);
    //printf("GETSHERE");
    if (mydir != NULL ) {
        sum_items = directory_items(dirname);

        while ((myfile = readdir(mydir)) != NULL) {
            sum_items--;

            strcpy(full_path, dirname);
            strcpy(fname, myfile->d_name);
            strcat(full_path, fname);
            lstat(full_path, &mystat);
            if (unhide_hidden == 0) {
                if (fname[0] == '.')
                    continue;
                //if file/directory starts with . don't skip it
            }
            if (S_ISDIR(mystat.st_mode)) {
                if (unhide_hidden || (strcmp(fname, ".") != 0 && strcmp(fname, "..") != 0)) {
                    //doesn't print . or .. unless option selected
                    if (!S_ISLNK(mystat.st_mode)) {

                        //makes directory name blue

                        if (sum_items) {
                            //If remaining items in directory != 0
                            printf("%s├──\033[34m%s\033[0m\n", prefix, fname);
                            strcpy(preprefix, prefix);
                            strcat(preprefix, "│   ");
                            //Keeps | for the directory in prefix
                        } else  {
                            printf("%s└──\033[34m%s\033[0m\n", prefix, fname);
                            strcpy(preprefix, prefix);
                            strcat(preprefix, "   ");
                        }
                        if (strcmp(fname, ".") != 0 && strcmp(fname, "..") != 0)
                            print_tree( full_path, preprefix, depth - 1, unhide_hidden,show_perms,show_size);
                    }
                }
            } else {
                //Checks if colors need to be applied and applies them
                if ((mystat.st_mode & S_IXUSR) != 0)
                    color = "\033[32m";     //green
                if ((mystat.st_mode & S_IWUSR) == 0)
                    color = "\033[31m";     //red
                if (sum_items)
                    printf("%s├──%s%s\033[0m", prefix, color, fname);
                else printf("%s└──%s%s\033[0m", prefix, color, fname);
                color = "";

                if (show_perms == 1) {
                    int mode = mystat.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
                    //The extra flags get mode in octal
                    printf(" (%o)   ", mode);
                    //if show_perms end prints with this
                }

                if (show_size == 1) {
                    int size = mystat.st_size/1024;
                    printf(" %oK", size);
                }
                printf("\n");
            } // else
        }
        closedir(mydir);
    }

    return 0;
} // print_tree

int main(int argc, char *argv[]) {
    int opt;
    int depth = -1;           //If >= 0 determines alloted depth for tree
    int reveal_hidden = 0;    //Reveals hidden files and directories
    int permissions = 0;      //Display permissions
    int filesize = 0;         //Display filesize
    int cmpdir = 0;           //Compare directories
    int pid[2];
    char dirname[260] = ".", cmpdir1[260], cmpdir2[260];

    if (argc > 1 && argv[1][0] != '-') {
        strcpy(dirname, argv[1]);
        //if the first argument of the tree command is not an opt it is used as the directory to tree.
        //else it searches thorugh current directory (./).
    }
    while ((opt = getopt(argc, argv, "d:hrpsc")) != -1) {
        switch (opt) {
        case 'd':
            depth = atoi(optarg);
            break;
        case 'r':
            reveal_hidden = 1;
            break;
        case 'p':
            permissions = 1;
            break;
        case 's':
            filesize = 1;
            break;
        case 'h':
            printf("Usage: ./tree [directory-to-search] [-d <depth>] [-r] [-p] [-s] [-h] [-c <directory1> <directory2>]\n\n");
            printf("Options:\n");
            printf("-d <depth>   Max allowed tree depth\n");
            printf("-r           Displays hidden files and directories\n");
            printf("-p           Displays file permissions\n");
            printf("-s           Displays file sizes\n");
            printf("-h           Displays help message\n");
            printf("-c <d1> <d2> Compares two directories trees side by side\n\n");
            printf("Note: All options are optional, including the directory argument.\n");
            return 0;
            break;
        case 'c':

            //strcpy(cmpdir1, optarg);
            if (argc < optind + 2) {
                fprintf(stderr, "Error: Missing one or more directory argument for -c option.\n");
                exit(EXIT_FAILURE);
                // see if there are enough remaining arguments for -c
            }
            strcpy(cmpdir1, argv[optind]);
            strcpy(cmpdir2, argv[optind + 1]);
            if (cmpdir1[0] == '-' || cmpdir2[0] == '-') {
                fprintf(stderr, "Error: Missing one or more directory argument for -c option.\n");
                exit(EXIT_FAILURE);
                // checks if the following argument is an opt instead of directory
            }
            cmpdir = 1;
            break;
        default:
            fprintf(stderr, "Usage: ./tree [directory-to-search] [-d <depth>] [-r] [-p] [-s] [-h] [-c <directory1> <directory2>]\n");
            return 1;
        }
    }

    if (cmpdir) {
        int fd1, fd2;
        //forks 2 children to use print_tree for each directory to compare but redirects their output to the designated newfiles
        pid[0] = fork();
        if (pid[0] == 0) {
            fd1 = open("cmpfile1.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
            if (fd1 < 0) {
                perror("Error opening output file");
                return 1;
            }

            // Redirect stdout to the new file
            if (dup2(fd1, STDOUT_FILENO) < 0) {
                perror("Error redirecting stdout");
                return 1;
            }
            printf("Directory: %s\n", cmpdir1);
            print_tree(cmpdir1, "", depth, reveal_hidden, permissions, filesize);
            close(fd1);
            exit(EXIT_SUCCESS);
        }
        wait(NULL);
        if (pid[0]) {
            pid[1] = fork();
        }
        if (pid[1] == 0) {
            fd2 = open("cmpfile2.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
            if (fd2 < 0) {
                perror("Error opening output file");
                return 1;
            }
            // Redirect stdout to the new file
            if (dup2(fd2, STDOUT_FILENO) < 0) {
                perror("Error redirecting stdout");
                return 1;
            }
            printf("Directory: %s\n", cmpdir2);
            print_tree(cmpdir2, "", depth, reveal_hidden, permissions, filesize);
            close(fd2);
            exit(EXIT_SUCCESS);
        }
        //now the output has been written to the two different files
        //here using the method side_by_side it will print the two outputs side by side
        wait(NULL);
        //takes the two files and displays them side by side.
        side_by_side("cmpfile1.txt", "cmpfile2.txt");
    } else { // normal case (not cmpdirs)
        printf("%s\n", dirname);
        print_tree(dirname, "", depth, reveal_hidden, permissions, filesize);
//        printf("depth: %d, reveal_hidden: %d, permissions: %d, filesize: %d\n", depth, reveal_hidden, permissions, filesize);
    }

    return 0;
}
