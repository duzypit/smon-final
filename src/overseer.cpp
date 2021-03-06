#include "../include/overseer.hpp"
Overseer::Overseer() : _threads(), _scribe()
{
    _scribe.start(std::ref(_msgQueue), std::ref(_mutex));
};

Overseer::~Overseer(){};

void Overseer::dispatch(const Request& r)
{
    switch (r.command) {
        case 'a':
            add(r);
            break;
        case 'l':
            listJobs();
            break;
        case 's':
            stop(r.job_id);
            break;
        case 'r':
            run(r.job_id);
            break;
        case 'd':
            deleteThread(r.job_id);
            break;
        default:
            //throw std::runtime_error("Overseer: Unknown command! Aborted.");
            std::cout << "Overseer: Unknown command! Aborted." << std::endl;
    }
}

void Overseer::add(const Request& r)
{
    auto tmp = std::make_shared<Beholder>(r);
    tmp -> start(std::ref(_msgQueue), std::ref(_mutex));
    _threads.push_back(tmp);
}

void Overseer::stop(std::size_t id)
{
    if( id <= _threads.size())
    {
        if(_threads[id] -> stopped() == false)
            _threads[id] -> stop();
    }
    else
    {
        std::cout << "Job id out of range! Aborted" << std::endl;
    }

}

void Overseer::run(std::size_t id)
{
    if( id <= _threads.size())
    {
        if(_threads[id] -> stopped() == true)
            _threads[id] -> run();
    }
    else
    {
        std::cout << "Job id out of range! Aborted" << std::endl;
    }

}

void Overseer::deleteThread(std::size_t id)
{
    if( id <= _threads.size())
    {

            _threads[id] -> stop();
            _threads.erase(_threads.begin()+id);
            //remove deleted line from config file

            std::string sourceFile("pprovidence.cfg");
            std::string destFile("tmp.cfg");
            std::fstream ifs;
            ifs.open(sourceFile);
            std::ofstream swap;
            swap.open(destFile, std::ios::app);

            if (ifs && swap)
            {
                std::string line;
                std::size_t counter = 0;
                while(std::getline(ifs, line))
                {
                    if (counter != id){
                        swap << line << std::endl;
                    }
                    ++counter;
                }
                swap.close();
                ifs.close();
                remove(sourceFile.c_str());
                rename(destFile.c_str(), sourceFile.c_str());
            }
            else
            {
                std::cout << "Couldn't open " << sourceFile << " or " << destFile << " for reading" << std::endl;
            }

    }
    else
    {
        std::cout << "Job id out of range! Aborted" << std::endl;
    }


}

void Overseer::listJobs()
{
    if(_threads.size() != 0) {
        std::cout <<
        std::setw(4) << std::left <<
        "id" <<
        std::setw(20) << std::left <<
        "ip" <<
        std::setw(8) << std::left <<
        "port" <<
        std::setw(20) << std::left <<
        "interval (seconds)" <<
        std::setw(30) << std::left <<
        "e-mail " <<
        std::setw(20) << std::left <<
        "sleeping/working" <<
        std::setw(20) << std::left <<
        "last status" <<
        std::setw(30) << std::left <<
        "last check time" <<

        std::endl;

        for(auto& b : _threads )
        {
            std::time_t lastCheckTime = b->lastCheck();
            std::tm tm = *std::localtime(&lastCheckTime);

            std::cout <<
            std::setw(4) << std::left <<
            std::distance(_threads.begin(), std::find(_threads.begin(), _threads.end(), b)) <<
            std::setw(20) << std::left <<
            b->ip() <<
            std::setw(8) << std::left <<
            b->port() <<
            std::setw(20) << std::left <<
            b->interval() <<
            std::setw(30) << std::left <<
            b->email() <<
            std::setw(20) << std::left <<
            (b->stopped() ? "sleeping" : "working") <<
            std::setw(20) << std::left;
            //last checked status & time before first chceck

            if (lastCheckTime == 0)
            {
                std::cout << "-" << "-" << std::endl;
            } else
            {
                std::cout <<
                (b->lastCheckOk() ? "failed" : "ok") <<
                std::put_time(&tm, "%F %T") <<
                std::endl;
            }
        }
    }
    else
    {
        std::cout << "Overseer: no jobs to list." << std::endl;
    }
}

void Overseer::setUpMailer(GmailCredentials& credentials)
{
    _scribe.setUpMailer(credentials);
}

