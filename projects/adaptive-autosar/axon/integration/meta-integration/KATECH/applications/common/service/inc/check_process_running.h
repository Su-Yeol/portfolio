#ifndef __CHECK_PROCESS_RUNNING_H__
#define __CHECK_PROCESS_RUNNING_H__

#include <dirent.h>
#include <unistd.h>
#define HDMAP_PROCESS "Hdmap"

bool checkProcessRunning(char *p_processname) {
    DIR *dir_p;
    struct dirent *dir_entry_p;
    char dir_name[40];  //uffer overrun potential
    char target_name[252];  //buffer overrun potential
    int target_result;
    char exe_link[252];
    int errorcount;
    int result;

    errorcount=0;
    result=0;
    dir_p = opendir("/proc/"); // Open /proc/ directory
    while(NULL != (dir_entry_p = readdir(dir_p))) { // Reading /proc/ entries
        if (strspn(dir_entry_p->d_name, "0123456789") == strlen(dir_entry_p->d_name)) { // Checking for numbered directories 
            strcpy(dir_name, "/proc/");
            strcat(dir_name, dir_entry_p->d_name);
            strcat(dir_name, "/");  // Obtaining the full-path eg: /proc/24657/ 
            exe_link[0] = 0;
            strcat(exe_link, dir_name);
            strcat(exe_link, "exe"); // Getting the full-path of that exe link
            target_result = readlink(exe_link, target_name, sizeof(target_name)-1); // Getting the target of the exe ie to which binary it points to
            if (target_result > 0) {
                target_name[target_result] = 0;
                if (strstr(target_name, p_processname) != NULL) {   // Searching for process name in the target name -- ??? could be a better search !!!
                    result = atoi(dir_entry_p->d_name);
                    VERBOSE("getProcessID(%s) :Found. id = %d\n", p_processname, result);
                    closedir(dir_p);
                    return (result != 0) ? true : false ;
                }
            }
        }
    }
    closedir(dir_p);
    VERBOSE("getProcessID(%s) : id = 0 (could not find process)\n", p_processname);
    return false;
}

#endif