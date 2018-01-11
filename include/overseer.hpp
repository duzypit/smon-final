#ifndef __OVERSEER_HPP__
#define __OVERSEER_HPP__
#include <thread>
#include <vector>
#include <algorithm>
#include <deque>
#include <mutex>
#include <iomanip> //std::setw()
#include <condition_variable>

#include "../include/datatypes.hpp"
#include "../include/beholder.hpp"
#include "../include/scribe.hpp"

class Overseer
{
public:
	Overseer(std::string filename) : _threads(), _scribe(filename)
    {
        _scribe.start(std::ref(_msgQueue),/* std::ref(_condVar),*/ std::ref(_mutex));
    };

    ~Overseer()
    {
        std::cout << "Overseer joined!" << std::endl;
    };

    void dispatch(const Request& r)
    {
        switch (r.command) {
            case 'a':
                add(r);
                break;
            case 'l':
                list_jobs();
                break;
            case 's':
                remove(r.job_id);
                break;

            default:
                std::cout << "Overseer: Unknown command! Aborted." << std::endl;
        }
    }

    void add(const Request& r)
    {
        auto tmp = std::make_shared<Beholder>(r);
        tmp -> start(std::ref(_msgQueue), /*std::ref(_condVar),*/ std::ref(_mutex));
        _threads.push_back(tmp);
    }

    void remove(std::size_t id)
    {
        if(_threads[id] -> stopped() == false)
            _threads[id] -> stop();
    }

    void list_jobs()
    {
        if(_threads.size() != 0) {
            std::cout <<
            std::setw(4) << std::left <<
            "id" <<
            std::setw(20) << std::left <<
            "ip" <<
            std::setw(8) << std::left <<
            "port" <<
            std::setw(15) << std::left <<
            "interval (ts)" <<
            std::setw(20) << std::left <<
            "e-mail " <<
            std::setw(20) << std::left <<
            "sleeping/working" <<
            std::endl;

            for(auto& b : _threads )
            {
                std::cout <<
                std::setw(4) << std::left <<
                std::distance(_threads.begin(), std::find(_threads.begin(), _threads.end(), b)) <<
                std::setw(20) << std::left <<
                b->ip() <<
                std::setw(8) << std::left <<
                b->port() <<
                std::setw(15) << std::left <<
                b->interval() <<
                std::setw(20) << std::left <<
                b->email() <<
                std::setw(20) << std::left <<
                (b->stopped() ? "sleeping" : "working") <<
                std::endl;
            }
        }
        else
        {
            std::cout << "Overseer: no jobs to list." << std::endl;
        }
    }

    void setUpMailer(GmailCreditenials& creditenials)
    {
        _scribe.setUpMailer(creditenials);
    }
private:
    std::vector<std::shared_ptr<Beholder>> _threads;
    std::deque<Report> _msgQueue;
    Scribe _scribe;
    std::mutex _mutex;
};

#endif
