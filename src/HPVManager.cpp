#if _WIN32
//extern "C" IMAGE_DOS_HEADER __ImageBase;
#elif defined(__APPLE__)
#include <dlfcn.h>
#elif defined(__linux)
#include <limits.h>
#include <sys/stat.h>
#endif

#include "HPVManager.h"
#include "Log.h"
#include "HPVHeader.h"
#include "HPVRenderBridge.h"

/* The HPV Manager Singleton */
HPV::HPVManager m_HPVManager;

static void initLog(bool log_to_file)
{
    if (log_to_file)
    {
        std::string path;
#if _WIN32
        char dllPath[MAX_PATH] = { 0 };
        IMAGE_DOS_HEADER __ImageBase;
        GetModuleFileNameA((HINSTANCE)&__ImageBase, dllPath, _countof(dllPath));
        
        std::string full_path = std::string(dllPath);
        std::size_t found = full_path.find_last_of("/\\");
        path = full_path.substr(0, found);
#elif __APPLE__
        Dl_info info;
        if (dladdr((const void *)initLog, &info)) 
        {
            path += std::string(info.dli_fname);
            std::string app_suffix(".app");
            int split_idx = path.rfind(app_suffix);
            path = path.substr(0,split_idx+app_suffix.size());
            split_idx = path.rfind("/");
            path = path.substr(0, split_idx);
        }
#elif __linux__
        char buf[PATH_MAX];
        ssize_t num_bytes;
        num_bytes = readlink("/proc/self/exe", buf, sizeof(buf));
        if (num_bytes > 0)
        {
            path = std::string(buf);
            int split_idx = path.rfind("/");
            path = path.substr(0, split_idx);
        }
#endif
        if (path.size() > 0)
        {
            HPV::hpv_log_enable_log_to_file();
            HPV::hpv_log_init(path.c_str(), HPV_LOG_APPEND);
        }
        else
        {
            HPV::hpv_log_disable_log_to_file();
        }
    }
    else
    {
        HPV::hpv_log_disable_log_to_file();
        
    }
    HPV::hpv_log_set_level(HPV_LOG_LEVEL_ALL);
}

namespace HPV {
    
    HPVManager::HPVManager()
    {
        m_players.clear();
        m_num_players = 0;
    }
    
    HPVManager::~HPVManager()
    {
        m_event_listeners.clear();
        HPV_VERBOSE("~HPVMAnager");
    }
    
    int8_t HPVManager::addPlayer()
    {
        int8_t node_idx = -1;
        
        if (m_players.size() <= HPV::MAX_NUMBER_OF_PLAYERS)
        {
            std::shared_ptr<HPVPlayer> new_player = std::make_shared<HPVPlayer>();
            if (m_event_listeners.size())
            {
                new_player->addHPVEventSink(&m_event_queue);
            }
            node_idx = m_num_players;
            m_players.insert(std::pair<uint8_t, HPVPlayerRef>(node_idx, new_player));
            new_player->_id = node_idx;
            m_num_players++;
        }
        
        return node_idx;
    }
    
    uint8_t HPVManager::initPlayer()
    {
        return this->addPlayer();
    }
    
    HPVPlayerRef HPVManager::getPlayer(uint8_t node_id)
    {
        if (isValidNodeId(node_id))
        {
            return m_players[node_id];
        }
        
        return nullptr;
    }
    
    
    std::vector<bool> HPVManager::update()
    {
        std::vector<bool> update_flags;

        if (m_players.size() > 0)
        {
            update_flags.resize(m_players.size(), false);

            for (uint8_t i = 0; i < m_players.size(); ++i)
            {
                if (!m_players[i]->isLoaded())
                {
                    update_flags[i] = false;
                }
                else
                {
                    update_flags[i] = m_players[i]->hasNewFrame();
                }
            }
        }
        
        return update_flags;
    }
    
    void HPVManager::closeAll()
    {
        for (auto& player : m_players)
        {
            player.second->close();
        }
        
        m_num_players = 0;

        m_players.clear();
        m_event_queue.clear();
        HPV_VERBOSE("Cleared all HPV Players");
    }
    
    void HPVManager::postEvent(const HPVEvent& event)
    {
        m_event_queue.push(event);
    }
    
    void HPVManager::processEvents()
    {
        while (m_event_queue.size() > 0)
        {
            HPVEvent event(HPVEventType::HPV_EVENT_NUM_TYPES, nullptr);
            
            if (m_event_queue.try_pop(event))
            {             
                for (HPVEventCallback& listener : m_event_listeners)
                {
                    if (listener) listener(event);
                }
            }
        }
    }
    
    /*******************************************************************************
     * GLOBAL Manager functions
     *******************************************************************************/
    HPVManager * ManagerSingleton()
    {
        return &m_HPVManager;
    }
    
    void InitHPVEngine(bool log_to_file /* = false */)
    {
        initLog(log_to_file);
        
        try
        {
            RendererSingleton()->load();
        }
        catch (std::exception& e)
        {
            HPV_ERROR("%s", e.what());
            std::exit(EXIT_FAILURE);
        }
    }
    
    void DestroyHPVEngine()
    {
        ManagerSingleton()->closeAll();
        RendererSingleton()->deleteGPUResources();
        RendererSingleton()->unload();
    }
    
    HPVPlayerRef NewPlayer()
    {
        uint8_t node_idx = ManagerSingleton()->initPlayer();
        return ManagerSingleton()->getPlayer(node_idx);
    }
  
    void Update()
    {
        if (ManagerSingleton()->getNumPlayers() > 0)
        {
            ManagerSingleton()->processEvents();
            RendererSingleton()->updateTextures();
        }
    }  
} /* End HPV namespace */
