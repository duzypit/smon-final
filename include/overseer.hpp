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
	Overseer(std::string filename);

    ~Overseer();

    void dispatch(const Request& r);

    void add(const Request& r);

    void remove(std::size_t id);

    void list_jobs();

    void setUpMailer(GmailCreditenials& creditenials);

private:
    std::vector<std::shared_ptr<Beholder>> _threads;
    std::deque<Report> _msgQueue;
    Scribe _scribe;
    std::mutex _mutex;
};

#endif
