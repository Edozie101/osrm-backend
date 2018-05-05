#ifndef UNPACKING_CACHE_HPP
#define UNPACKING_CACHE_HPP

#include <boost/optional/optional_io.hpp>
#include <boost/thread.hpp>

#include "../../third_party/compute_detail/lru_cache.hpp"
#include "util/typedefs.hpp"

// sizeof size_t: 8
// sizeof unsigned: 4
// sizeof unchar: 1
// sizeof uint32: 4
namespace osrm
{
namespace engine
{
typedef unsigned char ExcludeIndex;
typedef unsigned Timestamp;
typedef std::tuple<NodeID, NodeID, unsigned char> Key;

template <typename AnnotationType> class UnpackingCache
{
  private:
    boost::compute::detail::lru_cache<std::tuple<NodeID, NodeID, unsigned char>, AnnotationType>
        m_cache;
    unsigned m_current_data_timestamp = 0;

  public:
    UnpackingCache(unsigned timestamp) : m_cache(8738133), m_current_data_timestamp(timestamp){};

    UnpackingCache(std::size_t cache_size, unsigned timestamp)
        : m_cache(cache_size), m_current_data_timestamp(timestamp){};

    void Clear(unsigned new_data_timestamp)
    {
        if (m_current_data_timestamp != new_data_timestamp)
        {
            m_cache.clear();
            m_current_data_timestamp = new_data_timestamp;
        }
    }

    bool IsEdgeInCache(std::tuple<NodeID, NodeID, unsigned char> edge)
    {
        return m_cache.contains(edge);
    }

    void AddEdge(std::tuple<NodeID, NodeID, unsigned char> edge, AnnotationType annotation)
    {
        m_cache.insert(edge, annotation);
    }

    AnnotationType GetAnnotation(std::tuple<NodeID, NodeID, unsigned char> edge)
    {
        boost::optional<AnnotationType> annotation = m_cache.get(edge);
        return annotation ? *annotation : MAXIMAL_EDGE_DURATION;
    }
};
} // engine
} // osrm

#endif // UNPACKING_CACHE_HPP
