#include "Log.h"
#include <sstream>
#if defined(_WIN32)
# include <stdarg.h>
# include <time.h>
#elif defined(__linux)
# include <stdarg.h>
#else
# include <sys/time.h>
#endif

namespace HPV {
    
    Log hpv_log;
    
    /* --------------------------------------------------------------------------------- */
    Log::Log() :
    write_to_stdout(true),
    write_to_file(true),
    level(HPV_LOG_LEVEL_ALL)
    {
    }
    
    Log::~Log()
    {
        if (ofs.is_open())
        {
            ofs.close();
        }
        
        write_to_stdout = false;
        write_to_file = false;
    }
    
    int Log::open(std::string filep, int mode)
    {
        if (!write_to_file)
        {
            return 0;
        }
        
        if (filepath.size() != 0)
        {
            printf("Error: trying to open the log file but it's already open? Calling hpv_log_init() twice?\n");
            return 0;
        }
        
        filepath = filep;
        
        if (0 == filepath.size())
        {
            printf("Error: cannot open the log filepath because the string is empty.\n");
            return 0;
        }
        
        if (mode == HPV_LOG_APPEND)
        {
            ofs.open(filepath.c_str(), std::ios::out | std::ios::app);
        }
        else
        {
            ofs.open(filepath.c_str(), std::ios::out | std::ios::trunc);
        }
        
        if (!ofs.is_open())
        {
            printf("Error: cannot open the log file. No permission? %s\n", filepath.c_str());
            return 0;
        }
        
        return 1;
    }
    
    void Log::log(int inlevel, const char* fmt, va_list args)
    {
        if (inlevel > level)
        {
            return;
        }
        
        static char buffer[1024 * 4];
        std::string slevel;
        std::stringstream ss_stdout;
        
        if (write_to_file)
        {
            if (!ofs.is_open())
            {
                return;
            }
        }
        
        vsprintf(buffer, fmt, args);
        
        if (write_to_file)
        {
            time_t ltime; /* calendar time */
            ltime=time(NULL); /* get current cal time */
            ofs << asctime( localtime(&ltime)) << " " ;
        }
        
        if (inlevel == HPV_LOG_LEVEL_DEBUG)
        {
            slevel = "[ debug ]";
            
            if (write_to_file)
            {
                ofs << slevel << ": ";
            }
        }
        else if (inlevel == HPV_LOG_LEVEL_VERBOSE)
        {
            slevel = "[verbose]";
            
            if (write_to_file)
            {
                ofs << slevel << ": ";
            }
        }
        else if (inlevel == HPV_LOG_LEVEL_WARNING)
        {
            slevel =  "[warning]";
            
            if (write_to_file)
            {
                ofs << slevel << ": ";
            }
        }
        else if (inlevel == HPV_LOG_LEVEL_ERROR)
        {
            slevel = "[ error ]";
            
            if (write_to_file)
            {
                ofs << slevel << ": ";
            }
        }
        
        if (write_to_file)
        {
            ofs <<  buffer << "\n";
        }
        
        if (write_to_stdout)
        {
            ss_stdout
            << slevel
            << ": "
            << buffer
            << std::endl;
            printf("%s", ss_stdout.str().c_str());
        }
        
        if (write_to_file)
        {
            ofs.flush();
        }
    }
    
    int hpv_log_init(std::string path, int mode)
    {
        std::stringstream ss;
        std::string filepath = "";
        char buf[4096];
        time_t t;
        struct tm* info;
        
        time(&t);
        info = localtime(&t);
        strftime(buf, 4096, "%Y.%m.%d", info);
        
        if (0 == path.size()) {
            filepath = "./";
        }
        else {
            filepath = path +"/";
        }
        
        ss << filepath << "log-" << buf << ".log";
        filepath = ss.str();
        
        return hpv_log.open(filepath, mode);
    }
    
    void hpv_log_disable_stdout()
    {
        hpv_log.write_to_stdout = false;
    }
    
    void hpv_log_enable_stdout()
    {
        hpv_log.write_to_stdout = false;
    }
    
    void hpv_log_disable_log_to_file()
    {
        hpv_log.write_to_file = false;
    }
    
    void hpv_log_enable_log_to_file()
    {
        hpv_log.write_to_file = true;
    }
    
    void hpv_log_set_level(int level)
    {
        hpv_log.level = level;
    }
    
    int hpv_log_get_level()
    {
        return hpv_log.level;
    }
    
    void hpv_debug(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        hpv_log.log(HPV_LOG_LEVEL_DEBUG, fmt, args);
        va_end(args);
    }
    
    void hpv_verbose(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        hpv_log.log(HPV_LOG_LEVEL_VERBOSE, fmt, args);
        va_end(args);
    }
    
    void hpv_warning(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        hpv_log.log(HPV_LOG_LEVEL_WARNING, fmt, args);
        va_end(args);
    }
    
    void hpv_error(const char* fmt, ...)
    {
        //std::cout << fmt;
        va_list args;
        va_start(args, fmt);
        hpv_log.log(HPV_LOG_LEVEL_ERROR, fmt, args);
        va_end(args);
    }
} /* End HPV namespace */