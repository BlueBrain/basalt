
.. _program_listing_file__goinfre_src_github.com_tristan0x_basalt_include_basalt_edges.hpp:

Program Listing for File edges.hpp
==================================

|exhale_lsh| :ref:`Return to documentation for file <file__goinfre_src_github.com_tristan0x_basalt_include_basalt_edges.hpp>` (``/goinfre/src/github.com/tristan0x/basalt/include/basalt/edges.hpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*************************************************************************
    * Copyright (C) 2019 Blue Brain Project
    *
    * This file is part of Basalt distributed under the terms of the GNU
    * Lesser General Public License. See top-level LICENSE file for details.
    *************************************************************************/
   #pragma once
   
   #include <basalt/fwd.hpp>
   #include <basalt/status.hpp>
   
   namespace basalt {
   
   template <EdgeOrientation Orientation>
   class Edges {
     public:
       explicit Edges(GraphImpl<Orientation>& pimpl);
   
       EdgeIterator begin(std::size_t position = 0) const;
   
       EdgeIterator end() const;
   
       Status insert(const vertex_uid_t& vertex1, const vertex_uid_t& vertex2, bool commit = false)
           __attribute__((warn_unused_result));
   
       Status insert(const vertex_uid_t& vertex1,
                     const vertex_uid_t& vertex2,
                     const char* data,
                     std::size_t size,
                     bool commit = false) __attribute__((warn_unused_result));
   
       Status insert(const vertex_uid_t& vertex,
                     const vertex_uids_t& vertices,
                     const std::vector<const char*>& data = {},
                     const std::vector<std::size_t>& sizes = {},
                     bool commit = false);
   
       Status insert(const vertex_uid_t& vertex,
                     vertex_t type,
                     const vertex_id_t* vertices,
                     size_t num_vertices,
                     bool create_vertices = false,
                     bool commit = false) __attribute__((warn_unused_result));
   
       Status insert(const vertex_uid_t& vertex,
                     vertex_t type,
                     const std::size_t* vertices,
                     const char* const* vertex_payloads,
                     const std::size_t* vertex_payloads_sizes,
                     size_t num_vertices,
                     bool create_vertices = false,
                     bool commit = false) __attribute__((warn_unused_result));
   
       Status get(const edge_uid_t& edge, std::string* value) const
           __attribute__((warn_unused_result));
   
       Status has(const vertex_uid_t& vertex1, const vertex_uid_t& vertex2, bool& result) const
           __attribute__((warn_unused_result));
   
       Status get(const vertex_uid_t& vertex, vertex_uids_t& edges) const
           __attribute__((warn_unused_result));
   
       Status get(const vertex_uid_t& vertex, vertex_t filter, vertex_uids_t& edges) const
           __attribute__((warn_unused_result));
   
       Status erase(const vertex_uid_t& vertex1, const vertex_uid_t& vertex2, bool commit = false)
           __attribute__((warn_unused_result));
   
       Status erase(const vertex_uid_t& vertex, vertex_t filter, size_t& removed, bool commit = false)
           __attribute__((warn_unused_result));
   
       Status erase(const vertex_uid_t& vertex, std::size_t& removed, bool commit = false)
           __attribute__((warn_unused_result));
   
       Status count(std::size_t& count) const __attribute__((warn_unused_result));
   
       Status clear(bool commit) __attribute__((warn_unused_result));
   
     private:
       GraphImpl<Orientation>& pimpl_;
   };
   
   extern template class Edges<EdgeOrientation::directed>;
   extern template class Edges<EdgeOrientation::undirected>;
   
   }  // namespace basalt
