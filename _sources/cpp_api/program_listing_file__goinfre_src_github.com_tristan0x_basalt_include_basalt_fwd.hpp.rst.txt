
.. _program_listing_file__goinfre_src_github.com_tristan0x_basalt_include_basalt_fwd.hpp:

Program Listing for File fwd.hpp
================================

|exhale_lsh| :ref:`Return to documentation for file <file__goinfre_src_github.com_tristan0x_basalt_include_basalt_fwd.hpp>` (``/goinfre/src/github.com/tristan0x/basalt/include/basalt/fwd.hpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*************************************************************************
    * Copyright (C) 2019 Blue Brain Project
    *
    * This file is part of Basalt distributed under the terms of the GNU
    * Lesser General Public License. See top-level LICENSE file for details.
    *************************************************************************/
   #pragma once
   
   #include <cstddef>
   #include <utility>
   #include <vector>
   
   namespace basalt {
   
   enum class EdgeOrientation {
       undirected,
       directed,
   };
   
   template <EdgeOrientation Orientation>
   class Edges;
   class EdgeIterator;
   class EdgeIteratorImpl;
   template <EdgeOrientation Orientation>
   class GraphImpl;
   class VertexIteratorImpl;
   class VertexIterator;
   template <EdgeOrientation Orientation>
   class Vertices;
   
   using vertex_t = int;
   using vertex_id_t = std::size_t;
   using vertex_uid_t = std::pair<vertex_t, std::size_t>;
   using edge_uid_t = std::pair<vertex_uid_t, vertex_uid_t>;
   using vertex_uids_t = std::vector<vertex_uid_t>;
   
   }  // namespace basalt
