#pragma once

//#include "taskflow.hpp"


namespace tf {
    struct Segment {

        std::string name;

        std::chrono::time_point<std::chrono::steady_clock> beg;
        std::chrono::time_point<std::chrono::steady_clock> end;

        Segment(
                const std::string& n,
                std::chrono::time_point<std::chrono::steady_clock> b
        );

        Segment(
                const std::string& n,
                std::chrono::time_point<std::chrono::steady_clock> b,
                std::chrono::time_point<std::chrono::steady_clock> e
        );
    };

    // data structure to store the entire execution timeline
    struct Timeline {
        void dump(std::ostream& os);
        std::chrono::time_point<std::chrono::steady_clock> origin;
        std::vector<std::vector<Segment>> segments;
        std::vector<std::stack<std::chrono::time_point<std::chrono::steady_clock>>> stacks;
    };

// ----------------------------------------------------------------------------
  
// class: Topology
class Topology {

  friend struct Timeline;
  friend class Taskflow;
  friend class Executor;


  
  public:

    template <typename P, typename C>
    Topology(Taskflow&, P&&, C&&);
    void on_entry(size_t workerId, Node* node);
    void on_exit(size_t workerId, Node* node);

  private:
    Taskflow& _taskflow;

    std::promise<Timeline> _promise; //change to Timeline

    PassiveVector<Node*> _sources;
    std::function<bool()> _pred;
    std::function<void()> _call;
    std::atomic<size_t> _join_counter {0};

    Timeline _timeline;
};
    void Topology::on_entry(size_t workerId, class tf::Node * node) {
        _timeline.stacks[workerId].push(std::chrono::steady_clock::now());

    }
    void Topology::on_exit(size_t workerId, class tf::Node * node) {
        assert(!_timeline.stacks[workerId].empty());

        auto beg = _timeline.stacks[workerId].top();
        _timeline.stacks[workerId].pop();

        _timeline.segments[workerId].emplace_back(
                node->name(), beg, std::chrono::steady_clock::now()
        );
    }



    Segment::Segment(const std::string &n, std::chrono::time_point<std::chrono::steady_clock> b,
                               std::chrono::time_point<std::chrono::steady_clock> e): name {n}, beg {b}, end {e} {

    }

    Segment::Segment(const std::string &n, std::chrono::time_point<std::chrono::steady_clock> b):  name {n}, beg {b} {

    }

    void Timeline::dump(std::ostream &os) {
        size_t first;
        for(first = 0; first<segments.size(); ++first) {
            if(segments[first].size() > 0) {
                break;
            }
        }
        os << '[';
        for(size_t w=first; w<segments.size(); w++) {
            if(w != first && segments[w].size() > 0) {
                os << ',';
            }
            for(size_t i=0; i<segments[w].size(); i++) {
                os << '{'
                   << "\"cat\":\"ChromeObserver\",";
                // name field
                os << "\"name\":\"";
                if(segments[w][i].name.empty()) {
                    os << w << '_' << i;
                }
                else {
                    os << segments[w][i].name;
                }
                os << "\",";
                // segment field
                os << "\"ph\":\"X\","
                   << "\"pid\":1,"
                   << "\"tid\":" << w << ','
                   << "\"ts\":" << std::chrono::duration_cast<std::chrono::microseconds>(
                        segments[w][i].beg - origin
                ).count() << ','
                   << "\"dur\":" << std::chrono::duration_cast<std::chrono::microseconds>(
                        segments[w][i].end - segments[w][i].beg
                ).count();
                if(i != segments[w].size() - 1) {
                    os << "},";
                }
                else {
                    os << '}';
                }
            }
        }
        os << "]\n";
    }

// Constructor
template <typename P, typename C>
inline Topology::Topology(Taskflow& tf, P&& p, C&& c):
  _taskflow(tf),
  _pred {std::forward<P>(p)},
  _call {std::forward<C>(c)} {
        _timeline.segments.resize(12);
        _timeline.stacks.resize(12);

        for(size_t w=0; w<12; ++w) {
            _timeline.segments[w].reserve(32);
        }

        _timeline.origin = std::chrono::steady_clock::now();
}

}  // end of namespace tf. ----------------------------------------------------
