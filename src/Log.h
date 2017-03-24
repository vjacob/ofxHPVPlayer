/**********************************************************
* Holo_ToolSet
* http://github.com/HasseltVR/Holo_ToolSet
* http://www.uhasselt.be/edm
*
* Distributed under LGPL v2.1 Licence
* http ://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
*
* Parts are based on code from roxlu https://github.com/roxlu/roxlu_experimental
**********************************************************/
#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <inttypes.h>

#define HPV_LOG_LEVEL_ERROR     1
#define HPV_LOG_LEVEL_WARNING   2
#define HPV_LOG_LEVEL_VERBOSE   3
#define HPV_LOG_LEVEL_DEBUG     4
#define HPV_LOG_LEVEL_ALL       5

#define HPV_LOG_TRUNCATE        0
#define HPV_LOG_APPEND          1

#  define HPV_DEBUG(fmt, ...) { hpv_debug(fmt, ##__VA_ARGS__); }
#  define HPV_VERBOSE(fmt, ...) { hpv_verbose(fmt, ##__VA_ARGS__); }
#  define HPV_WARNING(fmt, ...) { hpv_warning(fmt, ##__VA_ARGS__); }
#  define HPV_ERROR(fmt, ...) { hpv_error(fmt, ##__VA_ARGS__); }

namespace HPV {

    int hpv_log_init(std::string path = "", int mode = HPV_LOG_APPEND);
    void hpv_log_disable_stdout();
    void hpv_log_enable_stdout();
    void hpv_log_disable_log_to_file();
    void hpv_log_enable_log_to_file();
    void hpv_log_set_level(int level);
    int hpv_log_get_level();

    void hpv_debug(const char* fmt, ...);
    void hpv_verbose(const char* fmt, ...);
    void hpv_warning(const char* fmt, ...);
    void hpv_error(const char* fmt, ...);

    /*
     * LOG class for logging to file and/or stdout
     */
    class Log
    {
    public:
        Log();
        ~Log();
        int open(std::string filepath, int mode);
        void log(int level, const char * fmt, va_list args);
        
    public:
        bool write_to_stdout;                  /* Write output also to stdout. */
        bool write_to_file;                    /* Write log to file. */
        int level;                             /* What level we should log. */
        
    private:
        std::string filepath;                  /* Filepath where we save the log file. */
        std::ofstream ofs;                     /* The output file stream */
    };

  extern Log hpv_log;

} /* End HPV namespace */