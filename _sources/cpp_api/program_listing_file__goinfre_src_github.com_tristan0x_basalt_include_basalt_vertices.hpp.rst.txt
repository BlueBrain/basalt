
.. _program_listing_file__goinfre_src_github.com_tristan0x_basalt_include_basalt_vertices.hpp:

Program Listing for File vertices.hpp
=====================================

|exhale_lsh| :ref:`Return to documentation for file <file__goinfre_src_github.com_tristan0x_basalt_include_basalt_vertices.hpp>` (``/goinfre/src/github.com/tristan0x/basalt/include/basalt/vertices.hpp``)

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
   #include <basalt/graph.hpp>
   #include <basalt/status.hpp>
   
   namespace basalt {
   
   template <EdgeOrientation Orientation>
   class Vertices {
     public:
       explicit Vertices(GraphImpl<Orientation>& pimpl);
       VertexIterator begin(std::size_t position = 0) const;
   
       VertexIterator end() const;
   
       Status count(std::size_t& count) const __attribute__((warn_unused_result));
   
       Status count(vertex_t type, std::size_t& count) const __attribute__((warn_unused_result));
   
       Status clear(bool commit) __attribute__((warn_unused_result));
   
       Status insert(const vertex_uid_t& vertex, bool commit = false)
           __attribute__((warn_unused_result));
   
       template <typename Payload>
       Status insert(const vertex_uid_t& vertex, const Payload& data, bool commit = false)
           __attribute__((warn_unused_result));
   
       Status insert(const vertex_uid_t& vertex,
                     const char* data,
                     std::size_t size,
                     bool commit = false) __attribute__((warn_unused_result));
       Status insert(const vertex_t* types,
                     const vertex_id_t* ids,
                     const char* const* payloads,
                     const std::size_t* payloads_sizes,
                     size_t num_vertices,
                     bool commit = false) __attribute__((warn_unused_result));
   
       template <typename T>
       Status get(const vertex_uid_t& vertex, T& payload) const __attribute__((warn_unused_result));
   
       Status get(const vertex_uid_t& vertex, std::string* value) const
           __attribute__((warn_unused_result));
   
       Status has(const vertex_uid_t& vertex, bool& result) const __attribute__((warn_unused_result));
   
       Status erase(const vertex_uid_t& vertex, bool commit = false)
           __attribute__((warn_unused_result));
   
     private:
       GraphImpl<Orientation>& pimpl_;
   };
   
   extern template class Vertices<EdgeOrientation::directed>;
   extern template class Vertices<EdgeOrientation::undirected>;
   
   }  // namespace basalt
   
   #include <basalt/vertices.ipp>
