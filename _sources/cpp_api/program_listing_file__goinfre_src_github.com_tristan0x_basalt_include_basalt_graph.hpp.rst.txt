
.. _program_listing_file__goinfre_src_github.com_tristan0x_basalt_include_basalt_graph.hpp:

Program Listing for File graph.hpp
==================================

|exhale_lsh| :ref:`Return to documentation for file <file__goinfre_src_github.com_tristan0x_basalt_include_basalt_graph.hpp>` (``/goinfre/src/github.com/tristan0x/basalt/include/basalt/graph.hpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*************************************************************************
    * Copyright (C) 2019 Blue Brain Project
    *
    * This file is part of Basalt distributed under the terms of the GNU
    * Lesser General Public License. See top-level LICENSE file for details.
    *************************************************************************/
   #pragma once
   
   #include <fstream>
   #include <iosfwd>
   #include <limits>
   #include <memory>
   #include <string>
   #include <utility>
   #include <vector>
   
   #include <basalt/fwd.hpp>
   #include <basalt/status.hpp>
   
   namespace basalt {
   
   template <EdgeOrientation Orientation>
   class Graph {
     public:
       using vertex_const_iterator_t = std::iterator<std::input_iterator_tag, const vertex_t>;
   
       explicit Graph(const std::string& path);
   
       Graph(const std::string& path, const std::string& config);
   
       ~Graph();
   
       Edges<Orientation>& edges();
   
       Vertices<Orientation>& vertices();
   
       Status commit() __attribute__((warn_unused_result));
   
       std::string statistics() const;
   
     private:
       std::unique_ptr<GraphImpl<Orientation>> pimpl_;
   };
   
   using UndirectedGraph = Graph<EdgeOrientation::undirected>;
   
   using DirectedGraph = Graph<EdgeOrientation::directed>;
   
   extern template class Graph<EdgeOrientation::directed>;
   extern template class Graph<EdgeOrientation::undirected>;
   
   vertex_uid_t make_id(vertex_t type, vertex_id_t id);
   
   }  // namespace basalt
   
   std::ostream& operator<<(std::ostream& ostr, const basalt::vertex_uid_t& id);
   
   std::ostream& operator<<(std::ostream& ostr, const basalt::vertex_uids_t& ids);
   
   std::ostream& operator<<(std::ostream& ostr, const basalt::edge_uid_t& id);
